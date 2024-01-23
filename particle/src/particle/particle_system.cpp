#include <particle/particle_system.hpp>
#include <glh/pipeline.hpp>

#include <numbers>

#include <util/types.hpp>

#include <glad/glad.h>

namespace particle {

namespace {
	struct alignas(16) Particle {
		glm::vec3 position;
		float mass;
		glm::vec3 velocity;
		float life;
		glm::vec3 acceleration;
		float size;
	};

	struct alignas(16) ParticleEmissionSettings {
		glm::vec3 position;
		float position_radius;
		glm::vec3 velocity;
		float velocity_angle_cos;
		float mass_min;
		float mass_max;
		float life_min;
		float life_max;
		float size_min;
		float size_max;
	};
	struct EmitParams {
		uint32_t offset;
		uint32_t count;
		uint32_t seed;
	};

	struct alignas(16) UpdateParams {
		glm::vec3 force;
		float delta_time;
		uint32_t num_particles;
		float gravity;
		float drag;
	};

	struct alignas(16) RenderParams {
		glm::vec4 color;
	};
}

ParticleSystem::ParticleSystem() : rng(std::random_device{}()) {
	particles_buffer[0] = std::make_unique<GlBuffer>(state.k_max_num_particles * sizeof(Particle));
	particles_buffer[1] = std::make_unique<GlBuffer>(state.k_max_num_particles * sizeof(Particle));

	init_pipeline_emit();
	init_pipeline_update();
	init_pipeline_compact();

	init_pipeline_draw();

	ui = std::make_unique<ParticleSystemUI>(state);
}

ParticleSystem::~ParticleSystem() {
	glDeleteVertexArrays(1, &draw_vao);
}

void ParticleSystem::update(float delta_time) {
	ui->update(delta_time);

	static uint32_t emit_counter = 0;
	static uint32_t compact_counter = 0;

	if (ui->executing) {
		if (state.emit_settings.emit_interval > 0 && ++emit_counter == state.emit_settings.emit_interval) {
			do_emit();
			emit_counter = 0;
		}
		if (state.num_particles > 0) {
			do_update(delta_time);
			if (state.emit_settings.compact_interval > 0 && ++compact_counter == state.emit_settings.compact_interval) {
				// eliminate died particles, do optimization, etc.
				do_compact();
				compact_counter = 0;
			}
		}
	}
	if (state.num_particles > 0) {
		do_draw();
	}

	glUseProgram(0);
}

void ParticleSystem::init_pipeline_emit() {
	emit_program = build_compute_program("particle/emit.comp.spv");

	emit_settings_buffer = std::make_unique<GlBuffer>(sizeof(ParticleEmissionSettings), GL_MAP_WRITE_BIT);
	emit_params_buffer = std::make_unique<GlBuffer>(sizeof(EmitParams), GL_MAP_WRITE_BIT);
}

void ParticleSystem::init_pipeline_update() {
	update_program = build_compute_program("particle/update.comp.spv");

	update_params_buffer = std::make_unique<GlBuffer>(sizeof(UpdateParams), GL_MAP_WRITE_BIT);
}

void ParticleSystem::init_pipeline_compact() {
	compact_program = build_compute_program("particle/compact.comp.spv");
	scan1_program = build_compute_program("particle/scan1.comp.spv");
	scan2_program = build_compute_program("particle/scan2.comp.spv");
	scan3_program = build_compute_program("particle/scan3.comp.spv");

	compact_indices_buffer = std::make_unique<GlBuffer>(state.k_max_num_particles * sizeof(uint32_t));
	scan_buffer[0] = std::make_unique<GlBuffer>(state.k_scan_width * sizeof(uint32_t)); // uint block_sums[];
	scan_buffer[1] = std::make_unique<GlBuffer>(sizeof(uint32_t), GL_MAP_READ_BIT); // uint total_sum;
	scan_params_buffer[0] = std::make_unique<GlBuffer>(sizeof(uint32_t), GL_MAP_WRITE_BIT);
	scan_params_buffer[1] = std::make_unique<GlBuffer>(sizeof(uint32_t), GL_MAP_WRITE_BIT);
}

void ParticleSystem::init_pipeline_draw() {
	draw_program = build_graphics_program("particle/draw.vert.spv", "particle/draw.frag.spv");

	draw_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderParams), GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &draw_vao);

	const uint32_t billboard_index[] = { 0, 1, 2, 0, 2, 3 };
	billboard_index_buffer = std::make_unique<GlBuffer>(sizeof(billboard_index), 0, billboard_index);
	glVertexArrayElementBuffer(draw_vao, billboard_index_buffer->id());

	billboard_tex = read_texture("assets/circle.png");
}

void ParticleSystem::do_emit() {
	if (ui->emit_settings_dirty) {
		auto data = emit_settings_buffer->typed_map<ParticleEmissionSettings>(true);
		data->position = state.emit_settings.position;
		data->position_radius = state.emit_settings.position_radius;
		data->velocity = state.emit_settings.velocity;
		data->velocity_angle_cos = std::cos(state.emit_settings.velocity_angle / 180.0f * std::numbers::pi);
		data->life_min = state.emit_settings.life_min;
		data->life_max = state.emit_settings.life_max;
		data->mass_min = state.emit_settings.mass_min;
		data->mass_max = state.emit_settings.mass_max;
		data->size_min = state.emit_settings.size_min;
		data->size_max = state.emit_settings.size_max;
		emit_settings_buffer->unmap();
		ui->emit_settings_dirty = false;
	}

	std::uniform_real_distribution<> rng01(0.0f, 1.0f);
	// randomly determine number of emitted particles
	uint32_t num_emitted =
		state.emit_settings.count_min + rng01(rng) * (state.emit_settings.count_max - state.emit_settings.count_min);
	num_emitted = std::min(num_emitted, state.k_max_num_particles - state.num_particles);
	if (num_emitted == 0) {
		return;
	}

	{
		auto data = emit_params_buffer->typed_map<EmitParams>(true);
		data->offset = state.num_particles;
		data->count = num_emitted;
		data->seed = emit_seed++;
		emit_params_buffer->unmap();
	}
	state.num_particles += num_emitted;

	glUseProgram(emit_program->id());
	uint32_t buffers[] = {
		particles_buffer[curr_particles_index]->id(),
		emit_settings_buffer->id(),
		emit_params_buffer->id(),
	};
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 1, buffers);
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers + 1);

	// workgroup.yz = 1
	// workgroup.x = (1, 2, ...) depends on num_emitted (1~256, 257~512, ...)
	// i.e. use only 1 workgroup when num_emitted is in [1, 256].
	glDispatchCompute((num_emitted + 255) / 256, 1, 1);

	// this compute shader writes shader storage. Use a synchronize() to make sure it is written before using the data.
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleSystem::do_update(float delta_time) {
	{
		auto data = update_params_buffer->typed_map<UpdateParams>(true);
		data->delta_time = delta_time;
		data->num_particles = state.num_particles;
		data->force = state.update_settings.force;
		data->gravity = state.update_settings.gravity;
		data->drag = state.update_settings.drag;
		update_params_buffer->unmap();
	}

	glUseProgram(update_program->id());
	uint32_t buffers[] = {
		particles_buffer[curr_particles_index]->id(),
		update_params_buffer->id(),
	};
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 1, buffers);
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 1, buffers + 1);

	glDispatchCompute((state.num_particles + 255) / 256, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleSystem::do_compact() {
	auto num_blocks = (state.num_particles + 511) / 512;
	{
		auto data0 = scan_params_buffer[0]->typed_map<uint32_t>(true);
		*data0 = state.num_particles;
		scan_params_buffer[0]->unmap();

		auto data1 = scan_params_buffer[1]->typed_map<uint32_t>(true);
		*data1 = num_blocks;
		scan_params_buffer[1]->unmap();
	}

	// scan 1
	{
		glUseProgram(scan1_program->id());
		/**
layout(binding = 0) buffer readonly Particles {
    Particle particles[];
};

layout(binding = 1) buffer writeonly Indices {
    uint indices[];
};

layout(binding = 2) buffer writeonly BlockSums {
    uint block_sums[];
};

layout(binding = 3) uniform ScanParams {
    uint num_particles;
} params;
		*/
		uint32_t buffers[] = {
			particles_buffer[curr_particles_index]->id(),
			compact_indices_buffer->id(),
			scan_buffer[0]->id(),
			scan_params_buffer[0]->id(),
		};
		glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, buffers);
		glBindBuffersBase(GL_UNIFORM_BUFFER, 3, 1, buffers + 3);

		glDispatchCompute(num_blocks, 1, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	// scan 2
	{
		glUseProgram(scan2_program->id());
		/**
layout(binding = 0) buffer BlockSums {
    uint block_sums[];
};

layout(binding = 1) buffer writeonly TotalSum {
    uint total_sum;
};

layout(binding = 2) uniform ScanParams {
    uint size;
} params;
		*/
		uint32_t buffers[] = {
			scan_buffer[0]->id(),
			scan_buffer[1]->id(),
			scan_params_buffer[1]->id(),
		};
		glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 2, buffers);
		glBindBuffersBase(GL_UNIFORM_BUFFER, 2, 1, buffers + 2);

		glDispatchCompute(1, 1, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	// scan 3
	if (num_blocks > 1) {
		glUseProgram(scan3_program->id());
/**
layout(binding = 0) buffer Indices {
    uint indices[];
};

layout(binding = 1) buffer readonly BlockSums {
    uint block_sums[];
};

layout(binding = 2) uniform ScanParams {
    uint size;
} params;
*/
		uint32_t buffers[] = {
			compact_indices_buffer->id(),
			scan_buffer[0]->id(),
			scan_params_buffer[0]->id(),
		};
		glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 2, buffers);
		glBindBuffersBase(GL_UNIFORM_BUFFER, 2, 1, buffers + 2);

		glDispatchCompute(num_blocks - 1, 1, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	// compact
	{
		glUseProgram(compact_program->id());
		/**
		 * 
layout(binding = 0) buffer readonly ParticlesOld {
    Particle particles_old[];
};

layout(binding = 1) buffer readonly Indices {
    uint indices[];
};

layout(binding = 2) buffer writeonly Particles {
    Particle particles[];
};

layout(binding = 3) uniform CompactParams {
    uint num_particles;
} params;
		*/
		uint32_t buffers[] = {
			particles_buffer[curr_particles_index]->id(),
			compact_indices_buffer->id(),
			particles_buffer[curr_particles_index ^ 1]->id(),
			scan_params_buffer[0]->id(),
		};
		glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, buffers);
		glBindBuffersBase(GL_UNIFORM_BUFFER, 3, 1, buffers + 3);

		glDispatchCompute(num_blocks, 1, 1);
	}

	// the particles are "compacted"
	curr_particles_index ^= 1;

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
	auto fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
	glDeleteSync(fence);

	// read num_particles
	auto data = scan_buffer[1]->typed_map<uint32_t>();
	state.num_particles = *data;
	scan_buffer[1]->unmap();
}

void ParticleSystem::do_draw() {
	
	// update render params
	if (ui->render_settings_dirty) {
		// draw_params_buffer is RenderParams in draw.frag
		auto data = draw_params_buffer->typed_map<RenderParams>(true);
		// assign value to data (the memory is mapped so that CPU can access)
		data->color = state.render_settings.color;
		// unmap the memory to indicate the GPU to reload it
		draw_params_buffer->unmap();
		ui->render_settings_dirty = false;
	}

	// glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(draw_program->id());
	uint32_t buffers[] = {
		particles_buffer[curr_particles_index]->id(),
		camera_buffer->id(),
		draw_params_buffer->id(),
	};

	// draw.vert:
	// 0: Particles
	// 1: Camera
	// draw.frag:
	// 2: RenderParams
	// 3: particle_tex
	
	// bind ParticlesOJ
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 1, buffers);

	// bind Camera & RenderParams
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers + 1);

	// bind particle_tex
	glBindTextureUnit(3, billboard_tex->id());
	glBindVertexArray(draw_vao);

	// draw 4 vertices (2 triangles, 6 vertices transferred)
	auto elements_cnt = billboard_index_buffer->size() / sizeof(uint32_t);
	assert(elements_cnt == 6);
	glDrawElementsInstanced(GL_TRIANGLES, elements_cnt, GL_UNSIGNED_INT, nullptr, state.num_particles);

	glBindVertexArray(0);
}

}
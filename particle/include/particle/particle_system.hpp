#pragma once

#include <memory>
#include <random>

#include <util/types.hpp>

#include <glh/gl_buffer.hpp>
#include <glh/gl_texture2d.hpp>
#include <glh/program.hpp>

#include <particle/particle_ui.hpp>
#include <particle/particle_state.hpp>

namespace particle {
	using namespace renderer;

class ParticleSystem {
public:
	ParticleSystem();
	~ParticleSystem();

	void set_camera_buffer(const GlBuffer* camera_buffer) { this->camera_buffer = camera_buffer; }

	void update(float delta_time);

private:
	void init_pipeline_emit();
	void init_pipeline_update();
	void init_pipeline_compact();
	void init_pipeline_draw();

	void do_emit();
	void do_update(float delta_time);
	void do_compact();
	void do_draw();

	std::mt19937 rng;

	std::unique_ptr<ParticleSystemUI> ui;
	ParticleSystemState state;

	uint32_t curr_particles_index = 0;
	std::unique_ptr<GlBuffer> particles_buffer[2];

	std::unique_ptr<GlComputeProgram> emit_program;
	std::unique_ptr<GlBuffer> emit_settings_buffer;
	std::unique_ptr<GlBuffer> emit_params_buffer;
	uint32_t emit_seed = 0;

	std::unique_ptr<GlComputeProgram> update_program;
	std::unique_ptr<GlBuffer> update_params_buffer;

	std::unique_ptr<GlComputeProgram> scan1_program;
	std::unique_ptr<GlComputeProgram> scan2_program;
	std::unique_ptr<GlComputeProgram> scan3_program;
	std::unique_ptr<GlComputeProgram> compact_program;
	std::unique_ptr<GlBuffer> compact_indices_buffer;
	std::unique_ptr<GlBuffer> scan_buffer[2];
	std::unique_ptr<GlBuffer> scan_params_buffer[2];

	std::unique_ptr<GlGraphicsProgram> draw_program;
	std::unique_ptr<GlBuffer> draw_params_buffer;
	uint32_t draw_vao = 0;
	std::unique_ptr<GlBuffer> billboard_index_buffer;
	std::unique_ptr<GlTexture2D> billboard_tex;

	const GlBuffer* camera_buffer = nullptr;
};

}

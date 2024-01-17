#include <brep/brep_system.hpp>

#include <glad/glad.h>

#include <glh/pipeline.hpp>

namespace brep {

namespace {
	struct alignas(16) RenderParams {
		Vector4 color;
		int two_side;
	};
}

BrepSystem::BrepSystem()
{
	ui = std::make_unique<BrepSystemUI>(state);

	draw_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderParams), GL_MAP_WRITE_BIT);
	draw_program = build_graphics_program("plain/plain.vert.spv", "plain/plain.frag.spv");

	glCreateVertexArrays(1, &wireframe_vao);
	glCreateVertexArrays(1, &tri_vao);
}

BrepSystem::~BrepSystem() {
	glDeleteVertexArrays(1, &wireframe_vao);
	glDeleteVertexArrays(1, &tri_vao);
}

void BrepSystem::update(float delta_time)
{
	ui->update(delta_time);

	if (state.solids.empty() == false) {
		// generate vbo
		if (ui->draw_dirty) {
			if (state.render_wireframe) {
				wireframe_vbo = draw_wireframes(state.cur_solid, wireframe_vao);
			} else {
				tri_vbo = draw_triangles(state.cur_solid, tri_vao);
			}
			ui->draw_dirty = false;
		}

		// draw call
		do_draw();
	}
}

void BrepSystem::do_draw() {
	// update render params
	if (ui->render_settings_dirty) {
		// draw_params_buffer is RenderParams in draw.frag
		auto data = draw_params_buffer->typed_map<RenderParams>(true);
		// assign value to data (the memory is mapped so that CPU can access)
		data->color = state.render_settings.color;
		data->two_side = state.render_settings.two_side ? 1 : 0;
		// unmap the memory to indicate the GPU to reload it
		draw_params_buffer->unmap();
		ui->render_settings_dirty = false;
	}

	glEnable(GL_DEPTH_TEST);

	glUseProgram(draw_program->id());
	uint32_t buffers[] = {
		camera_buffer->id(),
		draw_params_buffer->id(),
	};

	// plain.vert:
	// 1: Camera
	// plain.frag:
	// 2: RenderParams
	// bind Camera & RenderParams
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers);

	if (state.render_wireframe && wireframe_vbo != nullptr) {
		glBindVertexArray(wireframe_vao);
		glDrawArrays(GL_LINES, 0, wireframe_vbo->size());
	} else if (tri_vbo != nullptr) {
		glBindVertexArray(tri_vao);
		glDrawArrays(GL_TRIANGLES, 0, tri_vbo->size());
	}

	glBindVertexArray(0);
}

}

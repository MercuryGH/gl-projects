#include <pathtrace/pathtrace_system.hpp>

#include <glad/glad.h>
#include <glh/pipeline.hpp>

namespace pathtrace {

PathtraceSystem::PathtraceSystem(uint32_t width, uint32_t height): scene(width, height)
{
	ui = std::make_unique<PathtraceSystemUI>(state);

	display_program = build_graphics_program("display/display.vert.spv", "display/display.frag.spv");
	// display_texture = read_texture()
	glCreateVertexArrays(1, &empty_vao);
}

PathtraceSystem::~PathtraceSystem()
{
	glDeleteVertexArrays(1, &empty_vao);
}

void PathtraceSystem::update(float delta_time)
{
	ui->update(delta_time);

	if (ui->scene_dirty) {
		// re-import scene
		// scene.import_scene(state.cur_obj_file_path());
		// state.n_triangles = scene.get_n_triangles();

		ui->scene_dirty = false;
	}

	if (ui->draw_dirty) {
		// re-pathtrace scene
		// scene.vpv_transform(camera_data);
		// scene.clear_zbuf(); // don't clear z buffer here. Hi-z uses depth info from previous rasterization

		timer.start();

		scene.render();

		timer.stop();
		state.last_render_time = timer.elapsed_milliseconds();

		// scene.write_result_to_texture();

		ui->draw_dirty = false;
	}

	// draw call
	display(scene.get_display_texture());
}

void PathtraceSystem::display(const GlTexture2D& display_texture) {
	glUseProgram(display_program->id());

	glBindVertexArray(empty_vao);
	glBindTexture(GL_TEXTURE_2D, display_texture.id());
	glDrawArrays(GL_TRIANGLES, 0, 3);
}


}

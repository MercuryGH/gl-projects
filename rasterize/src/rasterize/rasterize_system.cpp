#include <rasterize/rasterize_system.hpp>

#include <glad/glad.h>
#include <glh/pipeline.hpp>

namespace rasterize {

RasterizeSystem::RasterizeSystem(const CameraData& camera_data, uint32_t width, uint32_t height):
camera_data(camera_data), scene(width, height)
{
	ui = std::make_unique<RasterizeSystemUI>(state);

	display_program = build_graphics_program("display/display.vert.spv", "display/display.frag.spv");
	// display_texture = read_texture()
	glCreateVertexArrays(1, &empty_vao);
}

RasterizeSystem::~RasterizeSystem()
{
	glDeleteVertexArrays(1, &empty_vao);
}

void RasterizeSystem::update(float delta_time)
{
	ui->update(delta_time);

	if (ui->model_dirty) {
		// re-import model
		scene.import_obj_model(state.cur_obj_file_path());
		state.n_triangles = scene.get_n_triangles();

		ui->model_dirty = false;
	}

	if (ui->draw_dirty) {
		// re-rasterize scene
		scene.vpv_transform(camera_data);
		scene.clear_zbuf();

		timer.start();

		state.rasterize(scene);

		timer.stop();
		state.last_rasterize_time = timer.elapsed_milliseconds();

		scene.write_result_to_texture();

		ui->draw_dirty = false;
	}

	// draw call
	display(scene.get_display_texture());
}

void RasterizeSystem::display(const GlTexture2D& display_texture) {
	glUseProgram(display_program->id());

	glBindVertexArray(empty_vao);
	glBindTexture(GL_TEXTURE_2D, display_texture.id());
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

}

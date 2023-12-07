#include <rasterize/rasterize_system.hpp>

#include <glad/glad.h>
#include <glh/pipeline.hpp>

namespace rasterize {

RasterizeSystem::RasterizeSystem(CameraData& camera_data, uint32_t width, uint32_t height):
scene(camera_data, width, height)
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
		// reimport model
		scene.import_obj_model(state.cur_obj_file_path());

		ui->model_dirty = false;
	}

	if (ui->draw_dirty) {
		// re-render scene
		scene.vpv_transform();
		scene.clear_zbuf();
		state.render(scene);
		scene.write_render_result_to_texture();

		ui->draw_dirty = false;
	}
	// draw call
	do_display(scene.get_render_display_texture());
}

void RasterizeSystem::do_display(const GlTexture2D& display_texture) {
	glUseProgram(display_program->id());

	glBindVertexArray(empty_vao);
	glBindTexture(GL_TEXTURE_2D, display_texture.id());
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

}

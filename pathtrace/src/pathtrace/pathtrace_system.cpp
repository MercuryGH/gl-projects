#include <pathtrace/pathtrace_system.hpp>

#include <thread>

#include <glad/glad.h>
#include <glh/pipeline.hpp>
#include <scene/pinhole_camera.hpp>

namespace pathtrace {

namespace {
	struct alignas(16) RenderParams {
		int gamma_correction; // bool is probably not works in certain case. use 32-bit aligned data instead
	};
}

PathtraceSystem::PathtraceSystem(Window& window): window(window)
{
    ui = std::make_unique<PathtraceSystemUI>(state);

	draw_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderParams), GL_MAP_WRITE_BIT);
    display_program = build_graphics_program("display/display.vert.spv", "display/display.frag.spv");

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
        bool read_from_cache = !(ui->import_scene_from_disk);

        std::string parent_dir;
        if (read_from_cache) {
            parent_dir = "assets/cached/scene/";
            parent_dir += std::string(state.cur_scene_name());
        } else {
            parent_dir = state.last_import_scene_path_from_disk;
        }

        // TODO: debug only
        // parent_dir = "/home/xinghai/codes/opengl/gl-projects/assets/scene/example-scenes-cg23/veach-mis";
        // read_from_cache = false;

        std::string obj_file_path = state.get_file_path(parent_dir, ".obj");
        std::string mtl_file_path = state.get_file_path(parent_dir, ".mtl");
        std::string xml_file_path = state.get_file_path(parent_dir, ".xml");

        timer.start();

        scene.import_scene_file(obj_file_path.c_str(), mtl_file_path.c_str(), xml_file_path.c_str(), read_from_cache);

        timer.stop();
        state.last_import_time = timer.elapsed_milliseconds();

        ui->import_scene_from_disk = false;
        ui->scene_dirty = false;
        state.n_triangles = scene.get_total_n_tris();

        const auto& pinhole_camera = scene.camera_data();
        for (int i = 0; i < 3; i++) {
            state.camera_settings.eye[i] = pinhole_camera.eye[i];
            state.camera_settings.lookat[i] = pinhole_camera.lookat[i];
            state.camera_settings.up[i] = pinhole_camera.up[i];
        }
        state.camera_settings.fovy = pinhole_camera.fovy;
        state.camera_settings.width = pinhole_camera.width;
        state.camera_settings.height = pinhole_camera.height;
        window.set_window_size(pinhole_camera.width, pinhole_camera.height);
    }

    if (ui->camera_dirty) {
        Vector3 eye, lookat, up;
        for (int i = 0; i < 3; i++) {
            eye[i] = state.camera_settings.eye[i];
            lookat[i] = state.camera_settings.lookat[i];
            up[i] = state.camera_settings.up[i];
        }
        PinholeCamera pinhole_camera(eye, lookat, up, state.camera_settings.fovy, state.camera_settings.width, state.camera_settings.height);
        scene.set_camera(pinhole_camera);
        window.set_window_size(pinhole_camera.width, pinhole_camera.height);
        ui->camera_dirty = false;
    }

    scene.enable_realtime_update_result(state.realtime_update_result);
    if (ui->draw_dirty) {
        std::thread([&] {
            timer.start();

            scene.render(state.spp);

            timer.stop();
            state.last_render_time = timer.elapsed_milliseconds();
        }).detach();

        ui->draw_dirty = false;
    }

    // update rendering result
    if (scene.is_rendering()) {
        float cur_tick_process = scene.get_spp_progress();
        if (cur_tick_process > state.spp_progress) {
            scene.assign_display_texture_data();
        }
        state.spp_progress = cur_tick_process;
    }
    if (scene.is_rendering() == false && state.is_rendering == true) {
        // last draw
        scene.assign_display_texture_data();
        scene.save_result_to_png_file();
    }
    state.is_rendering = scene.is_rendering();

    // draw call
    display(scene.get_display_texture());
}

void PathtraceSystem::display(const GlTexture2D& display_texture) {
	if (ui->render_settings_dirty) {
		auto data = draw_params_buffer->typed_map<RenderParams>(true);
		data->gamma_correction = state.render_settings.gamma_correction ? 1 : 0; // update value
		draw_params_buffer->unmap();
		ui->render_settings_dirty = false;
	}

    glUseProgram(display_program->id());
    uint32_t buffers[] = {
		draw_params_buffer->id(),
	};
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 1, buffers);

    glBindVertexArray(empty_vao);
    glBindTexture(GL_TEXTURE_2D, display_texture.id());
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


}

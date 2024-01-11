#include <pathtrace/pathtrace_system.hpp>

#include <glad/glad.h>
#include <glh/pipeline.hpp>

namespace pathtrace {

PathtraceSystem::PathtraceSystem()
{
    ui = std::make_unique<PathtraceSystemUI>(state);

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

        std::string obj_file_path = state.get_file_path(parent_dir, ".obj");
        std::string mtl_file_path = state.get_file_path(parent_dir, ".mtl");
        std::string xml_file_path = state.get_file_path(parent_dir, ".xml");

        timer.start();

        scene.import_scene_file(obj_file_path.c_str(), mtl_file_path.c_str(), xml_file_path.c_str(), read_from_cache);

        timer.stop();
        state.last_import_time = timer.elapsed_milliseconds();

        ui->import_scene_from_disk = false;
        ui->scene_dirty = false;
    }

    if (ui->draw_dirty) {
        timer.start();

        scene.render(state.spp);

        timer.stop();
        state.last_render_time = timer.elapsed_milliseconds();

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

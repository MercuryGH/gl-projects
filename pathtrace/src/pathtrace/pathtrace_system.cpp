#include <pathtrace/pathtrace_system.hpp>

#include <glad/glad.h>
#include <glh/pipeline.hpp>

namespace pathtrace {

PathtraceSystem::PathtraceSystem(uint32_t width, uint32_t height): scene(width, height)
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
    std::string ret = state.get_file_path("123", "456");
    printf("%s\n", ret.c_str());
    return;

    if (ui->scene_dirty) {
        bool read_from_cache = !(ui->import_scene_from_disk && state.cur_scene_id == 0);

        // if () {
        //     const char* path = state.last_import_scene_path_from_disk;
        //     ui->import_scene_from_disk = false;

        // } else {
        //     // import from cache
        //     std::string obj_file_path = state.cached_scene_file_path(".obj");
        //     std::string mtl_file_path = state.cached_scene_file_path(".mtl");
        //     std::string xml_file_path = state.cached_scene_file_path(".xml");
        // }

        // scene.import_scene_file(obj_file_path.c_str(), mtl_file_path.c_str(), xml_file_path.c_str(), read_from_cache);

        ui->scene_dirty = false;
    }

    if (ui->draw_dirty) {
        // re-pathtrace scene

        timer.start();

        scene.render(state.spp);

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

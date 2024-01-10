#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace pathtrace {

namespace {
    const char* scene_dir = "scene/";
    const char* scene_names[] = {
        "classic-cornell-box",
        "cornell-box",
    };
}

class PathtraceSystemState {
    friend class PathtraceSystemUI;
    friend class PathtraceSystem;

    const char* cur_scene_name() const {
        return scene_names[cur_scene_id];
    }

    std::string get_file_path(std::string parent_dir_str, std::string postfix) const {
        // std::string scene_name = std::string(cur_scene_name());

        fs::path file(scene_names[0]);
        fs::path parent_dir("scene");
        fs::path full_path = parent_dir / file;

        // path string contains utf-8 chars
        // std::u8string path_str(full_path.u8string());

        // assume path string is simple
        std::string path_str = full_path.generic_string();

        return path_str;
    }

private:
    int cur_scene_id{ 0 }; // selectd cached scene
    const char* last_import_scene_path_from_disk{ nullptr };

    int spp{ 20 }; // #sample per pixel

    double last_render_time{ 0 };
    int n_triangles{ 0 };
};

}

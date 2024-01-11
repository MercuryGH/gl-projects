#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace pathtrace {

namespace {
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

    std::string get_file_path(std::string scene_dir_str, std::string postfix) const {
        fs::path parent_dir(scene_dir_str);
        std::string scene_name = parent_dir.filename().generic_string();
        fs::path file_name = scene_name + postfix;

        fs::path full_path = parent_dir / file_name;

        // assume path string is simple
        std::string path_str = full_path.generic_string();

        return path_str;
    }

private:
    int cur_scene_id{ 0 }; // selectd cached scene
    const char* last_import_scene_path_from_disk{ nullptr };

    int spp{ 5 }; // #sample per pixel

    double last_import_time{ 0 };
    double last_render_time{ 0 };
    int n_triangles{ 0 };
};

}

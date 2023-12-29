#pragma once

namespace pathtrace {

namespace {
    const char* scene_file_paths[] = {
        "assets/scene/example-scenes-cg23/cube.obj",
        "assets/scene/example-scenes-cg23/obj/sphere.obj",
        "assets/scene/example-scenes-cg23/obj/bunny2k.obj",
        "assets/scene/example-scenes-cg23/obj/armadillo1m.obj",
        "assets/scene/example-scenes-cg23/obj/armadillo1m-cull.obj",
    };
}

class PathtraceSystemState {
    friend class PathtraceSystemUI;
    friend class PathtraceSystem;

    const char* cur_scene_file_path() const {
        return scene_file_paths[cur_scene_id];
    }

private:
    int cur_scene_id{ 0 }; // select obj model
    int spp{ 20 }; // #sample per pixel

    double last_render_time{ 0 };
    int n_triangles{ 0 };
};

}

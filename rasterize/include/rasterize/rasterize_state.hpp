#pragma once

#include <scene/scene.hpp>

#include <functional>

namespace rasterize {

namespace {
    const char* obj_file_paths[] = {
        "assets/cached/obj/cube.obj",
        "assets/cached/obj/sphere.obj",
        "assets/cached/obj/bunny2k.obj",
        "assets/cached/obj/armadillo1m.obj",
        "assets/cached/obj/armadillo1m-cull.obj",
    };

    const std::function<void(Scene&)> rasterize_funcs[] = {
        &Scene::rasterize_basic,
        &Scene::rasterize_hierarchical,
        &Scene::rasterize_octree,
    };
}

class RasterizeSystemState {
    friend class RasterizeSystemUI;
    friend class RasterizeSystem;

    const char* cur_obj_file_path() const {
        return obj_file_paths[cur_model_id];
    }

    void rasterize(Scene& scene) const {
        rasterize_funcs[cur_algo_id](scene);
    }

private:
    int cur_model_id{ 0 }; // select obj model
    int cur_algo_id{ 0 }; // select rasterize func

    double last_rasterize_time{ 0 };

    int n_triangles{ 0 };
};

}

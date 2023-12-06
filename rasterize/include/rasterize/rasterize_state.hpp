#pragma once

#include <util/types.hpp>

#include <scene/scene.hpp>

#include <functional>

namespace rasterize {

namespace {
    const char* obj_file_paths[] = {
        "assets/obj/sphere.obj",
        "assets/obj/bunny.obj",
        "assets/obj/armadillo.obj",
        "assets/obj/dragon.obj",
    };

    const std::function<void(Scene&)> render_funcs[] = {
        &Scene::render_basic,
        &Scene::render_hierarchical,
        &Scene::render_octree,
    };
}

class RasterizeSystemState {
    friend class RasterizeSystemUI;
    friend class RasterizeSystem;

    const char* cur_obj_file_path() const {
        return obj_file_paths[cur_model_id];
    }

    void render(Scene& scene) const {
        render_funcs[cur_algo_id](scene);
    }

private:
    int cur_model_id{ 0 }; // select obj model
    int cur_algo_id{ 0 }; // select render func
};

}
#pragma once

#include <functional>

#include <glh/resource.hpp>
#include <camera/camera.hpp>

// #include <scene/triangle.hpp>
// #include <zbuffer/zbuffer.hpp>

namespace pathtrace {

class Scene {
public:
    Scene(uint32_t width, uint32_t height);

    void import_scene_file(const char* path);

    void render();

    const renderer::GlTexture2D& get_display_texture();
    void write_result_to_texture();

    void vpv_transform(const renderer::CameraData& camera_data);

    // int get_n_triangles() const { return triangles.size(); }

private:
//     void rasterize_hierarchical_triangles(const std::vector<Triangle>& triangles);
//
//     std::vector<Triangle> triangles;
//     std::vector<Triangle> transformed_triangles; // tris after mvpv transforms
//
//     ZBuffer z_buf;

    renderer::GlTexture2D display_texture;

    int n_culled_triangles{ 0 }; // debug data
};

}

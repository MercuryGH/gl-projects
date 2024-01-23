#pragma once

#include <functional>

#include <glh/gl_texture2d.hpp>
#include <camera/camera.hpp>

#include <scene/triangle.hpp>
#include <zbuffer/zbuffer.hpp>

namespace rasterize {

class Scene {
public:
    Scene(uint32_t width, uint32_t height);

    void import_obj_model(const char* path);

    void rasterize_basic();
    void rasterize_hierarchical();
    void rasterize_octree();
    void build_octree_and_rasterize(const std::vector<Triangle>& triangles);

    const renderer::GlTexture2D& get_display_texture();
    void write_result_to_texture();

    void clear_zbuf();
    void vpv_transform(const renderer::CameraData& camera_data);

    int get_n_triangles() const { return triangles.size(); }

private:
    void rasterize_hierarchical_triangles(const std::vector<Triangle>& triangles);

    std::vector<Triangle> triangles;
    std::vector<Triangle> transformed_triangles; // tris after mvpv transforms

    ZBuffer z_buf;

    renderer::GlTexture2D display_texture;

    int n_culled_triangles{ 0 }; // debug data
};

}

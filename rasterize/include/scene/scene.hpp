#pragma once

#include <functional>

#include <glh/resource.hpp>
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

    const renderer::GlTexture2D& get_display_texture();
    void write_result_to_texture();

    void clear_zbuf();
    void vpv_transform(const renderer::CameraData& camera_data);

    int get_n_triangles() const { return triangles.size(); }

private:
    std::vector<Triangle> triangles; // can be replaced with vector of models
    std::vector<Triangle> transformed_triangles; // tris after mvpv transforms

    ZBuffer z_buf;

    renderer::GlTexture2D display_texture;
};

}

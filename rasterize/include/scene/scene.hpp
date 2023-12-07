#pragma once

#include <functional>

#include <glh/resource.hpp>
#include <camera/camera.hpp>

#include <scene/model.hpp>
#include <scene/triangle.hpp>
#include <zbuffer/zbuffer.hpp>

namespace rasterize {

class Scene {
public:
    Scene(CameraData& camera_data, uint32_t width, uint32_t height);

    void import_obj_model(const char* path);

    void render_basic();
    void render_hierarchical();
    void render_octree();

    void render();
    const GlTexture2D& get_render_display_texture();
    void write_render_result_to_texture();

    void clear_zbuf();
    void vpv_transform();

private:
    std::vector<Triangle> triangles; // can be replaced with vector of models
    std::vector<Triangle> transformed_triangles; // tris after mvpv transforms

    ZBuffer z_buf;

    CameraData& camera;
    GlTexture2D display_texture;
};

}

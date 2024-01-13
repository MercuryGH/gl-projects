#pragma once

#include <functional>

#include <glh/resource.hpp>
#include <util/progress_percentage.hpp>

#include <material/material.hpp>
#include <geometry/hittable.hpp>
#include <light/area_light_group.hpp>
#include <scene/screen_buffer.hpp>
#include <scene/pinhole_camera.hpp>

namespace pathtrace {

class Scene {
public:
    Scene() {}
    Scene(uint32_t width, uint32_t height);
    ~Scene();
    void clear();
    void set_size(uint32_t width, uint32_t height);

    void import_scene_file(const char* obj_file_path, const char* mtl_file_path, const char* xml_file_path, bool read_from_cache=false);

    void render(int spp);

    void assign_display_texture_data() { display_texture.set_data(display_texture_buf.data()); }
    const renderer::GlTexture2D& get_display_texture();

    PinholeCamera& camera_data() { return camera; }

    float get_spp_progress() const { return spp_progress_percentage.get_progress(); }
    bool is_rendering() const { return rendering; }

private:
    Vector3 path_tracing(const Ray& ray, const IHittable& world, int x, int y);
    void write_result_to_texture_buf();

    // double buffer for multithreading R/W
    ScreenBuffer buf;
    int write_buffer_index{ 0 };

    int cur_spp{ 0 };
    bool rendering{ false };

    std::vector<IHittable*> objects;
    IHittable* bvh_root{ nullptr };
    std::vector<IMaterial*> materials;
    AreaLightGroup area_lights;

    PinholeCamera camera;

    std::vector<uint8_t> display_texture_buf;
    renderer::GlTexture2D display_texture;

    util::ProgressPercentage spp_progress_percentage;
};

}

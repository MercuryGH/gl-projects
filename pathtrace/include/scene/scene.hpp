#pragma once

#include <functional>

#include <glh/gl_buffer.hpp>
#include <glh/gl_texture2d.hpp>
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
    void set_camera(PinholeCamera camera) { this->camera = camera; set_size(camera.width, camera.height); }

    float get_spp_progress() const { return spp_progress_percentage.get_progress(); }
    bool is_rendering() const { return rendering; }
    void enable_realtime_update_result(bool flag) { update_result = flag; }

    const std::vector<IHittable*>& object_list() const { return objects; }
    int get_total_n_tris() const { return total_n_tris; }

    void save_result_to_png_file() { display_texture.dump_png_file(); }

private:
    Vector3 path_tracing(const Ray& ray, const IHittable& world, int x, int y);
    void write_result_to_texture_buf();

    // double buffer for multithreading R/W
    ScreenBuffer buf;
    int write_buffer_index{ 0 };

    int cur_spp{ 0 };
    bool rendering{ false };
    bool update_result{ true };

    std::vector<IHittable*> objects;
    int total_n_tris{ 0 };
    IHittable* bvh_root{ nullptr };
    std::vector<AMaterial*> materials;
    AreaLightGroup area_lights;

    PinholeCamera camera;

    std::vector<uint8_t> display_texture_buf;
    renderer::GlTexture2D display_texture;

    util::ProgressPercentage spp_progress_percentage;
};

}

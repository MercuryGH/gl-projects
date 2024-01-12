#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>

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

    const renderer::GlTexture2D& get_display_texture();
    void write_result_to_texture();
    PinholeCamera& camera_data() { return camera; }

    float get_spp_progress() { return spp_progress_percentage.get_progress(); }
    bool is_rendering() const { return rendering; }

private:
    Vector3 path_tracing(const Ray& ray, const IHittable& world, int x, int y);

    // double buffer for multithreading R/W
    std::condition_variable cv;
    std::mutex mtx;
    ScreenBuffer bufs[2];
    int write_buffer_index{ 0 };
    ScreenBuffer& get_read_buffer() { return bufs[1 - write_buffer_index]; }
    ScreenBuffer& get_write_buffer() { return bufs[write_buffer_index]; }
    void swap_buffer() { write_buffer_index = 1 - write_buffer_index; }
    void copy_buffer_w2r() { get_read_buffer() = get_write_buffer(); }
    bool main_thread_read{ false };
    bool rendering{ false };

    int cur_spp{ 0 };

    std::vector<IHittable*> objects;
    IHittable* bvh_root{ nullptr };
    std::vector<IMaterial*> materials;
    AreaLightGroup area_lights;

    PinholeCamera camera;

    renderer::GlTexture2D display_texture;

    util::ProgressPercentage spp_progress_percentage;
};

}

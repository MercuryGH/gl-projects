#pragma once

#include <functional>

#include <glh/resource.hpp>

#include <material/material.hpp>
#include <geometry/hittable.hpp>
#include <light/area_light_group.hpp>
#include <scene/screen_buffer.hpp>
#include <scene/pinhole_camera.hpp>

namespace pathtrace {

class Scene {
public:
    Scene(uint32_t width, uint32_t height);
    ~Scene();
    void clear();

    void import_scene_file(const char* obj_file_path, const char* xml_file_path);

    void render();

    const renderer::GlTexture2D& get_display_texture();
    void write_result_to_texture(int spp);
    PinholeCamera& camera_data() { return camera; }

private:
    Vector3 path_tracing(const Ray& ray, const IHittable& world);
    Vector3 sample_light(Vector3 wo, const IHittable& world, const HitRecord& hit_record);

    std::vector<IHittable*> objects;
    IHittable* bvh_root{ nullptr };
    std::vector<IMaterial*> materials;
    AreaLightGroup area_lights;

    PinholeCamera camera;

    ScreenBuffer buf;
    renderer::GlTexture2D display_texture;
};

}

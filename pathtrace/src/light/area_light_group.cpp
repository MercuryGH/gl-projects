#include <light/area_light_group.hpp>

#include <util/math.hpp>
#include <util/random.hpp>
#include <material/material.hpp>

namespace pathtrace {

namespace {
    template<typename T>
    T uniform_sample(std::vector<T> list) {
        const int n = list.size();
        int random_index = util::get_uniform_int_distribution(0, n - 1);
        return list.at(random_index);
    }
}

AreaLightGroup::~AreaLightGroup() {
    for (auto area_light : area_lights) {
        delete area_light;
    }
    area_lights.clear();
}

std::pair<Vector3, ScalarType> AreaLightGroup::uniform_sample_ray(Vector3 wo, const IHittable& world, const HitRecord& prev_record, HitRecord& light_record) const {
    const int n_lights = area_lights.size();
    IAreaLight* light = uniform_sample(area_lights);

    auto [ wi, pdf ] = light->uniform_sample_ray(wo, world, prev_record, light_record);
    pdf *= (1.0f / n_lights);
    return { wi, pdf };
}

ScalarType AreaLightGroup::pdf(const Ray& ray, const IHittable& world, HitRecord& light_record) const {
    const int n_lights = area_lights.size();
    bool ray_hits = world.hit(ray, Vector2{ k_eps, k_max }, light_record);

    // TODO: epsilon equal
    bool hits_light = ray_hits && light_record.material->light_emitted() != Vector3{ 0, 0, 0 };
    bool hits_geometry = ray_hits && light_record.hit_geometry != nullptr;
    ScalarType light_cosine = -glm::dot(ray.dir, light_record.normal);
    bool cosine_judge = light_cosine > 0;

    // hits area light
    if (hits_light && hits_geometry && cosine_judge) {
        ScalarType sq_distance = glm::dot(ray.origin - light_record.pos, ray.origin - light_record.pos);
        ScalarType area = light_record.hit_geometry->get_area();
        ScalarType pdf = sq_distance / (area * light_cosine);
        pdf *= (1.0f / n_lights);
        return pdf;
    }

    return 0.0f;
}

}
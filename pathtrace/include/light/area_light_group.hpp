#pragma once

#include <vector>
#include <unordered_map>

#include <light/ray.hpp>
#include <light/hit_record.hpp>
#include <light/area_light.hpp>

namespace pathtrace {

class AreaLightGroup {
public:
    AreaLightGroup() {}
    AreaLightGroup(const std::vector<IAreaLight*>& area_lights): area_lights(area_lights) {}
    ~AreaLightGroup();
    void clear();

    void set_area_lights(const std::vector<IAreaLight*>& area_lights) {
        clear();
        this->area_lights = area_lights;
    }

    // MIS light from object surface
    Vector3 sample_light(Vector3 wo, const IHittable& world, const HitRecord& hit_record);

private:
    // sample ray uniformly at each area light source
    std::pair<Vector3, ScalarType> uniform_sample_ray(Vector3 wo, const IHittable& world, const HitRecord& prev_record, HitRecord& light_record) const;
    ScalarType pdf(const Ray& ray, const IHittable& world, HitRecord& hit_record) const;

    std::vector<IAreaLight*> area_lights;
};

}
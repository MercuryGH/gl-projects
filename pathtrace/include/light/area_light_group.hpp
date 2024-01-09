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

    void set_area_lights(const std::vector<IAreaLight*>& area_lights) {
        this->area_lights = area_lights;
    }

    // sample ray uniformly at each area light source
    std::pair<Vector3, ScalarType> uniform_sample_ray(Vector3 wo, const IHittable& world, const HitRecord& prev_record, HitRecord& light_record) const;

    ScalarType pdf(const Ray& ray, const IHittable& world, HitRecord& hit_record) const;

private:
    std::vector<IAreaLight*> area_lights;
};

}
#pragma once

#include <vector>
#include <unordered_map>

#include <light/ray.hpp>
#include <light/hit_record.hpp>
#include <geometry/hittable.hpp>

namespace pathtrace {

class AreaLightGroup {
public:
    // hit_record return by ref
    ScalarType pdf_hit(const Ray& ray, const IHittable& world, HitRecord& hit_record) const;

private:
    // AreaLight* get_light_object_by_index(ObjectIndexType index);

    // std::unordered_map<ObjectIndexType, AreaLight*> emissive_objects;
};

}
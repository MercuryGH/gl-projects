#pragma once

#include <util/types.hpp>

#include <geometry/primitive.hpp>
#include <geometry/hittable.hpp>

namespace pathtrace {

// interface
// Uniform area light
class IAreaLight: public IHittable, public IPrimitive {
public:
    virtual Vector3 uniform_sample_point() const = 0;

    // default method
    std::pair<Vector3, ScalarType> uniform_sample_ray(const IHittable& world, const HitRecord& prev_record, HitRecord& light_record) const;

    virtual ~IAreaLight() {}
};

}

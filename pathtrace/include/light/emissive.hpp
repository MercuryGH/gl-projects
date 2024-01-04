#pragma once

#include <util/types.hpp>

#include <geometry/primitive.hpp>
#include <light/hit_record.hpp>

namespace pathtrace {

// emissive geometry primitive
class Emissive {
public:
    ScalarType sample_ray(const HitRecord& hit_record) const;

private:
    IPrimitive* primitive;
};

}
#pragma once

#include <util/types.hpp>

namespace pathtrace {

enum MaterialType {
    ePhong,
    eGlass,
    eEmissive
};

struct HitRecord;

// interface
class IMaterial {
public:
    virtual ScalarType pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const = 0;
    virtual ScalarType bsdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const = 0;
    virtual ScalarType scatter(Vector3& wi, Vector3 wo, const HitRecord& hit_record) const = 0;
    virtual MaterialType get_type() const = 0;
};

}
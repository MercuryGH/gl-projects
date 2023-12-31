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
    /**
     * light sample pdf based on material property
    */
    virtual ScalarType pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const = 0;

    virtual Vector3 bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const = 0;

    /**
     * sample the direction of wi by given wo
     * returns { wi, pdf(wo, wi) }
    */
    virtual std::pair<Vector3, ScalarType> scatter(Vector3 wo, const HitRecord& hit_record) const = 0;

    /**
     * runtime cast
    */
    virtual MaterialType get_type() const = 0;

    /**
     * rgb emitted. (0, 0, 0) if non-emssive
    */
    virtual Vector3 light_emitted() const = 0;
};

}
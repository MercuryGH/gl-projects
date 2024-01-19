#pragma once

#include <util/types.hpp>

namespace pathtrace {

using MaterialIDType = int;

enum MaterialType {
    ePhong,
    eGlass
};

struct HitRecord;

// abstract class
class AMaterial {
public:
    AMaterial() = default;
    AMaterial(MaterialIDType material_id): material_id(material_id) {}

    virtual ~AMaterial() {}

    /**
     * light sample pdf based on material property
    */
    virtual ScalarType pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const = 0;

    virtual Vector3 bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const = 0;

    /**
     * sample the direction of wi by given wo, rays are in the same surface position
     * assume path tracing starts at camera and ends at light
     * returns { wi, pdf(wo, wi) }
    */
    virtual std::pair<Vector3, ScalarType> sample_wi(Vector3 wo, const HitRecord& hit_record) const = 0;

    /**
     * runtime cast
    */
    virtual MaterialType get_type() const = 0;

    /**
     * rgb emitted. (0, 0, 0) if non-emssive
    */
    virtual Vector3 light_emitted() const = 0;

    MaterialIDType id() const { return material_id; }

private:
    MaterialIDType material_id;
};

}

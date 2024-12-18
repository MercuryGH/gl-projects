#pragma once

#include <material/material.hpp>

namespace pathtrace {

class GlassMaterial: public AMaterial {
public:
    GlassMaterial() = default;
    GlassMaterial(MaterialIDType material_id): AMaterial(material_id) {}
    GlassMaterial(MaterialIDType material_id, ScalarType ior): AMaterial(material_id), ior(ior) {}

    ScalarType pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const override;
    Vector3 bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const override;
    std::pair<Vector3, ScalarType> sample_wi(Vector3 wo, const HitRecord& hit_record) const override;
    MaterialType get_type() const override { return eGlass; }
    Vector3 light_emitted() const override { return { 0, 0, 0 }; }

private:
    ScalarType ior; // index of refraction
    ScalarType energy_conserve{ 0.9 }; // use in bxdf
};

}

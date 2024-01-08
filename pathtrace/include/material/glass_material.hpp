#pragma once

#include <material/material.hpp>

namespace pathtrace {

class GlassMaterial: public IMaterial {
public:
    GlassMaterial() {}
    GlassMaterial(ScalarType ior): ior(ior) {}

    ScalarType pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const override;
    Vector3 bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const override;
    std::pair<Vector3, ScalarType> scatter(Vector3 wo, const HitRecord& hit_record) const override;
    MaterialType get_type() const override { return eGlass; }
    Vector3 light_emitted() const override { return { 0, 0, 0 }; }

private:
    ScalarType ior; // index of refraction
    ScalarType energy_conserve{ 0.9 }; // use in bxdf
};

}
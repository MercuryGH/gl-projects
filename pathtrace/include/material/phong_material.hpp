#pragma once

#include <memory>

#include <material/material.hpp>

namespace pathtrace {

struct TextureRGBf;

class PhongMaterial: public IMaterial {
public:
    PhongMaterial(): texture(nullptr) {}
    PhongMaterial(Vector3 diffuse, Vector3 specular, Vector3 emissive, ScalarType phong_exponent, const std::shared_ptr<TextureRGBf>& texture): diffuse(diffuse), specular(specular), emissive(emissive), phong_exponent(phong_exponent), texture(texture) {}

    ScalarType pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const override;
    Vector3 bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const override;
    std::pair<Vector3, ScalarType> sample_wi(Vector3 wo, const HitRecord& hit_record) const override;
    MaterialType get_type() const override { return ePhong; }
    Vector3 light_emitted() const override { return emissive; }

    bool has_texture() const { return texture != nullptr; }

private:
    /**
     * simple lambertian diffuse pdf
    */
    ScalarType pdf_diffuse(Vector3 wi, Vector3 wo, Vector3 normal) const;
    std::pair<Vector3, ScalarType> sample_diffuse(Vector3 wo, const HitRecord& hit_record) const;

    /**
     * specular pdf based on Normal Distribution Function of Blinn
    */
    ScalarType pdf_specular(Vector3 wi, Vector3 wo, Vector3 normal) const;
    std::pair<Vector3, ScalarType> sample_specular(Vector3 wo, const HitRecord& hit_record) const;

    Vector3 diffuse; // diffuse reflectivity
    Vector3 specular; // specular reflectivity
    Vector3 emissive; // radiance emitted. (0, 0, 0) for non-emissive material
    ScalarType phong_exponent;

    std::shared_ptr<TextureRGBf> texture;
    // alternative: raw pointer indicates non-ownership:
    // const TextureRGBf* texture_ptr;
};

}

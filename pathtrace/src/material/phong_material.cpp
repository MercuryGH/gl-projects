#include <material/phong_material.hpp>

#include <cassert>

#include <glm/gtx/component_wise.hpp>

#include <util/math.hpp>
#include <util/random.hpp>
#include <material/texture_rgb_f.hpp>
#include <light/hit_record.hpp>

namespace pathtrace {

ScalarType PhongMaterial::pdf_diffuse(Vector3 wi, Vector3 wo, Vector3 normal) const {
    ScalarType normal_cosine = glm::dot(normal, wi);
    if (normal_cosine <= 0) {
        return 0;
    }

    return normal_cosine * k_inv_pi;
}

std::pair<Vector3, ScalarType> PhongMaterial::sample_diffuse(Vector3 wo, const HitRecord& hit_record) const {
    Vector3 p = util::get_uniform_unit_sphere_distribution();
    Vector3 wi = util::local_to_world(p, hit_record.normal);
    ScalarType pdf = pdf_diffuse(wi, wo, hit_record.normal);
    return { wi, pdf };
}

ScalarType PhongMaterial::pdf_specular(Vector3 wi, Vector3 wo, Vector3 normal) const {
    ScalarType normal_cosine = glm::dot(normal, wi);
    if (normal_cosine <= 0) {
        return 0;
    }

    Vector3 w_reflect = util::reflect(wo, normal);
    if (glm::dot(wi, w_reflect) <= 0) {
        return 0;
    }

    // phong ndf (not blinn-phong). Using reflctance instead of half-way vector
    // ref: https://agraphicsguynotes.com/posts/sample_microfacet_brdf/
    return (phong_exponent + 1) * k_inv_2pi * std::pow(glm::dot(wi, w_reflect), phong_exponent);
}

std::pair<Vector3, ScalarType> PhongMaterial::sample_specular(Vector3 wo, const HitRecord& hit_record) const {
    // sample with (cos_\theta)^{\alpha+1}
    // biased unit sphere
    ScalarType phi = util::get_uniform_real_distribution(0.0f, k_2pi);
    ScalarType cos_theta = std::pow(util::get_uniform_real_distribution(0.0f, 1.0f), 1.0f / (phong_exponent + 1));
    ScalarType cos_phi = std::cos(phi);
    Vector3 p = util::unit_sphere_to_cartesian(cos_theta, cos_phi);

    Vector3 w_reflect = util::reflect(wo, hit_record.normal);
    Vector3 wi = util::local_to_world(p, w_reflect);
    ScalarType pdf = pdf_specular(wi, wo, hit_record.normal);
    return { wi, pdf };
}

ScalarType PhongMaterial::pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const {
    Vector3 normal = hit_record.normal;
    Vector3 kd = has_texture() ? texture->at(hit_record.uv) : diffuse;
    Vector3 ks = specular;

    ScalarType normal_cosine = glm::dot(normal, wi);
    if (normal_cosine <= 0) {
        return 0;
    }

    ScalarType sum_prob = glm::compMax(ks) + glm::compMax(kd);
    if (sum_prob <= 0) {
        return 0;
    }

    ScalarType diffuse_prob = glm::compMax(kd) / sum_prob;
    ScalarType specular_prob = 1.0f - diffuse_prob;

    // MIS
    return diffuse_prob * pdf_diffuse(wi, wo, normal) + specular_prob * pdf_specular(wi, wo, normal);
}

Vector3 PhongMaterial::bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const {
    Vector3 normal = hit_record.normal;
    Vector3 kd = has_texture() ? texture->at(hit_record.uv) : diffuse;
    Vector3 ks = specular;

    ScalarType normal_cosine = glm::dot(normal, wi);
    if (normal_cosine <= 0) {
        return { 0, 0, 0 };
    }

    Vector3 w_reflect = util::reflect(wo, normal);
    Vector3 diffuse_term = kd * k_inv_pi;
    Vector3 specular_term = ks * (phong_exponent + 2) * std::pow(glm::dot(w_reflect, wo), phong_exponent);

    return diffuse_term + specular_term;
}

std::pair<Vector3, ScalarType> PhongMaterial::sample_wi(Vector3 wo, const HitRecord& hit_record) const {
    Vector3 normal = hit_record.normal;
    Vector3 kd = has_texture() ? texture->at(hit_record.uv) : diffuse;
    Vector3 ks = specular;

    ScalarType sum_prob = glm::compMax(ks) + glm::compMax(kd);
    if (sum_prob <= 0) {
        // bad
        assert(false);
        return { {0, 0, 1}, 0 };
    }

    ScalarType diffuse_prob = glm::compMax(kd) / sum_prob;
    ScalarType specular_prob = 1.0f - diffuse_prob;

    // MIS
    ScalarType random_u = util::get_uniform_real_distribution(0.0f, 1.0f);
    Vector3 wi;
    ScalarType pdf_d, pdf_s;
    if (diffuse_prob > random_u) {
        std::tie(wi, pdf_d) = sample_diffuse(wo, hit_record);
        pdf_s = pdf_specular(wi, wo, normal);
    } else {
        std::tie(wi, pdf_s) = sample_specular(wo, hit_record);
        pdf_d = pdf_diffuse(wi, wo, normal);
    }

    ScalarType normal_cosine = glm::dot(normal, wi);
    if (normal_cosine <= 0) {
        return { wi, 0 };
    }

    return { wi, diffuse_prob * pdf_d + specular_prob * pdf_s };
}

}
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
        return 0.0f;
    }

    return normal_cosine * k_inv_pi;
}

std::pair<Vector3, ScalarType> PhongMaterial::sample_diffuse(Vector3 wo, const HitRecord& hit_record) const {
    // MI sample with (cos_\theta)
    // a good formula derivation: https://agraphicsguynotes.com/posts/sample_microfacet_brdf/
    ScalarType phi = util::get_uniform_real_distribution(0.0f, k_2pi);
    ScalarType cos_theta = std::sqrt(1.0f - util::get_uniform_real_distribution(0.0f, 1.0f));
    ScalarType cos_phi = std::cos(phi);
    Vector3 p_local = util::unit_sphere_to_cartesian(cos_theta, cos_phi);
    Vector3 wi = glm::normalize(util::local_to_world(p_local, hit_record.normal));
    ScalarType wi_normal_cosine = glm::dot(hit_record.normal, wi);
    ScalarType pdf = wi_normal_cosine <= 0 ? 0.0 : cos_theta * k_inv_pi;

    return { wi, pdf };
}

ScalarType PhongMaterial::pdf_specular(Vector3 wi, Vector3 wo, Vector3 normal) const {
    ScalarType wi_normal_cosine = glm::dot(normal, wi);
    if (wi_normal_cosine <= 0) {
        return 0.0f;
    }

    Vector3 wo_reflect = util::reflect(wo, normal);
    ScalarType wi_wr_weight = glm::dot(wi, wo_reflect);
    bool wi_wr_same_dir = wi_wr_weight > 0;
    if (wi_wr_same_dir == false) {
        return 0.0f;
    }

    // phong ndf (not blinn-phong). Using reflctance instead of half-way vector
    return (phong_exponent + 1) * k_inv_2pi * std::pow(wi_wr_weight, phong_exponent);
}

std::pair<Vector3, ScalarType> PhongMaterial::sample_specular(Vector3 wo, const HitRecord& hit_record) const {
    // MI sample with (cos_\theta)^{\alpha+1}
    // biased unit sphere
    wo = glm::normalize(wo);
    ScalarType phi = util::get_uniform_real_distribution(0.0f, k_2pi);
    ScalarType cos_theta = std::pow(util::get_uniform_real_distribution(0.0f, 1.0f), 1.0f / (phong_exponent + 1));

    ScalarType cos_phi = std::cos(phi);
    Vector3 p = util::unit_sphere_to_cartesian(cos_theta, cos_phi);

    Vector3 wo_reflect = util::reflect(wo, hit_record.normal);
    Vector3 wi = glm::normalize(util::local_to_world(p, wo_reflect));
    ScalarType wi_normal_cosine = glm::dot(wi, hit_record.normal);
    ScalarType pdf = wi_normal_cosine <= 0 ? 0.0f : k_inv_2pi * (phong_exponent + 1) * std::pow(cos_theta, phong_exponent);
    return { wi, pdf };
}

ScalarType PhongMaterial::pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const {
    Vector3 normal = hit_record.normal;
    Vector3 kd = has_texture() ? texture->at(hit_record.uv) : diffuse;
    Vector3 ks = specular;
    wi = glm::normalize(wi);
    wo = glm::normalize(wo);

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

    ScalarType pdf = diffuse_prob * pdf_diffuse(wi, wo, normal) + specular_prob * pdf_specular(wi, wo, normal);

    // MIS
    return pdf;
}

Vector3 PhongMaterial::bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const {
    Vector3 normal = hit_record.normal;
    Vector3 kd = has_texture() ? texture->at(hit_record.uv) : diffuse;
    Vector3 ks = specular;
    wi = glm::normalize(wi);
    wo = glm::normalize(wo);

    ScalarType normal_cosine = glm::dot(normal, wi);
    if (normal_cosine <= 0) {
        return { 0, 0, 0 };
    }

    Vector3 diffuse_term = kd * k_inv_pi;

    Vector3 wi_reflect = util::reflect(wi, normal);
    ScalarType wo_wr_weight = glm::dot(wi_reflect, wo);
    bool wo_wr_same_dir = wo_wr_weight > 0;
    Vector3 specular_term;
    if (wo_wr_same_dir) {
        specular_term = ks * k_inv_2pi * (phong_exponent + 2) * std::pow(wo_wr_weight, phong_exponent);
    } else {
        specular_term = { 0, 0, 0 };
    }

    return diffuse_term + specular_term;
}

std::pair<Vector3, ScalarType> PhongMaterial::sample_wi(Vector3 wo, const HitRecord& hit_record) const {
    Vector3 normal = hit_record.normal;
    Vector3 kd = has_texture() ? texture->at(hit_record.uv) : diffuse;
    Vector3 ks = specular;
    wo = glm::normalize(wo);

    ScalarType sum_prob = glm::compMax(kd) + glm::compMax(ks);
    if (sum_prob <= 0) {
        return { {0, 0, 0}, 0.0f };
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

    ScalarType wi_normal_cosine = glm::dot(wi, normal);
    if (wi_normal_cosine <= 0) {
        return { wi, 0.0f };
    }

    ScalarType pdf = diffuse_prob * pdf_d + specular_prob * pdf_s;

    return { wi, pdf };
}

}

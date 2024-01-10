#include <material/glass_material.hpp>

#include <util/math.hpp>
#include <util/random.hpp>

#include <light/hit_record.hpp>

namespace pathtrace {

ScalarType GlassMaterial::pdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const {
    return 1.0f;
}

Vector3 GlassMaterial::bxdf(Vector3 wi, Vector3 wo, const HitRecord& hit_record) const {
    return energy_conserve * Vector3{ 1, 1, 1 };
}

std::pair<Vector3, ScalarType> GlassMaterial::sample_wi(Vector3 wo, const HitRecord& hit_record) const {
    Vector3 normal = hit_record.normal;

    ScalarType normal_cosine = glm::dot(normal, wo);
    ScalarType rel_ior = normal_cosine > 0 ? 1.0f / ior : ior;
    normal = normal_cosine > 0 ? normal : -normal;

    ScalarType cos_theta = glm::dot(wo, normal);
    ScalarType sin_theta = std::sqrt(1.0f - util::sq(cos_theta));

    bool cannot_refract = rel_ior * sin_theta > 1.0;
    ScalarType random_u = util::get_uniform_real_distribution(0.0f, 1.0f);
    Vector3 wi = (cannot_refract || util::fresnel_schlick(cos_theta, rel_ior) > random_u) ? util::reflect(wo, normal) : util::refract(wo, normal, rel_ior);

    return { glm::normalize(wi), 1.0f };
}

}
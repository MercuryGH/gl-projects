#include <util/math.hpp>

#include <util/random.hpp>

namespace util {

Vector3 reflect(Vector3 wo, Vector3 normal) {
    return glm::normalize(2.0f * glm::dot(normal, wo) * normal - wo);
}

/**
 * refract with Snell's law
 * ref: https://raytracing.github.io/books/RayTracingInOneWeekend.html#dielectrics
*/
Vector3 refract(Vector3 wo, Vector3 normal, ScalarType ior) {
    ScalarType cos_theta = glm::dot(wo, normal);
    Vector3 r_out_perp = ior * (-wo + cos_theta * normal);
    Vector3 r_out_parallel = -std::sqrt(1.0f - glm::dot(r_out_perp, r_out_perp)) * normal;
    return r_out_parallel + r_out_perp;
}

/**
 * fresnel effect approximation by schlick
*/
ScalarType fresnel_schlick(ScalarType cos, ScalarType ref_idx) {
    ScalarType r0 = util::sq((1 - ref_idx) / (1 + ref_idx));
    return r0 + (1 - r0) * std::pow((1 - cos), 5);
}

Vector3 local_to_world(Vector3 local, Vector3 unit_z) {
    // construct orthogonal basis
    unit_z = glm::normalize(unit_z);
    Vector3 unit_y = (std::abs(unit_z.x) > 0.9f) ? Vector3{ 0, 1, 0 } : Vector3{ 1, 0, 0 };
    unit_y = glm::normalize(glm::cross(unit_y, unit_z));

    Vector3 unit_x = glm::normalize(glm::cross(unit_y, unit_z));
    assert(glm::cross(unit_y, unit_z).length() > k_eps);

    Matrix3 mat = { unit_x, unit_y, unit_z };
    return mat * local;
}

Vector3 unit_sphere_to_cartesian(ScalarType cos_theta, ScalarType cos_phi) {
    ScalarType sin_theta = std::sqrt(1.0f - util::sq(cos_theta));
    ScalarType sin_phi = std::sqrt(1.0f - util::sq(cos_phi));

    return { sin_theta * cos_phi, sin_theta * sin_phi, cos_theta };
}

Vector3 get_uniform_unit_sphere_distribution() {
    ScalarType phi = util::get_uniform_real_distribution(0.0f, k_2pi);
    ScalarType cos_theta = std::sqrt(1.0f - util::get_uniform_real_distribution(0.0f, 1.0f));
    ScalarType cos_phi = std::cos(phi);
    return unit_sphere_to_cartesian(cos_theta, cos_phi);
}

}

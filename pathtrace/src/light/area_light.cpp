#include <light/area_light.hpp>

#include <cstdio>

#include <util/math.hpp>
#include <light/ray.hpp>

namespace pathtrace {

/**
 * sample area light pdf
 * ref: https://computergraphics.stackexchange.com/questions/12624/importance-sampling-rectangular-light
*/
std::pair<Vector3, ScalarType> IAreaLight::uniform_sample_ray(const IHittable& world, const HitRecord& prev_record, HitRecord& light_record) const {
    Vector3 random_p = uniform_sample_point();
    Ray light_ray(prev_record.pos, random_p - prev_record.pos); // cast ray from object to light

    Vector3 wi = light_ray.dir; // already normalized

    bool hits = world.hit(light_ray, Vector2{ k_eps, k_max }, light_record);
    bool hit_light = glm::dot(random_p - light_record.pos, random_p - light_record.pos) < k_eps; // the ray hits the light with no obstacle
    bool object_face_judge = glm::dot(prev_record.normal, wi) > 0;
    ScalarType light_cosine = -glm::dot(light_record.normal, wi);
    bool light_face_judge = light_cosine > 0;
    if (hits && hit_light && object_face_judge && light_face_judge) {
        ScalarType sq_distance = glm::dot(light_record.pos - prev_record.pos, light_record.pos - prev_record.pos);
        ScalarType area = get_area();
        ScalarType pdf = sq_distance / (area * light_cosine);

        return { wi, pdf };
    }
    return { wi, 0.0f };
}

}

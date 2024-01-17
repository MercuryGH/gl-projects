#include <geometry/triangle.hpp>

#include <geometry/bounding_box.hpp>

#include <util/random.hpp>
#include <util/math.hpp>

namespace pathtrace {

ScalarType Triangle::get_area() const {
    return 0.5f * std::abs(glm::length(glm::cross(p(1) - p(0), p(2) - p(0))));
}

Vector3 Triangle::get_centroid() const {
    return (p(0) + p(1) + p(2)) / 3.0f;
}

/**
 * ray-triangle intersection
 * M-T algorithm. ref: GAMES 101 course
*/
bool Triangle::hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const {
    Vector3 edge1 = p(1) - p(0);
    Vector3 edge2 = p(2) - p(0);
    Vector3 ray_cross_e2 = glm::cross(ray.dir, edge2);
    ScalarType det = glm::dot(edge1, ray_cross_e2);

    if (std::abs(det) < k_small_eps)
        return false; // This ray is parallel to this triangle.

    ScalarType inv_det = 1.0f / det;
    Vector3 s = ray.origin - p(0);
    ScalarType u = inv_det * glm::dot(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return false;

    Vector3 s_cross_e1 = glm::cross(s, edge1);
    ScalarType v = inv_det * glm::dot(ray.dir, s_cross_e1);

    if (v < 0 || u + v > 1)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    ScalarType t = inv_det * glm::dot(edge2, s_cross_e1);

    if (util::inside(t, t_range[0], t_range[1])) // ray intersection
    {
        ScalarType w = 1.0f - u - v;
        hit_record = HitRecord {
            .pos = ray.origin + ray.dir * t,
            .t = t,
            .normal = get_normal(),
            .uv = uv[0] * w + uv[1] * u + uv[2] * v,
            .material = material,
            .hit_object = this,
            .hit_geometry = this
        };
        return true;
    }
    // This means that there is a line intersection but not a ray intersection.
    return false;
}

BoundingBox Triangle::get_bounding_box() const {
    return BoundingBox(*this);
}

}

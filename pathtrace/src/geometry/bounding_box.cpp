#include <geometry/bounding_box.hpp>

#include <geometry/triangle.hpp>
#include <util/math.hpp>

namespace pathtrace {

BoundingBox::BoundingBox(const Triangle& tri) {
    make_empty();
    merge(tri);
}

void BoundingBox::make_empty() {
    pmin = Vector3(k_max, k_max, k_max);
    pmax = Vector3(k_min, k_min, k_min);
}

Vector3 BoundingBox::centroid() const {
    return (pmin + pmax) * 0.5f;
}

ScalarType BoundingBox::extent() const {
    return glm::length(pmax - pmin);
}

void BoundingBox::round_to_int() {
    for (int i = 0; i < 3; i++) {
        pmin[i] = std::floor(pmin[i]);
        pmax[i] = std::ceil(pmax[i]);
    }
}

void BoundingBox::transform(const Matrix4 &mat) {
    ScalarType x[] = { pmin.x, pmax.x };
    ScalarType y[] = { pmin.y, pmax.y };
    ScalarType z[] = { pmin.z, pmax.z };

    for (int i = 0; i < 8; i++) {
        Vector3 result = mat * Vector4(x[i % 2], y[(i / 2) % 2], z[(i / 3) % 2], 1.0f);
        if (i == 0) {
            pmin = result;
            pmax = result;
        } else {
            pmin = glm::min(pmin, result);
            pmax = glm::max(pmax, result);
        }
    }
}

void BoundingBox::merge(const Triangle &tri) {
    for (int i = 0; i < 3; i++) {
        merge(tri.p(i));
    }
}

void BoundingBox::merge(const BoundingBox &rhs) {
    pmin = glm::min(pmin, rhs.pmin);
    pmax = glm::max(pmax, rhs.pmax);
}

void BoundingBox::merge(Vector3 p) {
    pmin = glm::min(pmin, p);
    pmax = glm::max(pmax, p);
}

void BoundingBox::intersect(const BoundingBox &rhs) {
    pmin = glm::max(pmin, rhs.pmin);
    pmax = glm::min(pmax, rhs.pmax);
}

bool BoundingBox::intersect_with(const BoundingBox &rhs) const {
    return pmin.x <= rhs.pmax.x && pmax.x >= rhs.pmin.x
        && pmin.y <= rhs.pmax.y && pmax.y >= rhs.pmin.y
        && pmin.z <= rhs.pmax.z && pmax.z >= rhs.pmin.z;
}

bool BoundingBox::contains(const BoundingBox &rhs) const {
    return pmin.x <= rhs.pmin.x && pmax.x >= rhs.pmax.x
        && pmin.y <= rhs.pmin.y && pmax.y >= rhs.pmax.y
        && pmin.z <= rhs.pmin.z && pmax.z >= rhs.pmax.z;
}

bool BoundingBox::hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const {
    // ScalarType t_min = t_range[0];
    // ScalarType t_max = t_range[1];
    // for (int a = 0; a < 3; a++) {
    //     auto invD = 1.0f / ray.dir[a];
    //     auto t0 = (min_p[a] - ray.origin[a]) * invD;
    //     auto t1 = (max_p[a] - ray.origin[a]) * invD;
    //     if (invD < 0.0f)
    //         std::swap(t0, t1);
    //     t_min = t0 > t_min ? t0 : t_min;
    //     t_max = t1 < t_max ? t1 : t_max;
    //     if (t_max < t_min)
    //         return false;
    // }
    // t_hit = t_min;
    return true;
}

void BoundingBox::get_bounding_box(BoundingBox &ret_bb) const {
    ret_bb = BoundingBox(*this);
}

}

#include <scene/bounding_box.hpp>

namespace rasterize {

template class BoundingBox<3>;
template class BoundingBox<2>;

template<int dimension>
void BoundingBox<dimension>::make_empty() {
    constexpr auto k_max = std::numeric_limits<ScalarType>::max();
    constexpr auto k_min = std::numeric_limits<ScalarType>::min();

    pmin = Vector3(k_max, k_max, k_max);
    pmax = Vector3(k_min, k_min, k_min);
}

template<int dimension>
Vector3 BoundingBox<dimension>::centroid() const {
    return (pmin + pmax) * 0.5f;
}

template<int dimension>
ScalarType BoundingBox<dimension>::extent() const {
    return glm::length(pmax - pmin);
}

template<int dimension>
void BoundingBox<dimension>::round_to_int() {
    for (int i = 0; i < 3; i++) {
        pmin[i] = std::floor(pmin[i]);
        pmax[i] = std::ceil(pmax[i]);
    }
}

template<int dimension>
void BoundingBox<dimension>::transform(const glm::mat4 &mat) {
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

template<int dimension>
void BoundingBox<dimension>::merge(const Triangle &tri) {
    for (int i = 0; i < 3; i++) {
        merge(tri.p[i]);
    }
}

template<int dimension>
void BoundingBox<dimension>::merge(const BoundingBox<dimension> &rhs) {
    pmin = glm::min(pmin, rhs.pmin);
    pmax = glm::max(pmax, rhs.pmax);
}

template<int dimension>
void BoundingBox<dimension>::merge(Vector3 p) {
    pmin = glm::min(pmin, p);
    pmax = glm::max(pmax, p);
}

template<int dimension>
void BoundingBox<dimension>::intersect(const BoundingBox<dimension> &rhs) {
    pmin = glm::max(pmin, rhs.pmin);
    pmax = glm::min(pmax, rhs.pmax);
}

template<int dimension>
bool BoundingBox<dimension>::intersect_with(const BoundingBox<dimension> &rhs) const {
    return pmin.x <= rhs.pmax.x && pmax.x >= rhs.pmin.x
        && pmin.y <= rhs.pmax.y && pmax.y >= rhs.pmin.y
        && pmin.z <= rhs.pmax.z && pmax.z >= rhs.pmin.z;
}

template<>
void BoundingBox<2>::foreach_pixel(const std::function<void(int, int)>& func) const {
    int pmin_x = static_cast<int>(pmin.x);
    int pmax_x = static_cast<int>(pmax.x);
    int pmin_y = static_cast<int>(pmin.y);
    int pmax_y = static_cast<int>(pmax.y);

    printf("(%d, %d) to (%d, %d)\n", pmin_x, pmin_y, pmax_x, pmax_y);

    for (int x = pmin_x; x <= pmax_x; x++) {
        for (int y = pmin_y; y <= pmax_y; y++) {
            func(x, y);
        }
    }
}

template<>
void BoundingBox<3>::foreach_pixel(const std::function<void(int, int)>& func) const {
    assert(false);
}


}

#pragma once

#include <functional>

#include <util/types.hpp>

#include <scene/triangle.hpp>

namespace rasterize {

template<int dimension>
class BoundingBox {
public:
    BoundingBox() { make_empty(); }
    BoundingBox(Vector3 pmin, Vector3 pmax): pmin(pmin), pmax(pmax) {}
    BoundingBox(const BoundingBox& rhs): pmin(rhs.pmin), pmax(rhs.pmax) {}
    void make_empty();

    Vector3 centroid() const;
    ScalarType extent() const;

    void round_to_int();
    void transform(const glm::mat4 &mat);
    void merge(const BoundingBox<dimension> &rhs);
    void merge(const Triangle &tri);
    void merge(Vector3 p);
    void intersect(const BoundingBox<dimension> &rhs);
    bool intersect_with(const BoundingBox<dimension> &rhs) const;

    // available when dimension = 2
    // should call round_to_int() before
    void foreach_pixel(const std::function<void(int, int)>& func) const;

private:
    Vector3 pmin, pmax;
};

using BoundingRect = BoundingBox<2>;
using BoundingCuboid = BoundingBox<3>;

}

#pragma once

/**
 * This file should be replaced by rasterizer's
*/

#include <glm/glm.hpp>

namespace pathtrace {

class BoundingBox {
public:
    void empty();

    Vector3 centroid() const;
    float extent() const;

    BoundingBox transform_by(const glm::mat4 &mat) const;
    void merge(const BoundingBox &rhs);
    void merge(const Vector3 &p);
    void intersect(const BoundingBox &rhs);
    bool intersect_with(const BoundingBox &rhs) const;

private:
    Vector3 pmin;
    Vector3 pmax;
};

}
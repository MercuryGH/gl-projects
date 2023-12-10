#include <scene/bounding_box.hpp>

namespace pathtrace {

void BoundingBox::empty() {

}

Vector3 BoundingBox::centroid() const {
    return Vector3(0);
}

float BoundingBox::extent() const {
    return 0;
}

BoundingBox BoundingBox::transform_by(const glm::mat4 &mat) const {
    return BoundingBox{};
}

void BoundingBox::merge(const BoundingBox &rhs) {

}

void BoundingBox::merge(const Vector3 &p) {

}

void BoundingBox::intersect(const BoundingBox &rhs) {

}

bool BoundingBox::intersect_with(const BoundingBox &rhs) const {
    return true;
}

}
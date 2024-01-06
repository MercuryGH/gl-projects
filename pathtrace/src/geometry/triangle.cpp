#include <geometry/triangle.hpp>

#include <util/random.hpp>

namespace pathtrace {

ScalarType Triangle::get_area() const {
    return 0;
}

Vector3 Triangle::get_centroid() const {
    return { 0, 0, 0 };
}

bool Triangle::hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const {
    return false;
}

void Triangle::get_bounding_box(BoundingBox &ret_bb) const {

}

Vector3 Triangle::uniform_sample_point() const {
    // ref: https://math.stackexchange.com/questions/538458/how-to-sample-points-on-a-triangle-surface-in-3d

    ScalarType sqrt_a = std::sqrt(util::get_uniform_real_distribution(0.0, 1.0));
    ScalarType b = util::get_uniform_real_distribution(0.0, 1.0);
    return p(0) * (1.0f - sqrt_a) + p(1) * (sqrt_a * (1 - b)) + p(2) * (sqrt_a * b);
}


}

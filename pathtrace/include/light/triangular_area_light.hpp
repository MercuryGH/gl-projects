#pragma once

#include <geometry/triangle.hpp>
#include <geometry/bounding_box.hpp>
#include <light/area_light.hpp>

namespace pathtrace {

class TriangularAreaLight: public IAreaLight {
public:
    TriangularAreaLight(const Triangle& tri): tri(tri) {}

    ScalarType get_area() const override {
        return tri.get_area();
    }
    Vector3 get_centroid() const override {
        return tri.get_centroid();
    }

    bool hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const override {
        return tri.hit(ray, t_range, hit_record);
    }
    BoundingBox get_bounding_box() const override {
        return tri.get_bounding_box();
    }

    Vector3 uniform_sample_point() const override;

private:
    Triangle tri;
};

}
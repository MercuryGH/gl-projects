#pragma once

#include <geometry/triangle.hpp>
#include <light/area_light.hpp>

namespace pathtrace {

class TriangularAreaLight: public IAreaLight {
public:
    ScalarType get_area() const override {
        return tri.get_area();
    }
    Vector3 get_centroid() const override {
        return tri.get_centroid();
    }

    bool hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const override {
        return tri.hit(ray, t_range, hit_record);
    }
    void get_bounding_box(BoundingBox &ret_bb) const override {
        tri.get_bounding_box(ret_bb);
    }

    Vector3 uniform_sample_point() const override;

private:
    Triangle tri;
};

}
#pragma once

#include <light/ray.hpp>
#include <geometry/hittable.hpp>
#include <geometry/bounding_box.hpp>

namespace pathtrace {

class BvhNode: public IHittable {
public:
    bool hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const override;
    void get_bounding_box(BoundingBox& ret_bb) const override;

private:
    IHittable* children;
    BoundingBox bbox;
};

}
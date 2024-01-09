#pragma once

#include <vector>

#include <light/ray.hpp>
#include <geometry/hittable.hpp>
#include <geometry/bounding_box.hpp>

namespace pathtrace {

/**
 * this class only represents non-leaf nodes in BVH
 * leaf nodes are hittable geometry like traingle, etc.
*/
class BvhNode: public IHittable {
public:
    BvhNode() = default;

    bool hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const override;
    void get_bounding_box(BoundingBox& ret_bb) const override;

    void set_child(int index, IHittable* node) { children[index] = node; }
    IHittable* get_child(int index) const { return children[index]; }

    constexpr static int k_n_children = 2; // binary tree

private:
    IHittable* children[k_n_children]{ nullptr };
    BoundingBox bbox;
};

}
#include <scene/bvh_node.hpp>

namespace pathtrace {

bool BvhNode::hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const {
    // TODO: debug only
    if (bbox.valid() == false) {
        assert(false);
    }

    ScalarType t_hit[k_n_children];
    bool hit[k_n_children];
    for (int i = 0; i < k_n_children; i++) {
        BoundingBox child_bb;
        get_child(i)->get_bounding_box(child_bb);

        // test AABB
        HitRecord tmp_record;
        hit[i] = child_bb.hit(ray, t_range, tmp_record);
        t_hit[i] = tmp_record.t;
    }

    // both hit
    if (hit[0] && hit[1]) {
        int near_child_idx = t_hit[0] < t_hit[1] ? 0 : 1;
        int far_child_idx = 1 - near_child_idx;

        // recursive test, nearest one first
        if (get_child(near_child_idx)->hit(ray, t_range, hit_record)) {
            if (hit_record.t < t_hit[far_child_idx]) {
                return true;
            }

            // need to test far one
            HitRecord far_record;
            if (get_child(far_child_idx)->hit(ray, t_range, far_record) && far_record.t < hit_record.t) {
                hit_record = far_record;
            }
            return true;
        } else {
            return get_child(far_child_idx)->hit(ray, t_range, hit_record);
        }
    }

    // only one hit
    for (int i = 0; i < k_n_children; i++) {
        if (hit[i]) {
            return get_child(i)->hit(ray, t_range, hit_record);
        }
    }

    return false;
}

void BvhNode::get_bounding_box(BoundingBox &ret_bb) const {
    ret_bb = bbox;
}

}
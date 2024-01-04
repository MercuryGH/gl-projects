#include <scene/bvh_node.hpp>

namespace pathtrace {

bool BvhNode::hit(const Ray& ray, Vector2 t_range, HitRecord& hit_record) const {
    return false;
}

void BvhNode::get_bounding_box(BoundingBox &ret_bb) const {
    
}

}
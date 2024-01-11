#include <scene/bvh.hpp>

#include <algorithm>

#include <scene/bvh_node.hpp>

namespace pathtrace {

namespace {

    // sort by bb's centroid[dim_idx]
    template<int dim_idx>
    bool cmp(IHittable* a, IHittable* b) {
        BoundingBox a_bb = a->get_bounding_box();
        BoundingBox b_bb = b->get_bounding_box();

        return a_bb.centroid()[dim_idx] < b_bb.centroid()[dim_idx];
    }

    /**
     * construct bianry bvh
    */
    IHittable* build_bvh_entry(std::vector<IHittable*>& objects, int start_index, int end_index) { 
        if (start_index >= end_index) {
            assert(false); // bad, bug
            return nullptr;
        }

        if (end_index - start_index == 1) {
            // leaf node
            return objects.at(start_index);
        }

        // non-leaf node
        BvhNode* node = new BvhNode();
        BoundingBox centroid_bb;
        for (int i = start_index; i < end_index; i++) {
            BoundingBox obj_bb = objects.at(i)->get_bounding_box();

            // set current bvh bb
            BoundingBox node_bb = node->get_bounding_box();

            // TODO: set node bb
            node_bb.merge(obj_bb);
            centroid_bb.merge(obj_bb.centroid());
        }
        int max_extent_dim = centroid_bb.max_extent_dim();

        const auto cmp_dim = max_extent_dim == 0 ? cmp<0>
            : max_extent_dim == 1 ? cmp<1>
            : cmp<2>;

        std::sort(objects.begin() + start_index, objects.begin() + end_index, cmp_dim);

        int mid_index = (start_index + end_index) / 2;

        node->set_child(0, build_bvh_entry(objects, start_index, mid_index));
        node->set_child(1, build_bvh_entry(objects, mid_index, end_index));

        return node;
    }

}

IHittable* build_bvh(std::vector<IHittable*>& objects) {
    return build_bvh_entry(objects, 0, objects.size());
}

void clear_bvh(IHittable* root) {
    BvhNode* non_leaf_node = dynamic_cast<BvhNode*>(root);
    if (non_leaf_node == nullptr) {
        return;
    }

    clear_bvh(non_leaf_node->get_child(0));
    clear_bvh(non_leaf_node->get_child(1));
    delete non_leaf_node;
}

}
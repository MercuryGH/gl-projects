#include <graph/loop.hpp>

#include <cassert>

#include <graph/half_edge.hpp>
#include <graph/util.inl>

namespace brep {

int Loop::n_hedges()
{
    if (hedges_dirty == false) {
        return n_h;
    }

    n_h = count_nodes(hedge_head);
    hedges_dirty = false;

    return n_h;
}

void Loop::add_hedge(HalfEdge *last_hedge, HalfEdge *added_hedge) {
    auto hedge1 = last_hedge;
    auto hedge2 = last_hedge->next;

    hedges_dirty = true;

    hedge1->next = added_hedge;
    added_hedge->prev = hedge1;
    added_hedge->next = hedge2;
    hedge2->prev = added_hedge;
}

bool Loop::find_hedge(HalfEdge *hedge) {
    if (hedge == nullptr) {
        assert(false);
    }
    if (hedge_head == nullptr) {
        assert(false);
    }

    auto cur_hedge = hedge_head;
    while (cur_hedge != hedge) {
        cur_hedge = cur_hedge->next;
        if (cur_hedge == hedge_head) {
            return false;
        }
    }

    return true;
}

}

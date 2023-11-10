#include <graph/face.hpp>

#include <cassert>

#include <graph/loop.hpp>
#include <graph/util.inl>

namespace brep {

int Face::n_loops()
{
    if (loops_dirty == false) {
        return n_l;
    }

    n_l = count_nodes(loop_head);
    loops_dirty = false;

    return n_l;
}

void Face::add_loop(Loop *loop) {
    if (loop == nullptr) {
        // ERR
        assert(false);
        return;
    }

    loop->face = this;
    loops_dirty = true;

    if (loop_head == nullptr) {
        loop_head = loop;
        loop_head->next = loop;
        loop_head->prev = loop;
    } else {
        // insert at the front (between loop1, loop2)
        auto loop1 = loop_head;
        auto loop2 = loop_head->next;
        loop1->next = loop;
        loop->prev = loop1;
        loop2->prev = loop;
        loop->next = loop2;
    }
}

Loop *Face::get_first_outer_loop() {
    auto cur_loop = loop_head;
    do {
        if (cur_loop->is_inner_loop() == false) {
            return cur_loop;
        }
        cur_loop = cur_loop->next;
    } while (cur_loop != loop_head);

    return nullptr;
}

}

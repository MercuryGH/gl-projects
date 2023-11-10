#pragma once

#include <graph/self_linked.hpp>

namespace brep {

struct Solid;
struct Loop;

struct Face: SelfLinked<Face> {
    Solid* solid{ nullptr };
    Loop* loop_head{ nullptr };
    int id; // id

    bool loops_dirty{ true };

    int n_loops();

    void add_loop(Loop* loop);
    Loop* get_first_outer_loop();

    int n_l{ 0 };
};

}
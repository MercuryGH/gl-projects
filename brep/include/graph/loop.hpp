#pragma once

#include <graph/self_linked.hpp>

namespace brep {

struct Face;
struct Loop;
struct HalfEdge;
struct Vertex;

struct Loop: SelfLinked<Loop> {
    Face* face{ nullptr };
    HalfEdge* hedge_head{ nullptr };
    bool inner_loop{ false };

    bool hedges_dirty{ true };

    int n_hedges();

    bool is_inner_loop() const { return inner_loop; }
    void add_hedge(HalfEdge* last_hedge, HalfEdge* added_hedge);
    bool find_hedge(HalfEdge* hedge);

    int n_h{ 0 };
};

}
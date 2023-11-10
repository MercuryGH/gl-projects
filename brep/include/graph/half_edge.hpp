#pragma once

#include <graph/self_linked.hpp>

namespace brep {

struct Loop;
struct Edge;
struct Vertex;

struct HalfEdge: SelfLinked<HalfEdge> {
    Loop* loop{ nullptr };
    HalfEdge* twin_hedge{ nullptr };
    Edge* edge{ nullptr };
    Vertex* start_v{ nullptr };
    Vertex* end_v{ nullptr };
};

}
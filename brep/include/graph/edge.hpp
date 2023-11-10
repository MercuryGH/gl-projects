#pragma once

#include <graph/self_linked.hpp>

namespace brep {

struct HalfEdge;

struct Edge: SelfLinked<Edge> {
    HalfEdge* hedge1{ nullptr };
    HalfEdge* hedge2{ nullptr };

    int id; // id
};

}
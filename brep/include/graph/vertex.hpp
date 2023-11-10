#pragma once

#include <graph/self_linked.hpp>

namespace brep {

struct Point;

struct Vertex: SelfLinked<Vertex> {
    Point* coordinate{ nullptr };
};

}
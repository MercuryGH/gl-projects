#pragma once

#include <graph/self_linked.hpp>

namespace brep {

struct Face;
struct Edge;
struct Vertex;
struct Loop;

struct Solid: SelfLinked<Solid> {
    Face* face_head{ nullptr };
    Edge* edge_head{ nullptr };

    bool faces_dirty{ true };
    bool edges_dirty{ true };

    int n_faces();
    int n_edges();

    void add_face(Face* face);
    bool remove_face(Face* face);
    void add_edge(Edge* edge);
    /**
     * returns true if remove success
    */
    bool remove_edge(Edge* edge);
    Loop* get_loop(Vertex* start_vertex, Vertex* end_vertex) const;

    int n_f{ 0 };
    int n_e{ 0 };
};

}
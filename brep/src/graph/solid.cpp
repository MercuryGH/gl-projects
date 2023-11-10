#include <graph/solid.hpp>

#include <cassert>

#include <graph/face.hpp>
#include <graph/edge.hpp>
#include <graph/half_edge.hpp>
#include <graph/util.inl>

namespace brep {

int Solid::n_faces()
{
    if (faces_dirty == false) {
        return n_f;
    }

    n_f = count_nodes(face_head);
    faces_dirty = false;

    return n_f;
}

int Solid::n_edges()
{
    if (edges_dirty == false) {
        return n_e;
    }

    n_e = count_nodes(edge_head);
    edges_dirty = false;

    return n_e;
}

void Solid::add_face(Face *face)
{
    if (face == nullptr) {
        // ERR
        assert(false);
        return;
    }

    face->id = n_faces() + 1;
    faces_dirty = true;

    if (face_head == nullptr) {
        face_head = face;
        face_head->next = face;
        face_head->prev = face;
    } else {
        // insert at the front (between face1, face2)
        auto face1 = face_head;
        auto face2 = face_head->next;
        face1->next = face;
        face->prev = face1;
        face2->prev = face;
        face->next = face2;
    }
}

bool Solid::remove_face(Face *face) {
    if (face == nullptr) {
        assert(false);
        return false;
    }
    if (face_head == nullptr) {
        assert(false);
        return false;
    }

    auto cur_face = face_head;
    while (cur_face != face) {
        cur_face = cur_face->next;
        if (cur_face == face_head) {
            return false;
        }
    }

    faces_dirty = true;

    if (n_faces() == 1) {
        delete face_head;
        face_head = nullptr;
    } else {
        // remove
        auto face1 = cur_face->prev;
        auto face2 = cur_face->next;
        face1->next = face2;
        face2->prev = face1;
        if (cur_face == face_head) {
            face_head = face1;
        }
        delete cur_face;
    }
    return true;
}

void Solid::add_edge(Edge *edge) {
    if (edge == nullptr) {
        // ERR
        assert(false);
        return;
    }

    edge->id = n_edges() + 1;
    edges_dirty = true;

    if (edge_head == nullptr) {
        edge_head = edge;
        edge_head->next = edge;
        edge_head->prev = edge;
    } else {
        // insert at the front (between edge1, edge2)
        auto edge1 = edge_head;
        auto edge2 = edge_head->next;
        edge1->next = edge;
        edge->prev = edge1;
        edge2->prev = edge;
        edge->next = edge2;
    }
}

bool Solid::remove_edge(Edge *edge) {
    if (edge == nullptr) {
        assert(false);
        return false;
    }
    if (edge_head == nullptr) {
        assert(false);
        return false;
    }

    auto cur_edge = edge_head;
    while (cur_edge != edge) {
        cur_edge = cur_edge->next;
        if (cur_edge == edge_head) {
            return false;
        }
    }

    edges_dirty = true;

    if (n_edges() == 1) {
        delete edge_head;
        edge_head = nullptr;
    } else {
        // remove
        auto edge1 = cur_edge->prev;
        auto edge2 = cur_edge->next;
        edge1->next = edge2;
        edge2->prev = edge1;
        if (cur_edge == edge_head) {
            edge_head = edge1;
        }
        delete cur_edge;
    }
    return true;
}

Loop *Solid::get_loop(Vertex *start_vertex, Vertex *end_vertex) const {
    auto cur_edge = edge_head;

    do {
        auto hedge1 = cur_edge->hedge1;
        if (hedge1->start_v == start_vertex && hedge1->end_v == end_vertex) {
            return hedge1->loop;
        }
        auto hedge2 = cur_edge->hedge2;
        if (hedge2->start_v == start_vertex && hedge2->end_v == end_vertex) {
            return hedge2->loop;
        }

        cur_edge = cur_edge->next;        

    } while (cur_edge != edge_head);

    return nullptr;
}

}

#include <dataop/operation.hpp>

#include <cassert>

namespace brep {

std::pair<Solid*, Vertex*> mvsf(Point p) {
    Solid* solid = new Solid();
    Face* face = new Face();
    Loop* loop = new Loop();
    Point* point = new Point(p);

    Vertex* vertex = new Vertex{ .coordinate = point };

    solid->add_face(face);
    face->solid = solid;
    face->add_loop(loop);
    loop->face = face;

    return { solid, vertex };
}

HalfEdge* mev(Point p, Vertex* v, Loop* loop) {
    // twin hedges and the edge
    HalfEdge* hedge1 = new HalfEdge();
    HalfEdge* hedge2 = new HalfEdge();
    Edge* edge = new Edge();

    edge->hedge1 = hedge1;
    edge->hedge2 = hedge2;

    hedge1->edge = edge;
    hedge2->edge = edge;
    hedge1->twin_hedge = hedge2;
    hedge2->twin_hedge = hedge1;

    Point* new_point = new Point(p);
    Vertex* new_vertex = new Vertex{ .coordinate = new_point };

    hedge1->start_v = v;
    hedge1->end_v = new_vertex;
    hedge2->start_v = new_vertex;
    hedge2->end_v = v;

    hedge1->loop = loop;
    hedge2->loop = loop;

    hedge1->next = hedge2;
    hedge2->prev = hedge1;
    if (loop->hedge_head == nullptr) {
        // no hedge in the loop, assign hedges
        hedge1->prev = hedge2;
        hedge2->next = hedge1;
        loop->hedge_head = hedge1;
    } else {
        auto cur_hedge = loop->hedge_head;

        while (cur_hedge->end_v != v) {
            cur_hedge = cur_hedge->next;
            if (cur_hedge == loop->hedge_head) {
                // cannot find hedge with end vertex being v
                assert(false);
            }
        }

        // operation on new twin hedges added
        hedge2->next = cur_hedge->next;
        cur_hedge->next->prev = hedge2;
        cur_hedge->next = hedge1;
        hedge1->prev = cur_hedge;
    }

    Solid* solid = loop->face->solid;
    solid->add_edge(edge);

    return hedge1;
}

Loop* mef(Vertex* start_v, Vertex* end_v, Loop* main_loop) {
    HalfEdge* hedge1 = new HalfEdge();
    HalfEdge* hedge2 = new HalfEdge();
    Edge* edge = new Edge();

    edge->hedge1 = hedge1;
    edge->hedge2 = hedge2;

    hedge1->edge = edge;
    hedge2->edge = edge;
    hedge1->twin_hedge = hedge2;
    hedge2->twin_hedge = hedge1;

    hedge1->start_v = start_v;
    hedge1->end_v = end_v;
    hedge2->start_v = end_v;
    hedge2->end_v = start_v;

    Loop* new_loop = new Loop();

    HalfEdge* cur_hedge = main_loop->hedge_head;
    while (cur_hedge->start_v != start_v) {
        cur_hedge = cur_hedge->next;
    }
    // find hedge with specified start vertex
    HalfEdge* m_hedge1 = cur_hedge;

    cur_hedge = main_loop->hedge_head;
    while (cur_hedge->start_v != end_v) {
        cur_hedge = cur_hedge->next;
    }
    // find hedge with specified start vertex
    // draw a graph to see clearly
    HalfEdge* m_hedge2 = cur_hedge;

    // modify hedge topo
    hedge1->next = m_hedge2;
    hedge1->prev = m_hedge1->prev;
    hedge2->next = m_hedge1;
    hedge2->prev = m_hedge1->prev;

    m_hedge1->prev->next = hedge1;
    m_hedge1->prev = hedge2;
    m_hedge2->prev->next = hedge2;
    m_hedge2->prev = hedge1;

    new_loop->hedge_head = hedge2;
    main_loop->hedge_head = hedge1;

    // assign hedge loop for all hedges
    cur_hedge = new_loop->hedge_head;
    while (cur_hedge->next != new_loop->hedge_head) {
        cur_hedge->loop = new_loop;
        cur_hedge = cur_hedge->next;
    }
    cur_hedge = main_loop->hedge_head;
    while (cur_hedge->next != main_loop->hedge_head) {
        cur_hedge->loop = main_loop;
        cur_hedge = cur_hedge->next;
    }

    Solid* solid = main_loop->face->solid;
    Face* new_face = new Face();
    new_face->add_loop(new_loop);
    new_loop->face = new_face;
    new_face->solid = solid;
    solid->add_face(new_face);
    solid->add_edge(edge);

    return new_loop;
}

Loop* kemr(Vertex* v1, Vertex* v2, Loop* loop) {
    Loop* new_loop = new Loop{ .inner_loop = true };
    // Loop* new_loop = new Loop();
    
    HalfEdge* cur_hedge = loop->hedge_head;
    // find hedge with start_v = v1, end_v = v2
    while (cur_hedge->start_v != v1 || cur_hedge->end_v != v2) {
        cur_hedge = cur_hedge->next;
        if (cur_hedge == loop->hedge_head) {
            assert(false);
        }
    }

    Edge* kill_edge = cur_hedge->edge;
    HalfEdge* twin_hedge = cur_hedge->twin_hedge;

    cur_hedge->prev->next = twin_hedge->next;
    cur_hedge->next->prev = twin_hedge->prev;
    twin_hedge->next->prev = cur_hedge->prev;
    twin_hedge->prev->next = cur_hedge->next;

    new_loop->hedge_head = cur_hedge->prev;
    loop->hedge_head = cur_hedge->next;

    // assign hedge loop for hedges in the new (inner) loop
    cur_hedge = loop->hedge_head;
    while (cur_hedge->next != loop->hedge_head) {
        cur_hedge->loop = new_loop;
        cur_hedge = cur_hedge->next;
    }

    Face* face = loop->face;
    face->add_loop(new_loop);

    Solid* solid = face->solid;
    solid->remove_edge(kill_edge);

    return new_loop;
}

void kfmrh(Face* kill_face, Face* face) {
    if (kill_face->n_loops() > 1) {
        // not supported
        assert(false);
        return;
    }

    kill_face->loop_head->inner_loop = true;
    face->add_loop(kill_face->loop_head);

    Solid* solid = kill_face->solid;
    solid->remove_face(kill_face);
}

Solid* sweep(Face* face, Vector3 translation) {
    Solid* solid = face->solid;

    Loop* cur_loop = face->loop_head;
    do {
        auto cur_hedge = cur_loop->hedge_head;
        auto first_vertex = cur_hedge->start_v;

        Point new_pos{ .pos = first_vertex->coordinate->pos + translation };

        // assume translation is up, which forms a down-to-up hedge here
        HalfEdge* first_up_hedge = mev(new_pos, first_vertex, cur_loop);

        Vertex* last_up_vertex = first_up_hedge->end_v;
        cur_hedge = cur_hedge->next;
        Vertex* cur_vertex = cur_hedge->start_v;
        while (cur_vertex != first_vertex) {
            Point new_pos{ .pos = cur_vertex->coordinate->pos + translation };
            HalfEdge* up_hedge = mev(new_pos, cur_vertex, cur_loop);
            Vertex* up_vertex = up_hedge->end_v;
            mef(last_up_vertex, up_vertex, cur_loop);

            last_up_vertex = up_vertex;
            cur_hedge = cur_hedge->next;
            cur_vertex = cur_hedge->start_v;
        }

        mef(last_up_vertex, first_up_hedge->end_v, cur_loop);

        cur_loop = cur_loop->next;
    } while (cur_loop != face->loop_head);

    return solid;
}

}
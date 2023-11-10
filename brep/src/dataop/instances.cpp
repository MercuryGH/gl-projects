#include <dataop/instances.hpp>

#include <cassert>
#include <algorithm>

#include <dataop/graph.hpp>
#include <dataop/operation.hpp>

namespace brep {

Solid* create_solid() {
    Point points[] = {
        Point{ .pos = Vector3(-1.0, 0.5, 2.0) },
        Point{ .pos = Vector3(1.0, 0.5, 2.0) },
        Point{ .pos = Vector3(-1.0, 0.5, -2.0) },
        Point{ .pos = Vector3(1.0, 0.5, -2.0) },

        Point{ .pos = Vector3(0.0, -1.0, 0.0) },
    };

    auto [solid, top_upper_left] = mvsf(points[0]);
    auto top_loop = solid->face_head->loop_head;

    auto top_upper_hedge = mev(points[1], top_upper_left, top_loop);
    auto top_upper_right = top_upper_hedge->end_v;

    auto top_left_hedge = mev(points[2], top_upper_left, top_loop);
    auto top_bottom_left = top_left_hedge->end_v;

    auto top_right_hedge = mev(points[3], top_upper_right, top_loop);
    auto top_bottom_right = top_right_hedge->end_v;

    auto down_loop = mef(top_bottom_left, top_bottom_right, top_loop);
    sweep(down_loop->face, points[4].pos);

    return solid;
}

Solid* create_holed_cuboid() {
    Point points[] = {
        Point{ .pos = Vector3(-1.0, 0.5, 2.0) },
        Point{ .pos = Vector3(1.0, 0.5, 2.0) },
        Point{ .pos = Vector3(1.0, 0.5, -2.0) },
        Point{ .pos = Vector3(-1.0, 0.5, -2.0) },

        Point{ .pos = Vector3(-1.0, -0.5, 2.0) },
        Point{ .pos = Vector3(1.0, -0.5, 2.0) },
        Point{ .pos = Vector3(1.0, -0.5, -2.0) },
        Point{ .pos = Vector3(-1.0, -0.5, -2.0) },

        Point{ .pos = Vector3(-0.5, 0.5, 1.5) },
        Point{ .pos = Vector3(0.5, 0.5, 1.5) },
        Point{ .pos = Vector3(0.5, 0.5, -1.5) },
        Point{ .pos = Vector3(-0.5, 0.5, -1.5) },

        Point{ .pos = Vector3(-0.5, -0.5, 1.5) },
        Point{ .pos = Vector3(0.5, -0.5, 1.5) },
        Point{ .pos = Vector3(0.5, -0.5, -1.5) },
        Point{ .pos = Vector3(-0.5, -0.5, -1.5) },
    };

    HalfEdge* top_hedges[4];
    Vertex* top_vertices[4];
    auto [solid, top_upper_left] = mvsf(points[0]);
    top_vertices[0] = top_upper_left;
    auto top_loop = solid->face_head->loop_head;

    for (int i = 1; i < 4; i++) {
        top_hedges[i] = mev(points[i], top_vertices[i - 1], top_loop);
        top_vertices[i] = top_hedges[i]->end_v;
    }

    // to get the new loop: vertex params order matters
    auto down_loop = mef(top_vertices[0], top_vertices[3], top_loop);

    HalfEdge* side_hedges[4];
    Vertex* down_vertices[4];
    for (int i = 0; i < 4; i++) {
        side_hedges[i] = mev(points[i + 4], top_vertices[i], down_loop);
        down_vertices[i] = side_hedges[i]->end_v;
    }

    for (int i = 0; i < 4; i++) {
        (void*)mef(down_vertices[i], down_vertices[(i + 1) % 4], down_loop);
    }

    // inner

    HalfEdge* top_inner_hedges[4]; // first element is a bridge
    Vertex* top_inner_vertices[4];
    top_inner_hedges[0] = mev(points[8], top_vertices[0], top_loop);
    top_inner_vertices[0] = top_inner_hedges[0]->end_v;
    for (int i = 1; i < 4; i++) {
        top_inner_hedges[i] = mev(points[i + 8], top_inner_vertices[i - 1], top_loop);
        top_inner_vertices[i] = top_inner_hedges[i]->end_v;
    }

    auto down_inner_loop = mef(top_inner_vertices[0], top_inner_vertices[3], top_loop);

    auto top_inner_loop = kemr(top_vertices[0], top_inner_vertices[0], top_loop);

    std::swap(top_loop, top_inner_loop);

    HalfEdge* side_inner_hedges[4];
    Vertex* down_inner_vertices[4];
    for (int i = 0; i < 4; i++) {
        side_inner_hedges[i] = mev(points[i + 12], top_inner_vertices[i], down_inner_loop);
        down_inner_vertices[i] = side_inner_hedges[i]->end_v;
    }

    for (int i = 0; i < 4; i++) {
        (void*)mef(down_inner_vertices[i], down_inner_vertices[(i + 1) % 4], down_inner_loop);
    }

    kfmrh(down_inner_loop->face, down_loop->face);

    return solid;
}

Solid* create_double_holed_cuboid() {
    Point points[] = {
        Point{ .pos = Vector3(-1.0, 0.5, 2.0) },
        Point{ .pos = Vector3(1.0, 0.5, 2.0) },
        Point{ .pos = Vector3(1.0, 0.5, -2.0) },
        Point{ .pos = Vector3(-1.0, 0.5, -2.0) },

        Point{ .pos = Vector3(-1.0, -0.5, 2.0) },
        Point{ .pos = Vector3(1.0, -0.5, 2.0) },
        Point{ .pos = Vector3(1.0, -0.5, -2.0) },
        Point{ .pos = Vector3(-1.0, -0.5, -2.0) },

        Point{ .pos = Vector3(-0.5, 0.5, 1.5) },
        Point{ .pos = Vector3(0.5, 0.5, 1.5) },
        Point{ .pos = Vector3(0.5, 0.5, 0.5) },
        Point{ .pos = Vector3(-0.5, 0.5, 0.5) },

        Point{ .pos = Vector3(-0.5, -0.5, 1.5) },
        Point{ .pos = Vector3(0.5, -0.5, 1.5) },
        Point{ .pos = Vector3(0.5, -0.5, 0.5) },
        Point{ .pos = Vector3(-0.5, -0.5, 0.5) },

        Point{ .pos = Vector3(-0.7, 0.5, -0.8) },
        Point{ .pos = Vector3(0.7, 0.5, -0.8) },
        Point{ .pos = Vector3(0.7, 0.5, -1.5) },
        Point{ .pos = Vector3(-0.7, 0.5, -1.5) },

        Point{ .pos = Vector3(-0.7, -0.5, -0.8) },
        Point{ .pos = Vector3(0.7, -0.5, -0.8) },
        Point{ .pos = Vector3(0.7, -0.5, -1.5) },
        Point{ .pos = Vector3(-0.7, -0.5, -1.5) },
    };

    HalfEdge* top_hedges[4];
    Vertex* top_vertices[4];
    auto [solid, top_upper_left] = mvsf(points[0]);
    top_vertices[0] = top_upper_left;
    auto top_loop = solid->face_head->loop_head;

    for (int i = 1; i < 4; i++) {
        top_hedges[i] = mev(points[i], top_vertices[i - 1], top_loop);
        top_vertices[i] = top_hedges[i]->end_v;
    }

    // to get the new loop: vertex params order matters
    auto down_loop = mef(top_vertices[0], top_vertices[3], top_loop);

    HalfEdge* side_hedges[4];
    Vertex* down_vertices[4];
    for (int i = 0; i < 4; i++) {
        side_hedges[i] = mev(points[i + 4], top_vertices[i], down_loop);
        down_vertices[i] = side_hedges[i]->end_v;
    }

    for (int i = 0; i < 4; i++) {
        (void*)mef(down_vertices[i], down_vertices[(i + 1) % 4], down_loop);
    }

    // Create the first hole

    HalfEdge* top_inner_hedges[4]; // first element is a bridge
    Vertex* top_inner_vertices[4];
    top_inner_hedges[0] = mev(points[8], top_vertices[0], top_loop);
    top_inner_vertices[0] = top_inner_hedges[0]->end_v;
    for (int i = 1; i < 4; i++) {
        top_inner_hedges[i] = mev(points[i + 8], top_inner_vertices[i - 1], top_loop);
        top_inner_vertices[i] = top_inner_hedges[i]->end_v;
    }

    auto down_inner_loop = mef(top_inner_vertices[0], top_inner_vertices[3], top_loop);

    auto top_inner_loop = kemr(top_vertices[0], top_inner_vertices[0], top_loop);

    std::swap(top_loop, top_inner_loop);

    HalfEdge* side_inner_hedges[4];
    Vertex* down_inner_vertices[4];
    for (int i = 0; i < 4; i++) {
        side_inner_hedges[i] = mev(points[i + 12], top_inner_vertices[i], down_inner_loop);
        down_inner_vertices[i] = side_inner_hedges[i]->end_v;
    }

    for (int i = 0; i < 4; i++) {
        (void*)mef(down_inner_vertices[i], down_inner_vertices[(i + 1) % 4], down_inner_loop);
    }

    kfmrh(down_inner_loop->face, down_loop->face);

    // Create the second hole

    HalfEdge* top_inner_hedges_2[4]; // first element is a bridge
    Vertex* top_inner_vertices_2[4];
    top_inner_hedges_2[0] = mev(points[16], top_vertices[0], top_loop);
    top_inner_vertices_2[0] = top_inner_hedges_2[0]->end_v;
    for (int i = 1; i < 4; i++) {
        top_inner_hedges_2[i] = mev(points[i + 16], top_inner_vertices_2[i - 1], top_loop);
        top_inner_vertices_2[i] = top_inner_hedges_2[i]->end_v;
    }

    auto down_inner_loop_2 = mef(top_inner_vertices_2[0], top_inner_vertices_2[3], top_loop);

    auto top_inner_loop_2 = kemr(top_vertices[0], top_inner_vertices_2[0], top_loop);

    // std::swap(top_loop, top_inner_loop);

    HalfEdge* side_inner_hedges_2[4];
    Vertex* down_inner_vertices_2[4];
    for (int i = 0; i < 4; i++) {
        side_inner_hedges_2[i] = mev(points[i + 20], top_inner_vertices_2[i], down_inner_loop_2);
        down_inner_vertices_2[i] = side_inner_hedges_2[i]->end_v;
    }

    for (int i = 0; i < 4; i++) {
        (void*)mef(down_inner_vertices_2[i], down_inner_vertices_2[(i + 1) % 4], down_inner_loop_2);
    }

    kfmrh(down_inner_loop_2->face, down_loop->face);

    return solid;
}

Solid* create_complicated() {
    Point top_points[] = {
        Point{ .pos = Vector3(-1.0, 0.3, 1.0) },
        Point{ .pos = Vector3(-1.0, 0.3, 0.6) },
        Point{ .pos = Vector3(-0.3, 0.3, 0.6) },
        Point{ .pos = Vector3(-0.3, 0.3, -0.6) },
        Point{ .pos = Vector3(-1.0, 0.3, -0.6) },
        Point{ .pos = Vector3(-1.0, 0.3, -1.0) },
        Point{ .pos = Vector3(1.0, 0.3, -1.0) },
        Point{ .pos = Vector3(1.0, 0.3, -0.6) },
        Point{ .pos = Vector3(0.3, 0.3, -0.6) },
        Point{ .pos = Vector3(0.3, 0.3, 0.6) },
        Point{ .pos = Vector3(1.0, 0.3, 0.6) },
        Point{ .pos = Vector3(1.0, 0.3, 1.0) },
    };

    Point top_inner_points[] = {
        Point{ .pos = Vector3(-0.2, 0.3, 0.6) },
        Point{ .pos = Vector3(-0.2, 0.3, -0.6) },
        Point{ .pos = Vector3(0.2, 0.3, -0.6) },
        Point{ .pos = Vector3(0.2, 0.3, 0.6) },
    };

    Point shallow_hole_1_points_1[] = {
        Point{ .pos = Vector3(-0.9, 0.2, -1.0) },
        Point{ .pos = Vector3(-0.9, -0.2, -1.0) },
        Point{ .pos = Vector3(-0.5, -0.2, -1.0) },
        Point{ .pos = Vector3(-0.5, 0.2, -1.0) },
    };
    Point shallow_hole_1_points_2[] = {
        Point{ .pos = Vector3(-0.9, 0.2, -0.6) },
        Point{ .pos = Vector3(-0.9, -0.2, -0.6) },
        Point{ .pos = Vector3(-0.5, -0.2, -0.6) },
        Point{ .pos = Vector3(-0.5, 0.2, -0.6) },
    };

    Point shallow_hole_2_points_1[] = {
        Point{ .pos = Vector3(0.5, 0.2, -1.0) },
        Point{ .pos = Vector3(0.5, -0.2, -1.0) },
        Point{ .pos = Vector3(0.9, -0.2, -1.0) },
        Point{ .pos = Vector3(0.9, 0.2, -1.0) },
    };
    Point shallow_hole_2_points_2[] = {
        Point{ .pos = Vector3(0.5, 0.2, -0.6) },
        Point{ .pos = Vector3(0.5, -0.2, -0.6) },
        Point{ .pos = Vector3(0.9, -0.2, -0.6) },
        Point{ .pos = Vector3(0.9, 0.2, -0.6) },
    };

    Point deep_hole_points_1[] = {
        Point{ .pos = Vector3(-1.0, 0.2, 0.9) },
        Point{ .pos = Vector3(-1.0, -0.2, 0.9) },
        Point{ .pos = Vector3(-1.0, -0.2, 0.7) },
        Point{ .pos = Vector3(-1.0, 0.2, 0.7) },
    };
    Point deep_hole_points_2[] = {
        Point{ .pos = Vector3(1.0, 0.2, 0.9) },
        Point{ .pos = Vector3(1.0, -0.2, 0.9) },
        Point{ .pos = Vector3(1.0, -0.2, 0.7) },
        Point{ .pos = Vector3(1.0, 0.2, 0.7) },
    };

    auto [solid, v0] = mvsf(top_points[0]);
    auto top_loop = solid->face_head->loop_head;

    Vertex* top_vertices[12];
    top_vertices[0] = v0;

    /* PART 1: create top vertices */

    auto prev_vertex = v0;
    for (int i = 1; i < 12; i++) {
        HalfEdge* hedge = mev(top_points[i], prev_vertex, top_loop);
        prev_vertex = hedge->end_v;
        top_vertices[i] = prev_vertex;
    }

    auto down_loop = mef(prev_vertex, v0, top_loop);

    prev_vertex = v0;
    Vertex* loop_start_vertex = nullptr;
    for (int i = 0; i < 4; i++) {
        HalfEdge* hedge = mev(top_inner_points[i], prev_vertex, top_loop);
        prev_vertex = hedge->end_v;
        if (i == 0) {
            loop_start_vertex = prev_vertex;
        }
    }

    auto down_inner_loop = mef(loop_start_vertex, prev_vertex, top_loop);
    auto top_inner_loop = kemr(v0, loop_start_vertex, top_loop);
    std::swap(top_loop, top_inner_loop);
    kfmrh(down_inner_loop->face, down_loop->face);

    /* PART 1.5: create bottom vertices */

    constexpr float k_solid_height = 0.6;
    sweep(down_loop->face, Vector3(0.0, -k_solid_height, 0.0));

    auto back_loop = solid->get_loop(top_vertices[6], top_vertices[5]);

    /* PART 2: create shallow hole 1 */

    prev_vertex = top_vertices[5];
    Vertex* shallow_hole_face_1_vertices[4];
    for (int i = 0; i < 4; i++) {
        HalfEdge* hedge = mev(shallow_hole_1_points_1[i], prev_vertex, back_loop);
        prev_vertex = hedge->end_v;
        shallow_hole_face_1_vertices[i] = prev_vertex;
    }

    auto through_inner_loop = mef(shallow_hole_face_1_vertices[0], prev_vertex, back_loop);
    auto back_inner_loop = kemr(top_vertices[5], shallow_hole_face_1_vertices[0], back_loop);
    std::swap(back_loop, back_inner_loop);

    Vertex* through_inner_loop_vertices[4];
    for (int i = 0; i < 4; i++) {
        HalfEdge* hedge = mev(shallow_hole_1_points_2[i], shallow_hole_face_1_vertices[i], through_inner_loop);
        prev_vertex = hedge->end_v;
        through_inner_loop_vertices[i] = prev_vertex;
    }

    for (int i = 0; i < 4; i++) {
        mef(through_inner_loop_vertices[i], through_inner_loop_vertices[(i + 1) % 4], through_inner_loop);
    }

    auto other_side_loop_1 = solid->get_loop(top_vertices[4], top_vertices[3]);
    kfmrh(through_inner_loop->face, other_side_loop_1->face);

    /* PART 2: create shallow hole 2 */

    prev_vertex = top_vertices[5];
    Vertex* shallow_hole_face_2_vertices[4];
    for (int i = 0; i < 4; i++) {
        HalfEdge* hedge = mev(shallow_hole_2_points_1[i], prev_vertex, back_loop);
        prev_vertex = hedge->end_v;
        shallow_hole_face_2_vertices[i] = prev_vertex;
    }

    auto through_inner_loop_2 = mef(shallow_hole_face_2_vertices[0], prev_vertex, back_loop);
    auto back_inner_loop_2 = kemr(top_vertices[5], shallow_hole_face_2_vertices[0], back_loop);

    std::swap(back_loop, back_inner_loop_2);

    Vertex* through_inner_loop_2_vertices[4];
    for (int i = 0; i < 4; i++) {
        HalfEdge* hedge = mev(shallow_hole_2_points_2[i], shallow_hole_face_2_vertices[i], through_inner_loop_2);
        prev_vertex = hedge->end_v;
        through_inner_loop_2_vertices[i] = prev_vertex;
    }

    for (int i = 0; i < 4; i++) {
        mef(through_inner_loop_2_vertices[i], through_inner_loop_2_vertices[(i + 1) % 4], through_inner_loop_2);
    }
    auto other_side_loop = solid->get_loop(top_vertices[8], top_vertices[7]);
    kfmrh(through_inner_loop_2->face, other_side_loop->face);

    /* PART 3: create deep hole */

    auto through_left_loop = solid->get_loop(top_vertices[1], top_vertices[0]);
    prev_vertex = top_vertices[0];
    Vertex* deep_hole_face_vertices[4];
    for (int i = 0; i < 4; i++) {
        HalfEdge* hedge = mev(deep_hole_points_1[i], prev_vertex, through_left_loop);
        prev_vertex = hedge->end_v;
        deep_hole_face_vertices[i] = prev_vertex;
    }

    auto through_right_inner_loop = mef(deep_hole_face_vertices[0], prev_vertex, through_left_loop);
    auto through_left_inner_loop = kemr(top_vertices[0], deep_hole_face_vertices[0], through_left_loop);
    std::swap(through_left_loop, through_left_inner_loop);

    Vertex* through_right_inner_loop_vertices[4];
    for (int i = 0; i < 4; i++) {
        HalfEdge* hedge = mev(deep_hole_points_2[i], deep_hole_face_vertices[i], through_right_inner_loop);
        prev_vertex = hedge->end_v;
        through_right_inner_loop_vertices[i] = prev_vertex;
    }

    for (int i = 0; i < 4; i++) {
        mef(through_right_inner_loop_vertices[i], through_right_inner_loop_vertices[(i + 1) % 4], through_right_inner_loop);
    }

    other_side_loop = solid->get_loop(top_vertices[11], top_vertices[10]);
    kfmrh(through_right_inner_loop->face, other_side_loop->face);

    return solid;
}

}
#pragma once

#include <util/types.hpp>

#include <dataop/graph.hpp>

namespace brep {

/**
 * make vertex solid face (+ outer loop)
*/
std::pair<Solid*, Vertex*> mvsf(Point p);

/**
 * make edge vertex by a given vertex
*/
HalfEdge* mev(Point p, Vertex* v, Loop* loop);

/**
 * make edge face by 2 given vertices (+ loop)
*/
Loop* mef(Vertex* start_v, Vertex* end_v, Loop* main_loop);

/**
 * kill edge make ring (inner loop)
 * v1: inner loop, v2: outer loop
*/
Loop* kemr(Vertex* v1, Vertex* v2, Loop* loop);

/**
 * kill face make ring hole
*/
void kfmrh(Face* kill_face, Face* face);

/**
 * translate a face to form a 3D solid
*/
Solid* sweep(Face* face, Vector3 translation);

}
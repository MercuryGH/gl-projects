#pragma once

#include <util/types.hpp>

namespace pathtrace {

struct IMaterial;
struct IHittable;
struct IPrimitive;

struct HitRecord {
    Vector3 pos;
    ScalarType t;
    Vector3 normal;
    Vector2 uv;
    const IMaterial* material;
    const IHittable* hit_object;
    const IPrimitive* hit_geometry;
};

}
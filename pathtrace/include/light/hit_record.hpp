#pragma once

#include <util/types.hpp>

namespace pathtrace {

struct IMaterial;
struct IHittable;

struct HitRecord {
    Vector3 pos;
    Vector3 normal;
    ScalarType t;
    Vector2 uv;
    const IMaterial& material;
    const IHittable& hit_object;
};

}
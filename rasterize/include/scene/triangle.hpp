#pragma once

#include <util/types.hpp>
#include <zbuffer/color.hpp>

namespace rasterize {

struct Triangle {
    Vector3 p[3];

    bool contains_point(Vector2 point) const;

    // point must be in triangle
    std::tuple<ScalarType, ScalarType, ScalarType> compute_barycentric_2d(Vector2 point) const;
    ScalarType interpolate_depth(Vector2 point) const;

    RgbColor get_color() const;
};

}
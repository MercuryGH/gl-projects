#pragma once

#include <util/types.hpp>
#include <zbuffer/color.hpp>
#include <zbuffer/zbuffer_pixel.hpp>

#include <camera/camera.hpp>

namespace rasterize {

struct Triangle {
    Vector3 p[3];

    bool contains_point(Vector2 point) const;

    // point must be in triangle
    std::tuple<ScalarType, ScalarType, ScalarType> compute_barycentric_2d(Vector2 point) const;
    ScalarType interpolate_depth(Vector2 point) const;
    ScalarType get_min_depth() const { return std::min(p[0].z, std::min(p[1].z, p[2].z)); }

    // view projection viewport
    Triangle vpv_transform(const renderer::CameraData& camera, Vector4 viewport) const;
};

}

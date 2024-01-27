#pragma once

#include <memory>

#include <util/types.hpp>
#include <glh/gl_buffer.hpp>

#include <tinynurbs.h>

namespace nurbs {

struct SurfaceVertex {
    Vector3 pos;
    Vector3 normal;
};

struct LineVertex {
    Vector3 pos;
};

using namespace renderer;

// 1-dimension draw line
std::unique_ptr<GlBuffer> create_line_vbo(const std::vector<Vector3>& vertices, uint32_t vao, uint32_t usage=0);

// 1. draw equal length line in the surface
// 2. draw surface control points lines
auto create_line_vbo(const std::vector<std::vector<Vector3>>& vertices, uint32_t vao, uint32_t usage=0) -> std::tuple<std::unique_ptr<GlBuffer>, std::unique_ptr<GlBuffer>, uint32_t>;

// draw blinn-phong shaded surface
auto create_surface_vbo(const std::vector<std::vector<Vector3>>& vertices, const std::vector<std::vector<Vector3>>& normals, uint32_t vao, uint32_t usage=0) -> std::tuple<std::unique_ptr<GlBuffer>, std::unique_ptr<GlBuffer>, uint32_t>;

}
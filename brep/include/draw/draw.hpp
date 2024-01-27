#pragma once

#include <glh/gl_buffer.hpp>
#include <util/types.hpp>

#include <memory>

namespace brep {
    using namespace renderer;

struct VertexAttr {
    Vector3 pos;
    Vector3 normal;
};

struct Solid;

std::unique_ptr<GlBuffer> draw_triangles(Solid* solid, uint32_t tri_vao);

std::unique_ptr<GlBuffer> draw_wireframes(Solid* solid, uint32_t wireframe_vao);

}
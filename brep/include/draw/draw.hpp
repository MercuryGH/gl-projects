#pragma once

#include <glh/resource.hpp>

#include <memory>

namespace brep {
    using namespace renderer;

struct Solid;

std::unique_ptr<GlBuffer> draw_triangles(Solid* solid, uint32_t tri_vao);

std::unique_ptr<GlBuffer> draw_wireframes(Solid* solid, uint32_t wireframe_vao);

}
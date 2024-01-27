#pragma once

#include <vector>

#include <tinynurbs.h>

#include <util/types.hpp>
#include <glh/gl_buffer.hpp>

namespace nurbs {

using namespace renderer;

// stores rendering data
struct Scene {
    Scene() = default;
    ~Scene();

    void load(const std::vector<tinynurbs::RationalCurve<ScalarType>>& curves, const std::vector<tinynurbs::RationalSurface<ScalarType>>& surfaces);
    void clear();

    void tick(float curve_u, float surface_u, float surface_v);

    // Scene data
	std::vector<tinynurbs::RationalCurve<ScalarType>> curves;
	std::vector<tinynurbs::RationalSurface<ScalarType>> surfaces;

    // Render data
	struct GlRender {
		uint32_t vao{ 0 };
		std::unique_ptr<GlBuffer> vbo{ nullptr };

        // selectable
        std::unique_ptr<GlBuffer> ebo{ nullptr };
        uint32_t n_indices{ 0 };
	};
    void render_lines(const std::vector<GlRender>& line_draw_data);
    void render_triangles(const std::vector<GlRender>& tri_draw_data);

	// #curves
	std::vector<GlRender> curve_draw_data;
    void render_curves() {
        render_lines(curve_draw_data);
    }
	std::vector<GlRender> curve_control_point_lines;
    void render_curve_control_point_lines() {
        render_lines(curve_control_point_lines);
    }
	std::vector<GlRender> curve_derivative_lines;
    void render_curve_derivative_lines() {
        render_lines(curve_derivative_lines);
    }

	// #surfaces
	std::vector<GlRender> surface_draw_data;
    void render_surfaces() {
        render_triangles(surface_draw_data);
    }
	std::vector<GlRender> surface_mesh_lines;
    void render_surface_mesh_lines() {
        render_lines(surface_mesh_lines);
    }
	std::vector<GlRender> surface_control_point_lines;
    void render_surface_control_point_lines() {
        render_lines(surface_control_point_lines);
    }
	std::vector<GlRender> surface_normal_lines;
    void render_surface_normal_lines() {
        render_lines(surface_normal_lines);
    }
	std::vector<GlRender> surface_u_derivative_lines;
    void render_surface_u_derivative_lines() {
        render_lines(surface_u_derivative_lines);
    }
	std::vector<GlRender> surface_v_derivative_lines;
    void render_surface_v_derivative_lines() {
        render_lines(surface_v_derivative_lines);
    }
};

}
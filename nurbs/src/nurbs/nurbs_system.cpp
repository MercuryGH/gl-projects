#include <nurbs/nurbs_system.hpp>
#include <glh/pipeline.hpp>

#include <numbers>

#include <util/types.hpp>

#include <glad/glad.h>

#include <draw/draw.hpp>
#include <algorithm/skinning.hpp>

namespace nurbs {

namespace {
	struct alignas(16) RenderSurfaceParams {
		Vector4 color;
		int two_side;
	};

	struct alignas(16) RenderLineParams {
		Vector4 color;
	};

    const Vector4 k_red_color{ 0.9, 0.1, 0.1, 1 };
    const Vector4 k_black_color{ 0.0, 0.0, 0.0, 1 };
    const Vector4 k_green_color{ 0.3, 0.8, 0.3, 1 };
}

NurbsSystem::NurbsSystem() {
	ui = std::make_unique<NurbsSystemUI>(state);

	draw_surface_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderSurfaceParams), GL_MAP_WRITE_BIT);

	draw_red_line_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderLineParams), 0, (void*)(&k_red_color));
    draw_green_line_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderLineParams), 0, (void*)(&k_green_color));
    draw_black_line_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderLineParams), 0, (void*)(&k_black_color));

    draw_curve_params_buffer = std::make_unique<GlBuffer>(sizeof(RenderLineParams), GL_MAP_WRITE_BIT);

	draw_surface_program = build_graphics_program("plain/plain.vert.spv", "plain/plain.frag.spv");
	draw_line_program = build_graphics_program("plain/plain.vert.spv", "plain/one_color.frag.spv");
}

NurbsSystem::~NurbsSystem() {
}

void NurbsSystem::update(float delta_time) {
	ui->update(delta_time);

    if (ui->draw_dirty) {
        // reload curve data
        std::vector<tinynurbs::RationalCurve<ScalarType>> curves;
        std::vector<tinynurbs::RationalSurface<ScalarType>> surfaces;
        state.instantiate_caller.at(state.cur_data_idx)(curves);

        // This value may change as long as degree < curves.size() is satisfied.
        constexpr int result_degree = 2; 
        std::vector<tinynurbs::RationalCurve<ScalarType>> new_curves; // not used, but can be visualized
        // skinning
        auto surface = skinning(curves, result_degree, new_curves);
        surfaces.push_back(surface);

        scene.load(curves, surfaces);

        ui->draw_dirty = false;
    }

    scene.tick(state.curve_u, state.surface_u, state.surface_v);
    draw();
}

void NurbsSystem::draw() {
	// update render params
	if (ui->surface_render_settings_dirty) {
		auto data = draw_surface_params_buffer->typed_map<RenderSurfaceParams>(true);
		data->color = state.surface_render_settings.color;
		data->two_side = state.surface_render_settings.two_side ? 1 : 0;
		draw_surface_params_buffer->unmap();
		ui->surface_render_settings_dirty = false;
	}
    if (ui->curve_render_settings_dirty) {
		auto data = draw_curve_params_buffer->typed_map<RenderLineParams>(true);
		data->color = state.curve_render_settings.color;
		draw_curve_params_buffer->unmap();
		ui->curve_render_settings_dirty = false;
    }

    // draw surfaces
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

	glUseProgram(draw_surface_program->id());
	uint32_t buffers[] = {
		camera_buffer->id(),
		draw_surface_params_buffer->id(),
	};
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers);
    if (state.show_surfaces) {
        scene.render_surfaces();
    }

    // draw lines
	glUseProgram(draw_line_program->id());

    // draw black lines
	buffers[1] = draw_black_line_params_buffer->id();
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers);
    if (state.show_surfaces) {
        scene.render_surface_mesh_lines();
    }

    // use red lines
    buffers[1] = draw_red_line_params_buffer->id();
    glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers);
    if (state.show_surface_tangent) {
        scene.render_surface_u_derivative_lines();
        scene.render_surface_v_derivative_lines();
    }
    if (state.show_surface_normal) {
        scene.render_surface_normal_lines();
    }
    if (state.show_curve_derivative) {
        scene.render_curve_derivative_lines();
    }

    // use green lines
    buffers[1] = draw_green_line_params_buffer->id();
    glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers);
    if (state.show_surface_control_mesh) {
        scene.render_surface_control_point_lines();
    }
    if (state.show_curve_control_points) {
        scene.render_curve_control_point_lines();
    }

	buffers[1] = draw_curve_params_buffer->id();
	glBindBuffersBase(GL_UNIFORM_BUFFER, 1, 2, buffers);
    if (state.show_curves) {
        scene.render_curves();
    }
}

}
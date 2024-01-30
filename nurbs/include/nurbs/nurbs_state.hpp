#pragma once

#include <functional>

#include <util/types.hpp>

#include <tinynurbs.h>

#include <data/data.hpp>

namespace nurbs {

class NurbsSystemState {
    friend class NurbsSystemUI;
    friend class NurbsSystem;

private:
    using DataProvider = std::function<void(std::vector<tinynurbs::RationalCurve<ScalarType>>&)>;

    std::vector<DataProvider> instantiate_caller = {
        &assign_data_11,
        &assign_data_12,
        &assign_data_21,
        &assign_data_22,
    };

    int cur_data_idx{ 0 };

	struct {
		glm::vec4 color{ (float)0x66 / 0xff, (float)0xcc / 0xff, (float)0xff / 0xff, 1.0f };
        bool two_side{ true };
	} surface_render_settings;

    struct {
        glm::vec4 color{ (float)0xf7 / 0xff, (float)0xd1 / 0xff, (float)0xff / 0xff, 1.0f };
    } curve_render_settings;

    bool show_grid{ true };

    // curve
    float curve_u{ 0 };
    bool show_curves{ true };
    bool show_curve_derivative{ true };
    bool show_curve_control_points{ true };

    // surface
    float surface_u{ 0 };
    float surface_v{ 0 };
    bool show_surfaces{ true };
    bool show_surface_tangent{ true };
    bool show_surface_normal{ true };
    bool show_surface_control_mesh{ true };
};

}

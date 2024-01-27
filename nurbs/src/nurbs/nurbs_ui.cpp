#include <nurbs/nurbs_ui.hpp>

#include <imgui.h>

#include <util/types.hpp>

namespace nurbs {

NurbsSystemUI::NurbsSystemUI(NurbsSystemState& state): state(state) {
}

void NurbsSystemUI::update(float delta_time) {
	draw_ui();
}

void NurbsSystemUI::draw_ui() {
	if (ImGui::Begin("Nurbs System")) {
		ImGui::Text("Test cases:");

		const char* instances[] = {
			"Data 1-1",
			"Data 1-2",
			"Data 2-1",
			"Data 2-2",
		};
		ImGui::PushItemWidth(-1);
		ImGui::Combo("##", &state.cur_data_idx, instances, IM_ARRAYSIZE(instances));
		ImGui::PopItemWidth();

		draw_dirty |= ImGui::Button("Load data", ImVec2(-1, 0));

		ImGui::Separator();

		// toggle show
		ImGui::Checkbox("show curves", &state.show_curves);
		ImGui::SliderFloat("curve u", &state.curve_u, 0.0f, 1.0f);
		ImGui::Checkbox("show curve derivative", &state.show_curve_derivative);
		ImGui::Checkbox("show curve control points", &state.show_curve_control_points);

		ImGui::Separator();

		ImGui::Checkbox("show surfaces", &state.show_surfaces);
		ImGui::SliderFloat("surface u", &state.surface_u, 0.0f, 1.0f);
		ImGui::SliderFloat("surface v", &state.surface_v, 0.0f, 1.0f);
		ImGui::Checkbox("show surface tangent", &state.show_surface_tangent);
		ImGui::Checkbox("show surface normal", &state.show_surface_normal);
		ImGui::Checkbox("show surface control mesh", &state.show_surface_control_mesh);

		ImGui::Separator();

		curve_render_settings_dirty |= ImGui::ColorEdit4("curve color", &state.curve_render_settings.color.x);

		surface_render_settings_dirty |= ImGui::ColorEdit4("surface color", &state.surface_render_settings.color.x);
		surface_render_settings_dirty |= ImGui::Checkbox("two side shading", &state.surface_render_settings.two_side);
	}
	ImGui::End();
}

}
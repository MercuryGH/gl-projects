#include <pathtrace/pathtrace_ui.hpp>

#include <imgui.h>

namespace pathtrace {

PathtraceSystemUI::PathtraceSystemUI(PathtraceSystemState& state): state(state) {
}

void PathtraceSystemUI::update(float delta_time) {
	draw_ui();
}

void PathtraceSystemUI::draw_ui() {
	if (ImGui::Begin("Path Tracer")) {

		// select scene
		ImGui::Text("Scene:");
		const char* instances[] = {
			"Cube",
			"Sphere",
			"Bunny",
			"Armadillo",
			"Culled Armadillo",
		};
		ImGui::PushItemWidth(-1);
		ImGui::Combo("##", &state.cur_scene_id, instances, IM_ARRAYSIZE(instances));
		ImGui::PopItemWidth();

		scene_dirty |= ImGui::Button("Load scene", ImVec2(-1, 0));

		ImGui::Text("#Tri: %d", state.n_triangles);

		ImGui::Separator();

		ImGui::InputInt("#spp", &state.spp);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6, 0.6));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7, 0.7));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8, 0.8));
		draw_dirty |= ImGui::Button("Render", ImVec2(-1, 0));
		ImGui::PopStyleColor(3);

		ImGui::Text("Last render time: %.2lf ms", state.last_render_time);
	}
	ImGui::End();
}

}

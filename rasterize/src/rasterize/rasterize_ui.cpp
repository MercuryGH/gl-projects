#include <rasterize/rasterize_ui.hpp>

#include <imgui.h>

#include <functional>

#include <scene/scene.hpp>

namespace rasterize {

RasterizeSystemUI::RasterizeSystemUI(RasterizeSystemState& state): state(state) {
}

void RasterizeSystemUI::update(float delta_time) {
	draw_ui();
}

void RasterizeSystemUI::draw_ui() {
	if (ImGui::Begin("Rasterizer")) {

		// select model
		ImGui::Text("Model:");
		const char* instances[] = {
			"Cube",
			"Sphere",
			"Bunny",
			"Armadillo",
			"Culled Armadillo",
		};
		ImGui::PushItemWidth(-1);
		ImGui::Combo("##", &state.cur_model_id, instances, IM_ARRAYSIZE(instances));
		ImGui::PopItemWidth();

		model_dirty |= ImGui::Button("Load instance", ImVec2(-1, 0));

		ImGui::Text("#Tri: %d", state.n_triangles);

		ImGui::Separator();

		ImGui::Text("Rasterizer algorithm:");
		const char* algorithms[] = {
			"Basic",
			"Basic Hierarchical",
			"Octree Hierarchical",
		};
		ImGui::PushItemWidth(-1);
		ImGui::Combo("## ", &state.cur_algo_id, algorithms, IM_ARRAYSIZE(algorithms));
		ImGui::PopItemWidth();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6, 0.6));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7, 0.7));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8, 0.8));
		draw_dirty |= ImGui::Button("Rasterize", ImVec2(-1, 0));
		ImGui::PopStyleColor(3);

		ImGui::Text("Last rasterize time: %.2lf ms", state.last_rasterize_time);
	}
	ImGui::End();
}

}

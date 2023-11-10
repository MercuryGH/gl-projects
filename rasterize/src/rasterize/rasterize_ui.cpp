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
		ImGui::Text("Select model:");
		const char* instances[] = { 
			"Sphere",
			"Bunny",
			"Armadillo",
		};
		static int instance_cur = 0;
		ImGui::PushItemWidth(-1);
		ImGui::Combo("##", &instance_cur, instances, IM_ARRAYSIZE(instances));
		ImGui::PopItemWidth();

		model_dirty |= ImGui::Button("Load instance", ImVec2(-1, 0));
		state.cur_model_id = instance_cur;

		// select algorithm

		// std::vector<std::function<Solid*()>> instantiate_caller = {
		// 	&create_solid,
		// 	&create_holed_cuboid,
		// 	&create_compact,
		// };

		ImGui::Separator();

		ImGui::Text("Rasterizer algo:");
		const char* algorithms[] = { 
			"Basic Z Buffer",
			"Basic Hierarchical Z Buffer",
			"Octree Hierarchical Z Buffer",
		};
		static int algorithm_cur = 0;

		ImGui::PushItemWidth(-1);
		ImGui::Combo("## ", &algorithm_cur, algorithms, IM_ARRAYSIZE(algorithms));
		ImGui::PopItemWidth();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6, 0.6));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7, 0.7));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8, 0.8));
		draw_dirty |= ImGui::Button("Render", ImVec2(-1, 0));
		ImGui::PopStyleColor(3);
	}
	ImGui::End();
}

}
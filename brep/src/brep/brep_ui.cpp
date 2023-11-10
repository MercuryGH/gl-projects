#include <brep/brep_ui.hpp>
#include <dataop/instances.hpp>

#include <imgui.h>

#include <functional>

namespace brep {

BrepSystemUI::BrepSystemUI(BrepSystemState& state): state(state) {
}

void BrepSystemUI::update(float delta_time) {
	draw_ui();
}

void BrepSystemUI::draw_ui() {
	if (ImGui::Begin("B-Rep System")) {
		ImGui::Text("Test cases:");
		std::vector<std::function<Solid*()>> instantiate_caller = {
			&create_solid,
			&create_holed_cuboid,
			&create_double_holed_cuboid,
			&create_complicated,
		};

		const char* instances[] = { 
			"Cuboid",
			"Holed Cuboid",
			"Double Holed Cuboid",
			"Complicated Solid",
		};
		static int item_current = 0;
		ImGui::PushItemWidth(-1);
		ImGui::Combo("##", &item_current, instances, IM_ARRAYSIZE(instances));
		ImGui::PopItemWidth();

		draw_dirty |= ImGui::Button("Load instance", ImVec2(-1, 0));
		if (draw_dirty) {
			Solid* solid = nullptr;
			if (state.solids.find(item_current) == state.solids.end()) {
				solid = instantiate_caller.at(item_current)();
				state.solids[item_current] = solid;
			} else {
				solid = state.solids.at(item_current);
			}
			assert(solid != nullptr);

			state.cur_solid = solid;
		}

		draw_dirty |= ImGui::Checkbox("wireframe", &state.render_wireframe);

		render_settings_dirty |= ImGui::ColorEdit4("color", &state.render_settings.color.x);
	}
	ImGui::End();
}

}
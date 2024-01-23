#include <nurbs/nurbs_ui.hpp>

#include <imgui.h>

namespace nurbs {

NurbsSystemUI::NurbsSystemUI(NurbsSystemState& state): state(state) {
}

void NurbsSystemUI::update(float delta_time) {
	draw_ui();
}

void NurbsSystemUI::draw_ui() {
	if (ImGui::Begin("Nurbs System")) {
	}
	ImGui::End();
}
}
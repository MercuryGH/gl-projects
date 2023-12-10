#include <pathtrace/pathtrace_ui.hpp>

#include <imgui.h>

namespace pathtrace {

PathtraceSystemUI::PathtraceSystemUI(PathtraceSystemState& state): state(state) {
}

void PathtraceSystemUI::update(float delta_time) {
	draw_ui();
}

void PathtraceSystemUI::draw_ui() {
	if (ImGui::Begin("B-Rep System")) {
	}
	ImGui::End();
}

}
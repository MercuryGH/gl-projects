#include <pathtrace/pathtrace_ui.hpp>

#include <imgui.h>

#include <glh/file_io.hpp>

namespace pathtrace {

PathtraceSystemUI::PathtraceSystemUI(PathtraceSystemState& state): state(state) {
}

void PathtraceSystemUI::update(float delta_time) {
	draw_ui();
}

void PathtraceSystemUI::draw_ui() {
	if (ImGui::Begin("Path Tracer")) {

		// select cached scene
		ImGui::Text("Select a cached scene:");
		const char* instances[] = {
			"Classic Cornell Box",
			"Modified Cornell Box",
		};
		ImGui::PushItemWidth(-1);
		ImGui::Combo("##", &state.cur_scene_id, instances, IM_ARRAYSIZE(instances));
		ImGui::PopItemWidth();

		ImGui::Text("OR, open a scene folder:");

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5, 0.6, 0.6));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.5, 0.7, 0.7));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.5, 0.8, 0.8));
		if (ImGui::Button("Open scene folder", ImVec2(-1, 0))) {
			const char* path = renderer::select_dir();
			if (path != nullptr) {
				state.last_import_scene_path_from_disk = path;
				import_scene_from_disk = true;
			}
		}
		ImGui::PopStyleColor(3);

		scene_dirty |= ImGui::Button("Load scene", ImVec2(-1, 0));

		ImGui::Text("Last render time: %.2lf ms", state.last_import_time);

		ImGui::Text("#Tri: %d", state.n_triangles);

		ImGui::Separator();
		// camera

		ImGui::Separator();
		// path tracing

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

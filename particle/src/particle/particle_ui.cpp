#include <particle/particle_ui.hpp>

#include <imgui.h>

namespace particle {

ParticleSystemUI::ParticleSystemUI(ParticleSystemState& state): state(state) {
}

void ParticleSystemUI::update(float delta_time) {
	draw_ui();
}

void ParticleSystemUI::draw_ui() {
	if (ImGui::Begin("Particle System")) {
		ImGui::Text("num particles: %u", state.num_particles);

		if (executing) {
			executing = !ImGui::Button("pause");
		}
		else {
			executing = ImGui::Button("resume");
		}

		ImGui::Separator();
		ImGui::Text("emit");

		// emit_settings_dirty |= ImGui::DragInt(
		// 	"emit interval", reinterpret_cast<int*>(&state.emit_settings.emit_interval),
		// 	1.0f, 0, 10
		// );
		// emit_settings_dirty |= ImGui::DragInt(
		// 	"compact interval", reinterpret_cast<int*>(&state.emit_settings.compact_interval),
		// 	1.0f, 0, 10
		// );

		emit_settings_dirty |= ImGui::DragIntRange2(
			"num emitted",
			reinterpret_cast<int*>(&state.emit_settings.count_min),
			reinterpret_cast<int*>(&state.emit_settings.count_max),
			1.0f, 0, state.k_max_num_particles
		);

		emit_settings_dirty |= ImGui::DragFloat3(
			"position", &state.emit_settings.position.x, 0.05f, -100.0f, 100.0f
		);
		emit_settings_dirty |= ImGui::DragFloat(
			"radius", &state.emit_settings.position_radius, 0.05f, 0.0f, 100.0f
		);
		emit_settings_dirty |= ImGui::DragFloat3(
			"velocity", &state.emit_settings.velocity.x, 0.05f, -100.0f, 100.0f
		);
		emit_settings_dirty |= ImGui::DragFloat(
			"angle", &state.emit_settings.velocity_angle, 1.0f, 0.0f, 180.0f
		);

		emit_settings_dirty |= ImGui::DragFloatRange2(
			"life", &state.emit_settings.life_min, &state.emit_settings.life_max,
			0.1f, 0.01f, 1000.0f
		);
		emit_settings_dirty |= ImGui::DragFloatRange2(
			"mass", &state.emit_settings.mass_min, &state.emit_settings.mass_max,
			0.01f, 0.01f, 100.0f
		);
		emit_settings_dirty |= ImGui::DragFloatRange2(
			"size", &state.emit_settings.size_min, &state.emit_settings.size_max,
			0.01f, 0.01f, 100.0f
		);

		ImGui::Separator();
		ImGui::Text("update");

		ImGui::DragFloat3("force", &state.update_settings.force.x, 0.01f, -100.0f, 100.0f);
		ImGui::DragFloat("gravity", &state.update_settings.gravity, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("drag", &state.update_settings.drag, 0.01f, 0.0f, 100.0f);

		ImGui::Separator();
		ImGui::Text("render");

		render_settings_dirty |= ImGui::ColorEdit4("color", &state.render_settings.color.x);
	}
	ImGui::End();
}
}
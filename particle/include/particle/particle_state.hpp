#pragma once

#include <glm/glm.hpp>

namespace particle {

class ParticleSystemState {
    friend class ParticleSystemUI;
    friend class ParticleSystem;

private:
    constexpr static uint32_t k_scan_width = 512;
    constexpr static uint32_t k_max_num_particles = k_scan_width * k_scan_width;

	uint32_t num_particles = 0;

	struct {
		uint32_t emit_interval = 1;
		uint32_t compact_interval = 1;
		uint32_t count_min = 1;
		uint32_t count_max = 1;
		glm::vec3 position = glm::vec3(0.0f);
		float position_radius = 0.0f;
		glm::vec3 velocity = glm::vec3(0.0f, 1.0f, 0.0f);
		float velocity_angle = 180.0f;
		float life_min = 1.0f;
		float life_max = 1.0f;
		float mass_min = 1.0f;
		float mass_max = 1.0f;
		float size_min = 0.05f;
		float size_max = 0.05f;
	} emit_settings;
	struct {
		glm::vec3 force = glm::vec3(0.0f);
		float gravity = 9.8f;
		float drag = 0.0f;
	} update_settings;
	struct {
		glm::vec4 color = glm::vec4((float)0x66 / 0xff, (float)0xcc / 0xff, (float)0xff / 0xff, 1.0f);
	} render_settings;
};

}
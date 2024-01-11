#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <camera/orbit_camera.hpp>
#include <camera/first_person_camera.hpp>

#include <util/math.hpp>
#include <window/window.hpp>

#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer {

OrbitCamera::OrbitCamera(glm::vec3 lookat, float radius, float aspect, float fov_deg) {
	this->theta = 0.0f,
	this->phi = k_pi * 0.5f,
	this->radius = radius,

	data = {
		.pos = {
			radius * std::sin(phi) * std::cos(theta),
			radius * std::cos(phi),
			radius * std::sin(phi) * std::sin(theta)
		},
		.lookat = lookat,
		.up = { 0.0f, 1.0f, 0.0f },
		.aspect = aspect,
		.fov_deg = fov_deg,

		.proj = glm::perspective(glm::radians(fov_deg), aspect, k_z_near, k_z_far),
		.proj_inv = glm::inverse(data.proj),

		.view = glm::lookAt(data.pos, data.lookat, data.up),
		.view_inv = glm::inverse(data.view)
	};

	update();

	camera_buffer = std::make_unique<GlBuffer>(sizeof(ShaderCamera), GL_MAP_WRITE_BIT);
}

OrbitCamera::OrbitCamera(const FirstPersonCamera& first_person_camera): OrbitCamera(first_person_camera.lookat(), 3.0f, first_person_camera.aspect(), first_person_camera.fov_deg()) {}

void OrbitCamera::rotate(float delta_x, float delta_y) {
	theta -= delta_x;
	if (theta < 0.0) {
		theta += 2.0f * k_pi;
	}
	else if (theta >= 2.0f * k_pi) {
		theta -= 2.0f * k_pi;
	}
	phi = std::clamp(phi + delta_y, 0.1f, k_pi - 0.1f);
	update();
}

void OrbitCamera::forward(float delta) {
	radius = std::max(radius + delta, 0.1f);
	update();
}

void OrbitCamera::set_angle(float theta_deg, float phi_deg) {
	theta = glm::radians(theta_deg);
	phi = glm::radians(phi_deg);
	update();
}

void OrbitCamera::update() {
	data.pos.x = radius * std::sin(phi) * std::cos(theta);
	data.pos.y = radius * std::cos(phi);
	data.pos.z = radius * std::sin(phi) * std::sin(theta);

	data.view = glm::lookAt(data.pos, data.lookat, data.up);
	data.view_inv = glm::inverse(data.view);
	camera_buffer_dirty = true;
}

void OrbitCamera::window_mouse_callback(uint32_t state, float x, float y, float last_x, float last_y) {
	// LMC
	if ((state & Window::eMouseLeft) != 0) {
		float dx = glm::radians(0.25f * (x - last_x));
		float dy = glm::radians(0.25f * (y - last_y));
		rotate(-dx, -dy);
	}
}

void OrbitCamera::window_scroll_callback(float xoffset, float yoffset) {
	forward(-0.5f * yoffset);
}

}

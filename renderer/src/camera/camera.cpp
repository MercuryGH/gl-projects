#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <camera/camera.hpp>
#include <util/math.hpp>

#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer {

namespace {
	struct ShaderCamera {
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 view_inv;
	};

	constexpr float k_z_near = 0.001f;
	constexpr float k_z_far = 100000.0f;
}

OrbitCamera::OrbitCamera(const glm::vec3& look_at, float radius, float aspect, float fov_deg) {
	data = {
		.look_at = look_at,
		.aspect = aspect,
		.fov_deg = fov_deg,
		.theta = 0.0f,
		.phi = k_pi * 0.5f,
		.radius = radius,
		.proj = glm::perspective(glm::radians(fov_deg), aspect, k_z_near, k_z_far),
		.proj_inv = glm::inverse(data.proj),
	};

	update();

	camera_buffer = std::make_unique<GlBuffer>(sizeof(ShaderCamera), GL_MAP_WRITE_BIT);
}

void OrbitCamera::rotate(float delta_x, float delta_y) {
	data.theta -= delta_x;
	if (data.theta < 0.0) {
		data.theta += 2.0f * k_pi;
	}
	else if (data.theta >= 2.0f * k_pi) {
		data.theta -= 2.0f * k_pi;
	}
	data.phi = std::clamp(data.phi + delta_y, 0.1f, k_pi - 0.1f);
	update();
}

void OrbitCamera::forward(float delta) {
	data.radius = std::max(data.radius + delta, 0.1f);
	update();
}

void OrbitCamera::set_aspect(float aspect) {
	data.aspect = aspect;
	data.proj = glm::perspective(glm::radians(data.fov_deg), aspect, k_z_near, k_z_far);
	data.proj_inv = glm::inverse(data.proj);
	update();
}

void OrbitCamera::set_fov(float fov_deg) {
	data.fov_deg = fov_deg;
	data.proj = glm::perspective(glm::radians(fov_deg), data.aspect, k_z_near, k_z_far);
	data.proj_inv = glm::inverse(data.proj);
	update();
}

void OrbitCamera::set_angle(float theta_deg, float phi_deg) {
	data.theta = glm::radians(theta_deg);
	data.phi = glm::radians(phi_deg);
	update();
}

void OrbitCamera::update() {
	data.pos.x = data.radius * std::sin(data.phi) * std::cos(data.theta);
	data.pos.y = data.radius * std::cos(data.phi);
	data.pos.z = data.radius * std::sin(data.phi) * std::sin(data.theta);
	data.view = glm::lookAt(data.pos, data.look_at, glm::vec3(0.0f, 1.0f, 0.0f));
	data.view_inv = glm::inverse(data.view);
	camera_buffer_dirty = true;
}

const GlBuffer* OrbitCamera::get_buffer() {
	// modify uniform camera_buffer (the mapped way)
	if (camera_buffer_dirty) {
		auto shader_data = camera_buffer->typed_map<ShaderCamera>(true);
		shader_data->view = data.view;
		shader_data->view_inv = data.view_inv;
		shader_data->proj = data.proj;
		camera_buffer->unmap();
		camera_buffer_dirty = false;
	}
	return camera_buffer.get();
}

}

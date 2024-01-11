#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <camera/camera.hpp>

#include <util/math.hpp>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer {

Camera::Camera(glm::vec3 pos, glm::vec3 lookat, glm::vec3 up, float aspect, float fov_deg) {
	data = {
        .pos = pos,
		.lookat = lookat,
        .up = up,
		.aspect = aspect,
		.fov_deg = fov_deg,

    	.proj = glm::perspective(glm::radians(fov_deg), aspect, k_z_near, k_z_far),
		.proj_inv = glm::inverse(data.proj),

        .view = glm::lookAt(pos, lookat, up),
        .view_inv = glm::inverse(data.view),
	};

	update();

	camera_buffer = std::make_unique<GlBuffer>(sizeof(ShaderCamera), GL_MAP_WRITE_BIT);
}

void Camera::set_aspect(float aspect) {
	data.aspect = aspect;
	data.proj = glm::perspective(glm::radians(data.fov_deg), aspect, k_z_near, k_z_far);
	data.proj_inv = glm::inverse(data.proj);
}

void Camera::set_fov(float fov_deg) {
	data.fov_deg = fov_deg;
	data.proj = glm::perspective(glm::radians(fov_deg), data.aspect, k_z_near, k_z_far);
	data.proj_inv = glm::inverse(data.proj);
}

const GlBuffer* Camera::get_buffer() {
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

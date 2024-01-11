#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <glh/resource.hpp>

namespace renderer {

struct CameraData {
	glm::vec3 pos;
	glm::vec3 lookat;
	glm::vec3 up;
	float aspect;
	float fov_deg;

	// cached
	glm::mat4 proj;
	glm::mat4 proj_inv;
	glm::mat4 view;
	glm::mat4 view_inv;
};

struct ShaderCamera {
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 view_inv;
};

class Camera {
public:
	Camera() {}
	Camera(glm::vec3 pos, glm::vec3 lookat, glm::vec3 up, float aspect, float fov_deg=45.0f);

	glm::vec3 position() const { return data.pos; }
	glm::vec3 lookat() const { return data.lookat; }
	float aspect() const { return data.aspect; }
	float fov_deg() const { return data.fov_deg; }
	glm::mat4 proj_mat() const { return data.proj; }
	glm::mat4 proj_inv_mat() const { return data.proj_inv; }
	glm::mat4 view_mat() const { return data.view; }
	glm::mat4 view_inv_mat() const { return data.view_inv; }
	CameraData& camera_data() { return data; }

	void set_aspect(float aspect);
	void set_fov(float fov_deg);

	const GlBuffer* get_buffer();

	constexpr static float k_z_near = 0.001f;
	constexpr static float k_z_far = 100000.0f;

	/* handle user input */
	virtual void window_mouse_callback(uint32_t state, float x, float y, float last_x, float last_y) {}
	virtual void window_scroll_callback(float xoffset, float yoffset) {}

protected:
	virtual void update() {}

	CameraData data;

	std::unique_ptr<GlBuffer> camera_buffer;
	bool camera_buffer_dirty{ true };
};

}

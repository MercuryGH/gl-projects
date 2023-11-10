#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <glh/resource.hpp>

namespace renderer {

struct CameraData {
	glm::vec3 pos;
	glm::vec3 look_at;
	float aspect;
	float fov_deg;
	float theta;
	float phi;
	float radius;
	glm::mat4 proj;
	glm::mat4 proj_inv;
	glm::mat4 view;
	glm::mat4 view_inv;
};

class OrbitCamera {
public:
	OrbitCamera(const glm::vec3& look_at, float radius, float aspect, float fov_deg=45.0f);

	glm::vec3 position() const { return data.pos; }
	glm::mat4 proj_mat() const { return data.proj; }
	glm::mat4 proj_inv_mat() const { return data.proj_inv; }
	glm::mat4 view_mat() const { return data.view; }
	glm::mat4 view_inv_mat() const { return data.view_inv; }
	CameraData& camera_data() { return data; }

	void rotate(float delta_x, float delta_y);
	void forward(float delta);

	void set_aspect(float aspect);
	void set_fov(float fov_deg);

	const GlBuffer* get_buffer();

private:
	void update();

	CameraData data;

	std::unique_ptr<GlBuffer> camera_buffer;
	bool camera_buffer_dirty{ true };
};

}
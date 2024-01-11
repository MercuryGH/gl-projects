#pragma once

#include <memory>

#include <camera/camera.hpp>
#include <glm/glm.hpp>

#include <glh/resource.hpp>

namespace renderer {

class FirstPersonCamera;

class OrbitCamera: public Camera {
public:
	OrbitCamera(glm::vec3 lookat, float radius, float aspect, float fov_deg=60.0f);
	OrbitCamera(const FirstPersonCamera& first_person_camera);

	void rotate(float delta_x, float delta_y);
	void forward(float delta);

	void set_angle(float theta_deg, float phi_deg);

	void window_mouse_callback(uint32_t state, float x, float y, float last_x, float last_y) override;
	void window_scroll_callback(float xoffset, float yoffset) override;

private:
	void update() override;

	float theta;
	float phi;
	float radius;
};

}

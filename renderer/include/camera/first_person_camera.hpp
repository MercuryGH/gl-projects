#pragma once

#include <memory>

#include <camera/camera.hpp>

#include <glm/glm.hpp>

#include <glh/resource.hpp>

namespace renderer {

enum CameraMoveDirection {
	eForward,
	eBackward,
	eLeft,
	eRight,
	eUp,
	eDown
};

class OrbitCamera;

class FirstPersonCamera: public Camera {
public:
	FirstPersonCamera(glm::vec3 pos, glm::vec3 front, float aspect, float fov_deg=60.0f);
	FirstPersonCamera(const OrbitCamera& orbit_camera);

	void enable_sprint(bool flag) { sprint = flag; }

	float get_move_speed() const { return move_speed; }
	void set_move_speed(float move_speed) { this->move_speed = move_speed; }

	// keyboard movement
	void move(CameraMoveDirection dir, float delta_t);

	// mouse movement
	void rotate(float delta_x, float delta_y, bool constrain_pitch=true);

	// mouse scroll (zoom), change fov
	void zoom(float delta);

	// change move speed 
	void change_speed(float delta);

	void window_mouse_callback(uint32_t state, float x, float y, float last_x, float last_y) override;
	void window_scroll_callback(float xoffset, float yoffset) override;

private:
	void update() override;

	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 world_up; // constant in fact
	float yaw_deg;
	float pitch_deg;

	float move_speed;
	bool sprint{ false };
};

}

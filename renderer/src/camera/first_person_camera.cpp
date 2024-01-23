#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <camera/first_person_camera.hpp>
#include <camera/orbit_camera.hpp>

#include <util/math.hpp>

#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <window/window.hpp>

namespace renderer {

namespace {
    constexpr float k_mouse_sensitivity = 0.3f;
}

FirstPersonCamera::FirstPersonCamera(glm::vec3 pos, glm::vec3 front, float aspect, float fov_deg) {
    this->front = glm::normalize(front);
    this->world_up = glm::vec3{ 0.0f, 1.0f, 0.0f };
    this->right = glm::normalize(glm::cross(front, world_up));

    this->pitch_deg = glm::degrees(std::asin(this->front.y));
    this->yaw_deg = glm::degrees(std::atan2(this->front.z, this->front.x));

    this->move_speed = 1.0f;

	data = {
        .pos = pos,
		.lookat = pos + this->front,
        .up = world_up,
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

FirstPersonCamera::FirstPersonCamera(const OrbitCamera& orbit_camera): FirstPersonCamera(orbit_camera.position(), orbit_camera.lookat() - orbit_camera.position(), orbit_camera.aspect(), orbit_camera.fov_deg()) {}

// keyboard movement
void FirstPersonCamera::move(CameraMoveDirection dir, float delta_t) {
    float cur_move_speed = sprint ? move_speed * 2.0f : move_speed;
    float displacement = cur_move_speed * delta_t;
    switch (dir) {
        case CameraMoveDirection::eForward: { data.pos += front * displacement; break; }
        case CameraMoveDirection::eBackward: { data.pos -= front * displacement; break; }
        case CameraMoveDirection::eLeft: { data.pos -= right * displacement; break; }
        case CameraMoveDirection::eRight: { data.pos += right * displacement; break; }
        case CameraMoveDirection::eUp: { data.pos += world_up * displacement; break; }
        case CameraMoveDirection::eDown: { data.pos -= world_up * displacement; break; }
        default: break;
    }
    update();
}

void FirstPersonCamera::tick(const renderer::Window& window, float delta_t) {
    enable_sprint(window.key_pressed(GLFW_KEY_LEFT_SHIFT));

    if (window.key_pressed(GLFW_KEY_W)) {
        move(CameraMoveDirection::eForward, delta_t);
    }
    if (window.key_pressed(GLFW_KEY_S)) {
        move(CameraMoveDirection::eBackward, delta_t);
    }
    if (window.key_pressed(GLFW_KEY_A)) {
        move(CameraMoveDirection::eLeft, delta_t);
    }
    if (window.key_pressed(GLFW_KEY_D)) {
        move(CameraMoveDirection::eRight, delta_t);
    }
    if (window.key_pressed(GLFW_KEY_SPACE)) {
        move(CameraMoveDirection::eUp, delta_t);
    }
    if (window.key_pressed(GLFW_KEY_LEFT_CONTROL)) {
        move(CameraMoveDirection::eDown, delta_t);
    } 
}

// mouse movement
void FirstPersonCamera::rotate(float delta_x, float delta_y, bool constrain_pitch) {
    delta_x *= k_mouse_sensitivity;
    delta_y *= k_mouse_sensitivity;

    yaw_deg += delta_x;
    pitch_deg += delta_y;

    if (constrain_pitch) {
        pitch_deg = util::clamp(pitch_deg, -89.0f, 89.0f);
    }
    update();
}

// mouse scroll (zoom), change fov only
void FirstPersonCamera::zoom(float delta) {
    data.fov_deg -= delta;
    data.fov_deg = util::clamp(data.fov_deg, 10.0f, 90.0f);

    data.proj = glm::perspective(glm::radians(data.fov_deg), data.aspect, k_z_near, k_z_far),
    data.proj_inv = glm::inverse(data.proj),
	camera_buffer_dirty = true;
}

// change move speed 
void FirstPersonCamera::change_speed(float delta) {
    if (delta > 0) {
        move_speed *= 1.2f;
    } else {
        move_speed *= 0.8f;
    }
}

void FirstPersonCamera::update() {
    front = glm::normalize(glm::vec3{
        std::cos(glm::radians(yaw_deg)) * std::cos(glm::radians(pitch_deg)),
        std::sin(glm::radians(pitch_deg)),
        std::sin(glm::radians(yaw_deg)) * std::cos(glm::radians(pitch_deg))
    });

    right = glm::normalize(glm::cross(front, world_up));
    data.up = glm::normalize(glm::cross(right, front));
    data.lookat = data.pos + front;

	data.view = glm::lookAt(data.pos, data.lookat, data.up);
	data.view_inv = glm::inverse(data.view);
	camera_buffer_dirty = true;
}

void FirstPersonCamera::window_mouse_callback(uint32_t state, float x, float y, float last_x, float last_y) {
    float delta_x = x - last_x;
    float delta_y = y - last_y;

    rotate(delta_x, -delta_y);
}

void FirstPersonCamera::window_scroll_callback(float xoffset, float yoffset) {
    change_speed(yoffset);
}

}

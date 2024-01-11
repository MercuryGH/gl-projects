#pragma once

#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <window/screen_capturer.hpp>
#include <unordered_set>

// yay -S nvidia nvidia-utils lib32-nvidia-utils

namespace renderer {

class Window {
	friend class ScreenCapturer;
public:
	Window(uint32_t width, uint32_t height, const char* title);
	~Window();

	float get_aspect() const { return static_cast<float>(width) / height; }

	void main_loop(const std::function<void()>& func);

	void process_input();

	using MouseCallback = std::function<void(uint32_t, float, float, float, float)>;
	enum Mouse {
		eMouseLeft = 1,
		eMouseRight = 2,
		eMouseMiddle = 4,
	};
	void set_mouse_callback(const MouseCallback& callback);

	using ScrollCallback = std::function<void(float, float)>;
	void set_scroll_callback(const ScrollCallback& callback);

	using KeyCallback = std::function<void(int, int)>;
	void set_key_callback(const KeyCallback& callback);

	using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
	void set_resize_callback(const ResizeCallback& callback);

	void set_title(const char* title);

	ScreenCapturer& get_screen_capturer() { return screen_capturer; }

	bool key_pressed(int key) const { return pressed_key.find(key) != pressed_key.end(); }

private:
	friend void glfw_cursor_pos_callback(GLFWwindow* window, double x, double y);
	friend void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void glfw_resize_callback(GLFWwindow* window, int width, int height);
	friend void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	uint32_t width;
	uint32_t height;

	GLFWwindow* window = nullptr;

	MouseCallback mouse_callback = [](uint32_t, float, float, float, float) {};
	float last_mouse_x = 0.0f;
	float last_mouse_y = 0.0f;

	ScrollCallback scroll_callback = [](float, float) {};

	KeyCallback key_callback = [](int, int) {};

	ResizeCallback resize_callback = [](uint32_t, uint32_t) {};

	std::unordered_map<float, ImFont*> imgui_cached_fonts;
	ImFont* imgui_curr_font = nullptr;
	float imgui_last_scale = 1.0f;

	std::unordered_set<int> pressed_key; // all pressed key in 1 frame

	ScreenCapturer screen_capturer;
};

}

#include <window/window.hpp>

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace renderer {

namespace {
	void glfw_error_callback(int error, const char* desc) {
		std::cerr << "[!GLFW ERROR] " << desc << std::endl;
	}

	void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param) {
		if (severity != GL_DEBUG_SEVERITY_HIGH) {
			return;
		}
		auto type_str = type == GL_DEBUG_TYPE_ERROR ? "[ERROR]"
			: type == GL_DEBUG_TYPE_PERFORMANCE ? "[PERFORMANCE]"
			: type == GL_DEBUG_TYPE_MARKER ? "[MARKER]"
			: type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR ? "[DEPRECATED]"
			: type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR ? "[UNDEFINED BEHAVIOR]" : "[OTHER]";
		auto severity_str = severity == GL_DEBUG_SEVERITY_HIGH ? "[HIGH]"
			: severity == GL_DEBUG_SEVERITY_MEDIUM ? "[MEDIUM]"
			: severity == GL_DEBUG_SEVERITY_LOW ? "[LOW]" : "[NOTIFICATION]";
		std::cerr << "OpenGL callback " << type_str << " " << severity_str << " " << message << std::endl;
		if (type == GL_DEBUG_TYPE_ERROR && severity == GL_DEBUG_SEVERITY_HIGH) {
			// exit(-1);
		}
	}

	constexpr float k_imgui_font_size = 14.0f;
}

Window* get_glfw_user_window(GLFWwindow* glfw_window) {
	return reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
}

void glfw_cursor_pos_callback(GLFWwindow* glfw_window, double x, double y) {
	auto window = get_glfw_user_window(glfw_window);
	uint32_t state = 0;
	if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		state |= Window::eMouseLeft;
	}
	if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		state |= Window::eMouseRight;
	}
	if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		state |= Window::eMouseMiddle;
	}
	window->mouse_callback(state, x, y, window->last_mouse_x, window->last_mouse_y);
	window->last_mouse_x = x;
	window->last_mouse_y = y;
}

void glfw_scroll_callback(GLFWwindow* glfw_window, double xoffset, double yoffset) {
	auto window = get_glfw_user_window(glfw_window);
	window->scroll_callback(xoffset, yoffset);
}

void glfw_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
	auto window = get_glfw_user_window(glfw_window);
	window->key_callback(key, action);
}

void glfw_resize_callback(GLFWwindow* glfw_window, int width, int height) {
	auto window = get_glfw_user_window(glfw_window);
	glViewport(0, 0, width, height);

	float width_scale, height_scale;
	glfwGetWindowContentScale(glfw_window, &width_scale, &height_scale);

	// imgui font dpi update
	auto dpi_scale = std::max(width_scale, height_scale);
	auto rel_scale = dpi_scale / window->imgui_last_scale;
	ImGui::GetStyle().ScaleAllSizes(rel_scale);
	window->imgui_last_scale = dpi_scale;
	if (auto it = window->imgui_cached_fonts.find(dpi_scale); it != window->imgui_cached_fonts.end()) {
		window->imgui_curr_font = it->second;
	}
	else {
		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig font_cfg{};
		font_cfg.SizePixels = k_imgui_font_size * dpi_scale;
		auto font = io.Fonts->AddFontDefault(&font_cfg);
		window->imgui_cached_fonts[dpi_scale] = font; // cache font of current dpi
		window->imgui_curr_font = font;
		io.Fonts->Build();
		ImGui_ImplOpenGL3_DestroyDeviceObjects();
	}

	width /= width_scale;
	height /= height_scale;
	if (window->width != width || window->height != height) {
		window->width = width;
		window->height = height;
		window->resize_callback(width, height);
	}
}

Window::Window(uint32_t width, uint32_t height, const char* title) : width(width), height(height), screen_capturer(*this)
{
	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Failed to create glfw window\n";
		return;
	}
	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, this);
	glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
	glfwSetScrollCallback(window, glfw_scroll_callback);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetFramebufferSizeCallback(window, glfw_resize_callback);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::cerr << "Failed to load OpenGL functions\n";
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_message_callback, nullptr);

	int frame_width, frame_height;
	glfwGetFramebufferSize(window, &frame_width, &frame_height);
	glViewport(0, 0, frame_width, frame_height);

	float width_scale, height_scale;
	glfwGetWindowContentScale(window, &width_scale, &height_scale);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	auto dpi_scale = std::max(width_scale, height_scale);
	ImFontConfig font_cfg{};
	font_cfg.SizePixels = k_imgui_font_size * dpi_scale;
	imgui_curr_font = io.Fonts->AddFontDefault(&font_cfg);
	imgui_cached_fonts[dpi_scale] = imgui_curr_font;

	ImGui::StyleColorsClassic();
	imgui_last_scale = dpi_scale;
	ImGui::GetStyle().ScaleAllSizes(dpi_scale);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
}

Window::~Window() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::main_loop(const std::function<void()>& callback) {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// const int k_miku_green[3] = { 0x39, 0xc5, 0xbb };
		const float k_bg_color[3] = { 0.2f, 0.3f, 0.3f };
		glClearColor(k_bg_color[0], k_bg_color[1], k_bg_color[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::PushFont(imgui_curr_font);
		callback();
		ImGui::PopFont();

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
}

void Window::set_mouse_callback(const MouseCallback& callback) {
	mouse_callback = callback;
}

void Window::set_scroll_callback(const ScrollCallback& callback) {
	scroll_callback = callback;
}

void Window::set_key_callback(const KeyCallback& callback) {
	key_callback = callback;
}

void Window::set_resize_callback(const ResizeCallback& callback) {
	resize_callback = callback;
}

void Window::set_title(const char* title) {
	glfwSetWindowTitle(window, title);
}

}

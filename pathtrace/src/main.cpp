#include <window/window.hpp>
#include <camera/camera.hpp>
#include <pathtrace/pathtrace_system.hpp>

int main(int argc, char** argv) {
	using namespace renderer;
	using namespace pathtrace;

	const uint32_t k_width = 800;
	const uint32_t k_height = 600;
	Window window(k_width, k_height, "pathtracer");

	OrbitCamera camera(Vector3(0.0f), 10.0f, window.get_aspect());
	PathtraceSystem pathtrace_system{};

	pathtrace_system.set_camera_buffer(camera.get_buffer());

	window.set_resize_callback([&](uint32_t width, uint32_t height) {
		camera.set_aspect(window.get_aspect());
	});

	window.set_mouse_callback([&](uint32_t state, float x, float y, float last_x, float last_y) {
		if ((state & Window::eMouseMiddle) != 0) {
			float dx = glm::radians(0.25f * (x - last_x));
			float dy = glm::radians(0.25f * (y - last_y));
			camera.rotate(dx, dy);
		}
		if ((state & Window::eMouseRight) != 0) {
			float dx = 0.005f * (x - last_x);
			float dy = 0.005f * (y - last_y);
			camera.forward(dx - dy);
		}
		pathtrace_system.set_camera_buffer(camera.get_buffer());
	});

	window.main_loop([&]() {
		pathtrace_system.update(ImGui::GetIO().DeltaTime);

		if (ImGui::Begin("Statistics")) {
			float fps = ImGui::GetIO().Framerate;
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
		}
		ImGui::End();
	});

	return 0;
}

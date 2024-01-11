#include <window/window.hpp>
#include <camera/orbit_camera.hpp>

int main(int argc, char** argv) {
	using namespace renderer;
	const uint32_t k_width = 800;
	const uint32_t k_height = 600;
	Window window(k_width, k_height, "gl");

	OrbitCamera camera(glm::vec3(0.0f), 10.0f, window.get_aspect());

	window.set_resize_callback([&](uint32_t width, uint32_t height) {
		camera.set_aspect(window.get_aspect());
	});

	window.set_mouse_callback([&](uint32_t state, float x, float y, float last_x, float last_y) {});

	window.main_loop([&]() {
		if (ImGui::Begin("Statistics")) {
			float fps = ImGui::GetIO().Framerate;
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
		}
		ImGui::End();
	});

	return 0;
}

#include <window/window.hpp>
#include <camera/camera.hpp>
#include <brep/brep_system.hpp>

int main(int argc, char** argv) {
	using namespace renderer;
	using namespace brep;

	const uint32_t k_width = 800;
	const uint32_t k_height = 600;
	Window window(k_width, k_height, "brep");

	OrbitCamera camera(Vector3(0.0f), 10.0f, window.get_aspect());
	BrepSystem brep_system{};

	brep_system.set_camera_buffer(camera.get_buffer());

	window.set_resize_callback([&](uint32_t width, uint32_t height) {
		camera.set_aspect(window.get_aspect());
	});

	window.set_mouse_callback([&](uint32_t state, float x, float y, float last_x, float last_y) {
		// LMC
		if ((state & Window::eMouseLeft) != 0) {
			float dx = glm::radians(0.25f * (x - last_x));
			float dy = glm::radians(0.25f * (y - last_y));
			camera.rotate(-dx, -dy);
		}
		brep_system.set_camera_buffer(camera.get_buffer());
	});

	window.set_scroll_callback([&](float xoffset, float yoffset) {
		// mouse middel scroll
		camera.forward(-0.5f * yoffset);
		brep_system.set_camera_buffer(camera.get_buffer());
	});

	window.main_loop([&]() {
		brep_system.update(ImGui::GetIO().DeltaTime);
		window.get_screen_capturer().update(); // update screen capturer after rendering, before ImGui calling

		if (ImGui::Begin("Statistics")) {
			float fps = ImGui::GetIO().Framerate;
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);

			if (ImGui::Button("Capture current frame", ImVec2(-1, 0))) {
				std::string capture_path = "./";
				if (capture_path.empty() == false)
				{
					window.get_screen_capturer().capture_current_state(capture_path);
				}
			}
		}
		
		ImGui::End();
	});

	return 0;
}

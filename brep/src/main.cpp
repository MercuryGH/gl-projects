#include <window/window.hpp>
#include <camera/orbit_camera.hpp>
#include <camera/first_person_camera.hpp>
#include <brep/brep_system.hpp>

int main(int argc, char** argv) {
	using namespace renderer;
	using namespace brep;

	const uint32_t k_width = 800;
	const uint32_t k_height = 600;
	Window window(k_width, k_height, "brep");

	std::unique_ptr<OrbitCamera> oc = std::make_unique<OrbitCamera>(Vector3(0.0f), 10.0f, window.get_aspect());
	std::unique_ptr<FirstPersonCamera> fpc = nullptr;
	Camera* camera = oc.get();

	BrepSystem brep_system{};
	brep_system.set_camera_buffer(camera->get_buffer());

	window.set_resize_callback([&](uint32_t width, uint32_t height) {
		camera->set_aspect(window.get_aspect());
	});

	window.set_mouse_callback([&](uint32_t state, float x, float y, float last_x, float last_y) {
		camera->window_mouse_callback(state, x, y, last_x, last_y);
		brep_system.set_camera_buffer(camera->get_buffer());
	});

	window.set_scroll_callback([&](float xoffset, float yoffset) {
		camera->window_scroll_callback(xoffset, yoffset);
		brep_system.set_camera_buffer(camera->get_buffer());
	});

	window.main_loop([&]() {
		brep_system.update(ImGui::GetIO().DeltaTime);
		window.get_screen_capturer().update(); // update screen capturer after rendering, before ImGui calling

		float fps = ImGui::GetIO().Framerate;
		float delta_t = 1.0f / fps;

		if (ImGui::Begin("Statistics")) {
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f * delta_t, fps);

			static bool use_fpc = false;
			ImGui::Checkbox("first person camera", &use_fpc);
			if (use_fpc) {
				if (fpc == nullptr) {
					fpc = std::make_unique<FirstPersonCamera>(*oc);
				}
				camera = fpc.get();
			} else {
				fpc = nullptr;
				camera = oc.get();
			}

			// tick FPCamera
			if (use_fpc) {
				fpc->tick(window, delta_t);
			}
			brep_system.set_camera_buffer(camera->get_buffer());

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

#include <window/window.hpp>
#include <camera/camera.hpp>
#include <pathtrace/pathtrace_system.hpp>

int main(int argc, char** argv) {
	using namespace renderer;
	using namespace pathtrace;

	const uint32_t k_width = 800;
	const uint32_t k_height = 800;
	Window window(k_width, k_height, "path tracer");

	OrbitCamera camera(Vector3(0.0f), 10.0f, window.get_aspect());

	PathtraceSystem pathtrace_system(camera.camera_data(), k_width, k_height);

	window.set_resize_callback([&](uint32_t width, uint32_t height) {
		camera.set_aspect(window.get_aspect());
	});

	window.set_mouse_callback([&](uint32_t state, float x, float y, float last_x, float last_y) {
		// LMC (invalidate in this app)
		// if ((state & Window::eMouseLeft) != 0) {
		// 	float dx = glm::radians(0.25f * (x - last_x));
		// 	float dy = glm::radians(0.25f * (y - last_y));
		// 	camera.rotate(-dx, -dy);
		// }
	});

	window.set_scroll_callback([&](float xoffset, float yoffset) {
		// mouse middel scroll
		camera.forward(-0.5f * yoffset);
	});

	window.main_loop([&]() {
		pathtrace_system.update(ImGui::GetIO().DeltaTime);
		window.get_screen_capturer().update(); // update screen capturer after rendering, before ImGui calling

		if (ImGui::Begin("Statistics & Camera")) {
			float fps = ImGui::GetIO().Framerate;
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);

			if (ImGui::Button("Capture current frame", ImVec2(-1, 0))) {
				std::string capture_path = "./";
				if (capture_path.empty() == false)
				{
					window.get_screen_capturer().capture_current_state(capture_path);
				}
			}

			ImGui::Separator();
			ImGui::Text("radius: %.3f", camera.camera_data().radius);

			// Camera data setter (for test only)
			static float fov = 45.0f;
			ImGui::SliderFloat("FOV", &fov, 1.0f, 180.0f);
			camera.set_fov(fov);

			static float theta = 286.0f;
			static float phi = 55.0f;
			ImGui::SliderFloat("theta", &theta, 0.0f, 360.0f);
			ImGui::SliderFloat("phi", &phi, 1.0f, 179.0f);
			camera.set_angle(theta, phi);
		}
		ImGui::End();
	});

	return 0;
}

#include <window/window.hpp>
#include <camera/camera.hpp>
#include <rasterize/rasterize_system.hpp>

int main(int argc, char** argv) {
	using namespace renderer;
	using namespace rasterize;

	const uint32_t k_width = 800;
	const uint32_t k_height = 600;
	Window window(k_width, k_height, "rasterizer");

	OrbitCamera camera(Vector3(0.0f), 10.0f, window.get_aspect());

	RasterizeSystem rasterize_system(camera.camera_data(), k_width, k_height);

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
	});

	window.set_scroll_callback([&](float xoffset, float yoffset) {
		// mouse middel scroll
		camera.forward(-0.5f * yoffset);
	});

	window.main_loop([&]() {
		rasterize_system.update(ImGui::GetIO().DeltaTime);

		if (ImGui::Begin("Statistics")) {
			float fps = ImGui::GetIO().Framerate;
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);

			ImGui::Separator();

			// Camera data setter (for test only)
			static float fov = 45.0f;
			ImGui::SliderFloat("FOV", &fov, 0.0f, 180.0f);
			camera.set_fov(fov);

			ImGui::Text("radius: %.3f", camera.camera_data().radius);
			ImGui::Text("theta: %.1f deg", glm::degrees(camera.camera_data().theta));
			ImGui::Text("phi: %.1f deg", glm::degrees(camera.camera_data().phi));
		}
		ImGui::End();
	});

	return 0;
}

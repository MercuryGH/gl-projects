#include <window/window.hpp>
#include <pathtrace/pathtrace_system.hpp>

int main(int argc, char** argv) {
	using namespace renderer;
	using namespace pathtrace;

	const uint32_t k_width = 800;
	const uint32_t k_height = 800;
	Window window(k_width, k_height, "path tracer");

	PathtraceSystem pathtrace_system(window);

	window.set_resize_callback([&](uint32_t width, uint32_t height) {
	});

	window.set_mouse_callback([&](uint32_t state, float x, float y, float last_x, float last_y) {
	});

	window.set_scroll_callback([&](float xoffset, float yoffset) {
	});

	window.main_loop([&]() {
		pathtrace_system.update(ImGui::GetIO().DeltaTime);
		window.get_screen_capturer().update(); // update screen capturer after rendering, before ImGui calling

		if (ImGui::Begin("Statistics")) {
			float fps = ImGui::GetIO().Framerate;
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);

			ImGui::Separator();

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

#pragma once

#include <memory>

#include <util/types.hpp>
#include <util/cpu_timer.hpp>

#include <window/window.hpp>
#include <glh/gl_buffer.hpp>
#include <glh/program.hpp>
#include <camera/camera.hpp>

#include <pathtrace/pathtrace_ui.hpp>
#include <pathtrace/pathtrace_state.hpp>

#include <scene/scene.hpp>

namespace pathtrace {
	using namespace renderer;

class PathtraceSystem {
public:
	PathtraceSystem(Window& window);
	~PathtraceSystem();

	void update(float delta_time);

private:
	void display(const GlTexture2D& display_texture);

	Window& window;
	Scene scene;

	std::unique_ptr<GlGraphicsProgram> display_program;
	std::unique_ptr<GlBuffer> draw_params_buffer;
	uint32_t empty_vao{ 0 };

	std::unique_ptr<PathtraceSystemUI> ui;
	PathtraceSystemState state;

	CpuTimer timer;
};

}


#pragma once

#include <memory>

#include <util/types.hpp>
#include <util/cpu_timer.hpp>

#include <glh/resource.hpp>
#include <glh/program.hpp>
#include <camera/camera.hpp>

#include <pathtrace/pathtrace_ui.hpp>
#include <pathtrace/pathtrace_state.hpp>

#include <scene/scene.hpp>

namespace pathtrace {
	using namespace renderer;

class PathtraceSystem {
public:
	PathtraceSystem(const CameraData& camera_data, uint32_t width, uint32_t height);
	~PathtraceSystem();

	void update(float delta_time);

private:
	void display(const GlTexture2D& display_texture);

	Scene scene;
	const CameraData& camera_data;

	std::unique_ptr<GlGraphicsProgram> display_program;
	uint32_t empty_vao{ 0 };

	std::unique_ptr<PathtraceSystemUI> ui;
	PathtraceSystemState state;

	CpuTimer timer;
};

}


#pragma once

#include <memory>

#include <util/types.hpp>
#include <util/cpu_timer.hpp>

#include <glh/resource.hpp>
#include <glh/program.hpp>
#include <camera/camera.hpp>

#include <rasterize/rasterize_ui.hpp>
#include <rasterize/rasterize_state.hpp>

#include <scene/scene.hpp>

namespace rasterize {
	using namespace renderer;

class RasterizeSystem {
public:
	RasterizeSystem(const CameraData& camera_data, uint32_t width, uint32_t height);
	~RasterizeSystem();

	void update(float delta_time);

private:
	void display(const GlTexture2D& display_texture);

	Scene scene;
	const CameraData& camera_data;

	std::unique_ptr<GlGraphicsProgram> display_program;
	uint32_t empty_vao{ 0 };

	std::unique_ptr<RasterizeSystemUI> ui;
	RasterizeSystemState state;

	CpuTimer timer;
};

}


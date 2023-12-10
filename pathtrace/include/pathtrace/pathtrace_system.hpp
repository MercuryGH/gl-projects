#pragma once

#include <memory>

#include <glh/resource.hpp>
#include <glh/program.hpp>

#include <pathtrace/pathtrace_ui.hpp>
#include <pathtrace/pathtrace_state.hpp>

#include <draw/draw.hpp>

namespace pathtrace {
	using namespace renderer;

class PathtraceSystem {
public:
	PathtraceSystem();
	~PathtraceSystem();

	void set_camera_buffer(const GlBuffer* camera_buffer) { this->camera_buffer = camera_buffer; }

	void update(float delta_time);

private:
	const GlBuffer* camera_buffer{ nullptr };

	std::unique_ptr<PathtraceSystemUI> ui;
	PathtraceSystemState state;
};

}


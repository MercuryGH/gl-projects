#pragma once

#include <memory>

#include <util/types.hpp>

#include <glh/gl_buffer.hpp>
#include <glh/gl_texture2d.hpp>
#include <glh/program.hpp>

#include <nurbs/nurbs_ui.hpp>
#include <nurbs/nurbs_state.hpp>

namespace nurbs {
	using namespace renderer;

class NurbsSystem {
public:
	NurbsSystem();
	~NurbsSystem();

	void set_camera_buffer(const GlBuffer* camera_buffer) { this->camera_buffer = camera_buffer; }

	void update(float delta_time);

private:
	const GlBuffer* camera_buffer{ nullptr };

};

}

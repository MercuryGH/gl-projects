#pragma once

#include <memory>

#include <util/types.hpp>

#include <glh/gl_buffer.hpp>
#include <glh/gl_texture2d.hpp>
#include <glh/program.hpp>

#include <nurbs/nurbs_ui.hpp>
#include <nurbs/nurbs_state.hpp>
#include <draw/scene.hpp>

namespace nurbs {
	using namespace renderer;

class NurbsSystem {
public:
	NurbsSystem();
	~NurbsSystem();

	void set_camera_buffer(const GlBuffer* camera_buffer) { this->camera_buffer = camera_buffer; }

	void update(float delta_time);

private:
	void draw();

	const GlBuffer* camera_buffer{ nullptr };

	std::unique_ptr<NurbsSystemUI> ui;
	NurbsSystemState state;

	// uniform buffers
	std::unique_ptr<GlBuffer> draw_surface_params_buffer;
	std::unique_ptr<GlBuffer> draw_red_line_params_buffer;
	std::unique_ptr<GlBuffer> draw_green_line_params_buffer;
	std::unique_ptr<GlBuffer> draw_black_line_params_buffer;
	std::unique_ptr<GlBuffer> draw_curve_params_buffer;

	// shader programs
	std::unique_ptr<GlGraphicsProgram> draw_surface_program;
	std::unique_ptr<GlGraphicsProgram> draw_line_program;

	// Rendering data
	Scene scene;
};

}

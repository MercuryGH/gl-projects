#pragma once

#include <memory>

#include <glh/resource.hpp>
#include <glh/program.hpp>

#include <brep/brep_ui.hpp>
#include <brep/brep_state.hpp>

#include <draw/draw.hpp>

namespace brep {
	using namespace renderer;

class BrepSystem {
public:
	BrepSystem();
	~BrepSystem();

	void set_camera_buffer(const GlBuffer* camera_buffer) { this->camera_buffer = camera_buffer; }

	void update(float delta_time);
	void do_draw();

private:
	const GlBuffer* camera_buffer{ nullptr };

	std::unique_ptr<BrepSystemUI> ui;
	BrepSystemState state;

	std::unique_ptr<GlBuffer> draw_params_buffer;
	std::unique_ptr<GlGraphicsProgram> draw_program;

	uint32_t tri_vao{ 0 };
	uint32_t wireframe_vao{ 0 };
	std::unique_ptr<GlBuffer> tri_vbo;
	std::unique_ptr<GlBuffer> wireframe_vbo;
};

}


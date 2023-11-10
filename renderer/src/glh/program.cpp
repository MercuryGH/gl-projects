#include <glh/program.hpp>
#include <string>
#include <iostream>
#include <cassert>

#include <glad/glad.h>

namespace renderer {

GlShader::GlShader(const char* source, uint32_t type) {
	shader = glCreateShader(type);
	const char* p_code = source;
	glShaderSource(shader, 1, &p_code, nullptr);
	glCompileShader(shader);

	int ret;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char log_info[512];
		glGetShaderInfoLog(shader, 512, nullptr, log_info);
		std::string sh_str;
		if (type == GL_VERTEX_SHADER) {
			sh_str = "vertex";
		}
		else if (type == GL_FRAGMENT_SHADER) {
			sh_str = "fragment";
		}
		else if (type == GL_COMPUTE_SHADER) {
			sh_str = "compute";
		}
		std::cerr << "Compilation Error on " << sh_str << " shader: " << log_info << std::endl;
		glDeleteShader(shader);
		shader = 0;
	}
}

GlShader::GlShader(const uint8_t* binary, uint32_t length, uint32_t type) {
	shader = glCreateShader(type);
	glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, binary, length);
	glSpecializeShader(shader, "main", 0, nullptr, nullptr);

	int ret;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char log_info[512];
		glGetShaderInfoLog(shader, 512, nullptr, log_info);
		std::string sh_str;
		if (type == GL_VERTEX_SHADER) {
			sh_str = "vertex";
		}
		else if (type == GL_FRAGMENT_SHADER) {
			sh_str = "fragment";
		}
		else if (type == GL_COMPUTE_SHADER) {
			sh_str = "compute";
		}
		std::cerr << "Compilation Error on " << sh_str << " shader: " << log_info << std::endl;
		glDeleteShader(shader);
		shader = 0;
	}
}

GlShader::~GlShader() {
	if (shader) {
		glDeleteShader(shader);
		shader = 0;
	}
}

GlProgram::~GlProgram() {
	if (program) {
		glDeleteProgram(program);
		program = 0;
	}
}

void GlProgram::use() const {
	glUseProgram(id());
}

GlGraphicsProgram::GlGraphicsProgram(const GlShader& vs, const GlShader& fs) {
	program = glCreateProgram();
	glAttachShader(program, vs.id());
	glAttachShader(program, fs.id());

	glLinkProgram(program);
	int ret = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char log_info[512];
		glGetProgramInfoLog(program, 512, nullptr, log_info);
		std::cerr << "link graphics program error: " << log_info << std::endl;

		glDeleteProgram(program);
		program = 0;
	}

	glDetachShader(program, vs.id());
	glDetachShader(program, fs.id());
}

GlComputeProgram::GlComputeProgram(const GlShader& cs) {
	program = glCreateProgram();
	glAttachShader(program, cs.id());

	glLinkProgram(program);
	int ret = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char log_info[512];
		glGetProgramInfoLog(program, 512, nullptr, log_info);
		std::cerr << "link compute program error: " << log_info << std::endl;

		glDeleteProgram(program);
		program = 0;
	}

	glDetachShader(program, cs.id());
}

}
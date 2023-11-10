#pragma once

#include <numbers>
#include <cstdint>

namespace renderer {

class GlShader {
public:
	GlShader(const char* source, uint32_t type);
	GlShader(const uint8_t* binary, uint32_t length, uint32_t type);
	~GlShader();

	uint32_t id() const { return shader; }

private:
	uint32_t shader = 0;
};

class GlProgram {
public:
	GlProgram() = default;
	~GlProgram();
	uint32_t id() const { return program; }
	void use() const;

protected:
	uint32_t program = 0;
};

class GlGraphicsProgram: public GlProgram {
public:
	GlGraphicsProgram(const GlShader& vs, const GlShader& fs);
};

class GlComputeProgram: public GlProgram {
public:
	GlComputeProgram(const GlShader& cs);
};

}

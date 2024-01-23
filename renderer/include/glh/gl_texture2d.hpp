#pragma once

#include <cstdint>
#include <memory>

#include <glad/glad.h>

namespace renderer {

class GlTexture2D {
public:
	// if level = 0, generate log_2(min(width, height)) mipmaps
	GlTexture2D() {}
	GlTexture2D(uint32_t format, uint32_t width, uint32_t height, uint32_t levels=0);
	~GlTexture2D();

	void init(uint32_t format, uint32_t width, uint32_t height, uint32_t levels=0);

	uint32_t id() const { return gl_texture; }

	uint32_t get_width() const { return width; }
	uint32_t get_height() const { return height; }
	uint32_t get_levels() const { return levels; }
	uint32_t get_format() const { return format; }

	void set_data(const void* data, uint32_t level = 0);

	void generate_mipmap();

	void dump_png_file(uint32_t level = 0);

private:
	uint32_t gl_texture = 0;
	uint32_t width;
	uint32_t height;
	uint32_t levels;
	uint32_t format;
	uint32_t channel_format;
	uint32_t channel_type;
};

}

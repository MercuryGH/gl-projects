#include <glh/gl_texture2d.hpp>

#include <cmath>

#include <glh/img_output.hpp>

namespace renderer {

namespace {
	void get_channel_format_type(uint32_t format, uint32_t& channel_format, uint32_t& channel_type) {
		if (format == GL_RGB8) {
			channel_format = GL_RGB;
			channel_type = GL_UNSIGNED_BYTE;
		}
		else if (format == GL_SRGB8) {
			channel_format = GL_SRGB;
			channel_type = GL_UNSIGNED_BYTE;
		}
		else if (format == GL_RGBA8) {
			channel_format = GL_RGBA;
			channel_type = GL_UNSIGNED_BYTE;
		}
		else if (format == GL_SRGB8_ALPHA8) {
			channel_format = GL_SRGB_ALPHA;
			channel_type = GL_UNSIGNED_BYTE;
		}
	}
}

GlTexture2D::GlTexture2D(uint32_t format, uint32_t width, uint32_t height, uint32_t levels) {
	init(format, width, height, levels);
}

void GlTexture2D::init(uint32_t format, uint32_t width, uint32_t height, uint32_t levels) {
	this->width = width;
	this->height = height;
	this->levels = levels;
	this->format = format;

	if (levels == 0) {
		uint32_t temp_width = width;
		uint32_t temp_height = height;
		levels = 1;
		while (temp_width > 1 || temp_height > 1) {
			levels++;
			temp_width = temp_width == 1 ? 1 : temp_width / 2;
			temp_height = temp_height == 1 ? 1 : temp_height / 2;
		}
	}
	get_channel_format_type(format, channel_format, channel_type);
	glCreateTextures(GL_TEXTURE_2D, 1, &gl_texture);
	glTextureStorage2D(gl_texture, levels, format, width, height);
	glTextureParameteri(gl_texture, GL_TEXTURE_MIN_FILTER, levels == 1 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(gl_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// glTextureParameteri(gl_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTextureParameteri(gl_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(gl_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(gl_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

GlTexture2D::~GlTexture2D() {
	glDeleteTextures(1, &gl_texture);
}

void GlTexture2D::set_data(const void* data, uint32_t level) {
	auto level_width = std::max(1u, width >> level);
	auto level_height = std::max(1u, height >> level);
	glTextureSubImage2D(gl_texture, level, 0, 0, level_width, level_height, channel_format, channel_type, data);
}

void GlTexture2D::generate_mipmap() {
	glGenerateTextureMipmap(gl_texture);
}

void GlTexture2D::dump_png_file(uint32_t level) {
	// hardcode RGBA here
    auto pixels = std::make_unique<GLubyte[]>(width * height * 4);
	glGetTextureImage(gl_texture, level, channel_format, channel_type, width * height * 4, pixels.get());
	save_png_file("./test.png", std::move(pixels), width, height);
}

}

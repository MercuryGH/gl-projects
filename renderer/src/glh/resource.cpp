#include <glh/resource.hpp>

#include <cmath>

#include <glad/glad.h>

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

GlBuffer::GlBuffer(uint64_t size, uint32_t usage, const void* data) : sz(size) {
	if ((usage & GL_MAP_WRITE_BIT) != 0) {
		usage |= GL_MAP_READ_BIT;
	}
	glCreateBuffers(1, &gl_buffer);
	glNamedBufferStorage(gl_buffer, size, data, usage);
}

GlBuffer::~GlBuffer() {
	unmap();
	glDeleteBuffers(1, &gl_buffer);
}

void* GlBuffer::map(bool write) {
	if (!mapped_ptr) {
		mapped_ptr = glMapNamedBuffer(gl_buffer, write ? GL_READ_WRITE : GL_READ_ONLY);
	}
	return mapped_ptr;
}

void GlBuffer::unmap() {
	if (mapped_ptr) {
		glUnmapNamedBuffer(gl_buffer);
		// synchronize()
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		mapped_ptr = nullptr;
	}
}

GlTexture2D::GlTexture2D(uint32_t format, uint32_t width, uint32_t height, uint32_t levels)
	: width(width), height(height), levels(levels), format(format) {
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

}
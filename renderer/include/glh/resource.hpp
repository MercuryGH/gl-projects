#pragma once

#include <cstdint>

namespace renderer {

class GlBuffer {
public:
	GlBuffer(uint64_t size, uint32_t usage = 0, const void* data = nullptr);
	~GlBuffer();

	uint32_t id() const { return gl_buffer; }

	uint64_t size() const { return sz; }

	void* map(bool write = false);

	template <typename T>
	T* typed_map(bool write = false) { return reinterpret_cast<T*>(map(write)); }

	void unmap();

private:
	uint32_t gl_buffer = 0;
	uint64_t sz;
	void* mapped_ptr = nullptr;
};

class GlTexture2D {
public:
	GlTexture2D(uint32_t format, uint32_t width, uint32_t height, uint32_t levels = 0);
	~GlTexture2D();

	uint32_t id() const { return gl_texture; }

	uint32_t get_width() const { return width; }
	uint32_t get_height() const { return height; }
	uint32_t get_levels() const { return levels; }
	uint32_t get_format() const { return format; }

	void set_data(const void* data, uint32_t level = 0);

	void generate_mipmap();

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
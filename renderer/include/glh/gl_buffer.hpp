#pragma once

#include <cstdint>
#include <memory>

#include <glad/glad.h>

namespace renderer {

class GlBuffer {
public:
	GlBuffer(uint64_t size, uint32_t usage=0, const void* data=nullptr);
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

}

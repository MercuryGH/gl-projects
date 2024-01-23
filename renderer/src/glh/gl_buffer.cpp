#include <glh/gl_buffer.hpp>

#include <cmath>

namespace renderer {

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

}

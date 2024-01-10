#pragma once

#include <memory>
#include <vector>

#include <glh/program.hpp>
#include <glh/resource.hpp>

namespace renderer {

std::vector<uint8_t> get_cached_file_data(const char* path);

std::unique_ptr<GlComputeProgram> build_compute_program(const char* spv_path);

std::unique_ptr<GlGraphicsProgram> build_graphics_program(const char* vs_spv_path, const char* fs_spv_path);

std::unique_ptr<GlTexture2D> read_texture(const char* path, bool read_from_cache=false);

std::unique_ptr<GlTexture2D> create_texture_from_raw(void* img_data, int n_channels, int width, int height);

}
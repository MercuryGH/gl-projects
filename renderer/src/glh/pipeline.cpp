#include <glh/pipeline.hpp>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(shaders_spv);
CMRC_DECLARE(assets);

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace renderer {

namespace {

std::unique_ptr<GlShader> build_shader(const char* spv_path, uint32_t type) {
    auto spv_file = cmrc::shaders_spv::get_filesystem().open(spv_path);
    // check 4B aligned SPV
    assert(spv_file.size() > 0 && spv_file.size() % 4 == 0);
    std::vector<uint8_t> spv_data(spv_file.size());
    std::copy(spv_file.begin(), spv_file.end(), spv_data.data());

    return std::make_unique<GlShader>(spv_data.data(), static_cast<uint32_t>(spv_file.size()), type);
}

}

std::unique_ptr<GlComputeProgram> build_compute_program(const char* spv_path) {
    return std::make_unique<GlComputeProgram>(
        *build_shader(spv_path, GL_COMPUTE_SHADER)
    );
}

std::unique_ptr<GlGraphicsProgram> build_graphics_program(const char* vs_spv_path, const char* fs_spv_path) {
    return std::make_unique<GlGraphicsProgram>(
        *build_shader(vs_spv_path, GL_VERTEX_SHADER), 
        *build_shader(fs_spv_path, GL_FRAGMENT_SHADER)
    );
}

std::unique_ptr<GlTexture2D> read_texture(const char* path) {
    auto file = cmrc::assets::get_filesystem().open(path);
    std::vector<uint8_t> file_data(file.size());
    std::copy(file.begin(), file.end(), file_data.data());

    int num_channels;
    int width;
    int height;
    auto img_data = stbi_load_from_memory(file_data.data(), file.size(), &width, &height, &num_channels, 0);

    std::unique_ptr<GlTexture2D> texture = create_texture_from_raw(img_data, num_channels, width, height);

    stbi_image_free(img_data);

    return texture;
}

std::unique_ptr<GlTexture2D> create_texture_from_raw(void* img_data, int n_channels, int width, int height) {
    std::unique_ptr<GlTexture2D> texture;

    if (n_channels == 4) {
        texture = std::make_unique<GlTexture2D>(GL_RGBA8, width, height);
        texture->set_data(img_data);
        texture->generate_mipmap();
    }
    else if (n_channels == 3) {
        texture = std::make_unique<GlTexture2D>(GL_RGB8, width, height);
        texture->set_data(img_data);
        texture->generate_mipmap();
    }
    else {
        static const uint8_t default_tex_data[] = { 255, 255, 255, 255 };
        texture = std::make_unique<GlTexture2D>(GL_RGBA8, 1, 1, 1);
        texture->set_data(default_tex_data);
    }

    return texture;
}

}
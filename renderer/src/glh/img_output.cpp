#include <glh/img_output.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace renderer {

void save_png_file(std::string path, std::unique_ptr<GLubyte[]> pixels, int width, int height) {
	stbi_flip_vertically_on_write(true);
	stbi_write_png(path.c_str(), width, height, 4, pixels.get(), 0);
}

}

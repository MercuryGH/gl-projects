#pragma once

#include <string>
#include <memory>

#include <glad/glad.h>

namespace renderer {

void save_png_file(std::string path, std::unique_ptr<GLubyte[]> pixels, int width, int height);

}

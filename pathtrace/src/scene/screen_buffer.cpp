#include <scene/screen_buffer.hpp>

#include <omp.h>

namespace pathtrace {

ScreenBuffer::ScreenBuffer(int width, int height): width(width), height(height) {
    clear();
}

void ScreenBuffer::clear() {
    colors = std::vector<Vector3>(width * height);
    for (int i = 0; i < colors.size(); i++) {
        colors.at(i) = Vector3{ 0, 0, 0 };
    }
}

void ScreenBuffer::foreach_pixel_parallel(const std::function<void(int, int)>& func) const {
    #pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            func(x, y);
        }
    }
}

}
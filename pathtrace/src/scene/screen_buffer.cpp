#include <scene/screen_buffer.hpp>

namespace pathtrace {

ScreenBuffer::ScreenBuffer(int width, int height): width(width), height(height) {
    clear();
}

void ScreenBuffer::clear() {
    colors = std::vector<Vector3>(width * height);
}

void ScreenBuffer::foreach_pixel(const std::function<void(int, int)>& func) const {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            func(x, y);
        }
    }
}

}
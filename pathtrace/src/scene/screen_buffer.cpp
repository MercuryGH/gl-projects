#include <scene/screen_buffer.hpp>

#include <omp.h>

namespace pathtrace {

ScreenBuffer::ScreenBuffer(int width, int height): width(width), height(height) {
    clear();
}

ScreenBuffer::ScreenBuffer(const ScreenBuffer& buf) {
    assert(buf.get_n_pixels() == get_n_pixels()); colors = buf.colors;
}

ScreenBuffer& ScreenBuffer::operator=(const ScreenBuffer& rhs) {
    assert(rhs.get_n_pixels() == get_n_pixels());
    if (this != &rhs) {
        colors = rhs.colors;
    }
    return *this;
}

int ScreenBuffer::get_pixel_idx(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return -1; // note
    }

    return y * width + x;
}

void ScreenBuffer::set_size(int width, int height) {
    this->width = width;
    this->height = height;
    clear();
}

void ScreenBuffer::clear() {
    colors = std::vector<Vector3>(width * height);
    debug_hint.map = std::vector<PixelDebugHintType>(width * height);
    for (int i = 0; i < colors.size(); i++) {
        colors.at(i) = Vector3{ 0, 0, 0 };
    }
}

void ScreenBuffer::foreach_pixel_parallel(const std::function<void(int, int)>& func) const {
    // #pragma omp parallel for collapse(2)
    #pragma omp parallel for schedule(dynamic) collapse(2)
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            func(x, y);
        }
    }
}

void ScreenBuffer::set_pixel_debug(int x, int y, PixelDebugHintType type) {
    // set all to its neighbors
    constexpr int k_circle_dis = 5;
    for (int i = 0; i < k_circle_dis; i++) {
        for (int j = 0; j < k_circle_dis - i; j++) {
            const int x_dirs[2] = { -1, 1 };
            const int y_dirs[2] = { -1, 1 };
            for (int x_i = 0; x_i < 2; x_i++) {
                for (int y_i = 0; y_i < 2; y_i++) {
                    int px = x + x_dirs[x_i];
                    int py = y + y_dirs[y_i];
                    int idx = get_pixel_idx(px, py);
                    if (idx != -1) {
                        debug_hint.map.at(idx) = type;
                    }
                }
            }
        }
    }
    debug_hint.map.at(get_pixel_idx(x, y)) = type;
}

PixelDebugHintType ScreenBuffer::get_pixel_debug(int x, int y) const {
    return debug_hint.map.at(get_pixel_idx(x, y));
}

}

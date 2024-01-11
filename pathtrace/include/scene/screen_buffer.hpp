#pragma once

#include <vector>
#include <functional>

#include <util/types.hpp>

namespace pathtrace {

class ScreenBuffer {
public:
    ScreenBuffer() {}
    ScreenBuffer(int width, int height);
    void clear();

    void set_size(int width, int height);

    int get_n_pixels() const { return width * height; }
    int get_pixel_idx(int x, int y) const { return y * width + x; }
    Vector3 get_pixel(int x, int y) const { return colors.at(get_pixel_idx(x, y)); }
    void add_pixel_color(int x, int y, Vector3 rgb_color) { colors.at(get_pixel_idx(x, y)) += rgb_color; }

    void foreach_pixel_parallel(const std::function<void(int, int)>& func) const;

private:
    int width;
    int height;
    std::vector<Vector3> colors;
};

}

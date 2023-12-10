#pragma once

#include <zbuffer/zbuffer_pixel.hpp>
#include <zbuffer/color.hpp>

#include <vector>

namespace rasterize {

class ZBuffer {
    friend class Scene;
public:
    ZBuffer(int width, int height);

    void clear();

    PixelIndex screen_pixel_idx(int x, int y) const;
    ScalarType get_depth(int x, int y) const;

    void rasterize(Triangle tri);

    void set_pixel(int x, int y, ScalarType depth);

    void build_hierarchical_z_buf(); // build pyramid
    void update_hierarchical_z_buf(int x, int y, ScalarType depth);

    // query if this triangle is occluded (entry)
    bool is_occluded(Triangle triangle) const;

    void foreach_pixel(const std::function<void(int, int)>& func) const;

private:
    // query if this depth at certain bb is occluded (PixelIndex from top to bottom)
    bool is_occluded(PixelIndex index, BoundingRect bb, ScalarType depth) const;

    int width;
    int height;

    BoundingRect screen_bb;

    std::vector<ZBufferPixel> pixels; // use PixelIndex (int) to index
    ScalarType min_depth{ std::numeric_limits<ScalarType>::max() };
    ScalarType max_depth{ std::numeric_limits<ScalarType>::min() };

    PixelIndex root_index{ k_nil };
};

}

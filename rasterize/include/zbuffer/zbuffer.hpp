#pragma once

#include <zbuffer/zbuffer_node.hpp>
#include <zbuffer/color.hpp>

#include <vector>

namespace rasterize {

class ZBuffer {
    friend class Scene;
public:
    ZBuffer(int width, int height);
    
    void clear();

    PixelIndex pixel_idx(int x, int y) const;
    ScalarType get_depth(int x, int y) const;
    RgbColor get_color(int x, int y) const;
    
    void save_to_file(); // debug only
    void rasterize(const Triangle& tri);
    void rasterize_with_bvh(const Triangle& tri); // update bvh

    void set_pixel(int x, int y, ScalarType depth, const RgbColor& color);
    void set_pixel_with_bvh(int x, int y, ScalarType depth, const RgbColor& color);

    void build_hierarchical_z_buf(); // build bvh
    void clear_hierarchical_z_buf();

    bool is_in_zbuffer(const BoundingRect& rect, ScalarType depth) const;
    bool is_in_zbuffer(PixelIndex index, const BoundingRect& rect, ScalarType depth) const;

    bool is_in_zbuffer(const Triangle& tri, ScalarType depth) const;

    void foreach_pixel(const std::function<void(int, int)>& func) const;

private:
    int width;
    int height;

    BoundingRect screen_bb;

    std::vector<ZBufferNode> nodes; // use PixelIndex (int) to index
    PixelIndex root;
};

}
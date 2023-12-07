#include <zbuffer/zbuffer.hpp>

#include <scene/bounding_box.hpp>

// TODO: debug only
#include <string>
#include <iostream>
#include <fstream>

namespace rasterize {

ZBuffer::ZBuffer(int width, int height): width(width), height(height)
{
    screen_bb.merge({0, 0, 1});
    screen_bb.merge({width - 1, height - 1, 1});
    clear();
}

void ZBuffer::clear() {
    nodes.clear();
    nodes.resize(width * height);
}

PixelIndex ZBuffer::pixel_idx(int x, int y) const
{
    return y * width + x;
}

ScalarType ZBuffer::get_depth(int x, int y) const
{
    return nodes.at(pixel_idx(x, y)).depth;
}

RgbColor ZBuffer::get_color(int x, int y) const
{
    return nodes.at(pixel_idx(x, y)).color;
}

void ZBuffer::save_to_file() {
    std::string s = "test.ppm";
    std::ofstream file(s);
    file << "P3\n"
         << width << " " << height << "\n255\n";
    for (int x_t = 0; x_t < width; x_t++) {
        for (int y_t = 0; y_t < height; y_t++) {
            const auto& color = get_color(x_t, y_t);
            file << static_cast<int>(color.r) << " " << static_cast<int>(color.g) << " " << static_cast<int>(color.b) << "  ";
        }
        file << "\n";
    }
}

void ZBuffer::rasterize(const Triangle &tri)
{
    BoundingRect tri_bb;
    tri_bb.merge(tri);
    tri_bb.intersect(screen_bb);
    tri_bb.round_to_int();

    tri_bb.foreach_pixel([&](int x, int y) {
        Vector2 point(x, y);
        if (tri.contains_point(point)) {
            ScalarType depth = tri.interpolate_depth(point);
            if (depth < get_depth(x, y)) {
                set_pixel(x, y, depth, tri.get_color());
            }
        }
    });
}

void ZBuffer::rasterize_with_bvh(const Triangle &tri)
{
}

void ZBuffer::set_pixel(int x, int y, ScalarType depth, const RgbColor &color)
{
    PixelIndex id = pixel_idx(x, y);
    nodes.at(id).color = color;
    nodes.at(id).depth = depth;
}

// TODO: modify function name to update_bvh()
void ZBuffer::set_pixel_with_bvh(int x, int y, ScalarType depth, const RgbColor &color)
{
    set_pixel(x, y, depth, color);
    // update_bvh()
}

void ZBuffer::build_hierarchical_z_buf()
{
}

void ZBuffer::clear_hierarchical_z_buf()
{
}

bool ZBuffer::is_in_zbuffer(const BoundingRect &rect, ScalarType depth) const
{
    return false;
}

bool ZBuffer::is_in_zbuffer(PixelIndex index, const BoundingRect &rect, ScalarType depth) const
{
    return false;
}

bool ZBuffer::is_in_zbuffer(const Triangle &tri, ScalarType depth) const
{
    return false;
}

void ZBuffer::foreach_pixel(const std::function<void(int, int)>& func) const
{
    screen_bb.foreach_pixel(func);
}

}

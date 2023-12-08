#include <zbuffer/zbuffer.hpp>

#include <scene/bounding_box.hpp>
#include <scene/triangle.hpp>

namespace rasterize {

ZBuffer::ZBuffer(int width, int height): width(width), height(height)
{
    screen_bb.merge({0, 0, 1});
    screen_bb.merge({width - 1, height - 1, 1});
    clear();
}

void ZBuffer::clear() {
    pixels.clear();
    pixels.resize(width * height);
    min_depth = std::numeric_limits<ScalarType>::max();
    max_depth = std::numeric_limits<ScalarType>::min();
    root_index = k_nil;
}

PixelIndex ZBuffer::pixel_idx(int x, int y) const
{
    return y * width + x;
}

ScalarType ZBuffer::get_depth(int x, int y) const
{
    return pixels.at(pixel_idx(x, y)).depth;
}

RgbColor ZBuffer::get_color(int x, int y) const
{
    return pixels.at(pixel_idx(x, y)).color;
}

void ZBuffer::rasterize(Triangle tri)
{
    BoundingRect tri_bb;
    tri_bb.merge(tri);
    tri_bb.intersect(screen_bb);
    tri_bb.round_to_int();

    tri_bb.foreach_pixel([&](int x, int y) {
        Vector2 point(x, y);
        if (tri.contains_point(point)) {
            ScalarType depth = tri.interpolate_depth(point);
            if (depth < get_depth(x, y) && depth > 0) { // z-culling
                set_pixel(x, y, depth, tri.get_color());
            }
        }
    });
}

void ZBuffer::set_pixel(int x, int y, ScalarType depth, const RgbColor &color)
{
    PixelIndex id = pixel_idx(x, y);
    pixels.at(id).color = color;
    pixels.at(id).depth = depth;
    min_depth = std::min(depth, min_depth);
    max_depth = std::max(depth, max_depth);

    if (root_index != k_nil) {
        update_hierarchical_z_buf(x, y, depth);
    }
}

void ZBuffer::build_hierarchical_z_buf()
{
    int cur_width, last_width, cur_height, last_height;

    std::vector<std::vector<int>> cur_map;
    std::vector<std::vector<int>> last_map;

    // construct tree (mipmap, pyramid) from bottom to top
    while ((cur_height == 1 && cur_width == 1) == false) {
        last_width = cur_width;
        last_height = cur_height;
        cur_height = cur_height % 2 == 1 ? cur_height / 2 + 1 : cur_height / 2;
        cur_width = cur_width % 2 == 1 ? cur_width / 2 + 1 : cur_width / 2;

        cur_map.resize(cur_height);
        for (int y = 0; y < cur_height; y++) {
            cur_map.at(y).resize(cur_width);
        }

        for (int y = 0; y < cur_height; y++) {
            for (int x = 0; x < cur_width; x++) {
                ZBufferPixel pixel{ .node_type = NodeType::eNonLeaf };
                for (int y_offset = 0; y_offset < 2; y_offset++) {
                    for (int x_offset = 0; x_offset < 2; x_offset++) {
                        int x_last = 2 * x + x_offset;
                        int y_last = 2 * y + y_offset;
                        if (x_last >= last_width || y_last >= last_height) {
                            pixel.children_indices[2 * y_offset + x_offset] = k_nil; // border
                        } else {
                            pixel.children_indices[2 * y_offset + x_offset] = last_map.at(y_last).at(x_last);
                        }
                    }
                }
            }
        }
    }
}

void ZBuffer::update_hierarchical_z_buf(int x, int y, ScalarType depth)
{
    // update from bottom to top
    for (PixelIndex index = pixel_idx(x, y); index != k_nil; ) {
        PixelIndex parent_index = pixels.at(index).parent_index;
        if (parent_index == k_nil) {
            break;
        }

        auto& parent_pixel = pixels.at(parent_index);
        ScalarType parent_depth = parent_pixel.depth;
        if (depth < parent_depth) {
            parent_pixel.depth = depth; // update min value
        } else {
            break; // no need to update
        }
        index = parent_index;
    }
}

bool ZBuffer::is_occluded(Triangle triangle) const
{
    BoundingRect tri_bb;
    tri_bb.merge(triangle);
    return is_occluded(root_index, tri_bb, triangle.get_min_depth());
}

bool ZBuffer::is_occluded(PixelIndex index, BoundingRect target_bb, ScalarType depth) const
{
    const auto& pixel = pixels.at(index);
    for (int i = 0; i < 4; i++) {
        if (pixel.children_indices[i] == k_nil) {
            continue;
        }
        PixelIndex child_index = pixel.children_indices[i];
        const BoundingRect& child_bb = pixels.at(child_index).bb;
        if (target_bb.contains(child_bb)) {
            return is_occluded(child_index, child_bb, depth);
        }
    }

    return pixel.depth < depth; // depth test
}

void ZBuffer::foreach_pixel(const std::function<void(int, int)>& func) const
{
    screen_bb.foreach_pixel(func);
}

}

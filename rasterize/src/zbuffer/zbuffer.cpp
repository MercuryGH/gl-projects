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

PixelIndex ZBuffer::screen_pixel_idx(int x, int y) const
{
    return y * width + x;
}

ScalarType ZBuffer::get_depth(int x, int y) const
{
    return pixels.at(screen_pixel_idx(x, y)).depth;
}

RgbColor ZBuffer::get_color(int x, int y) const
{
    return pixels.at(screen_pixel_idx(x, y)).color;
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
    PixelIndex id = screen_pixel_idx(x, y);
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
    // clear pixels except for leaf level
    pixels.resize(width * height);

    int cur_width, last_width, cur_height, last_height;
    cur_width = width;
    cur_height = height;

    // init leaf level
    for (int y = 0; y < cur_height; y++) {
        for (int x = 0; x < cur_width; x++) {
            auto& bb = pixels.at(screen_pixel_idx(x, y)).bb;
            bb.make_empty();
            bb.merge({ x - 0.5f, y - 0.5f, std::numeric_limits<ScalarType>::min() });
            bb.merge({ x + 0.5f, y + 0.5f, std::numeric_limits<ScalarType>::max() });
        }
    }

    std::vector<std::vector<int>> cur_map; // current level
    std::vector<std::vector<int>> last_map(cur_height); // children level
    for (int y = 0; y < cur_height; y++) {
        last_map.at(y).resize(cur_width);
        for (int x = 0; x < cur_width; x++) {
            last_map.at(y).at(x) = screen_pixel_idx(x, y);
        }
    }

    // construct tree (mipmap, pyramid) from bottom to top
    for (; cur_height > 1 || cur_width > 1; ) {
        last_width = cur_width;
        last_height = cur_height;
        cur_height = cur_height % 2 == 1 ? cur_height / 2 + 1 : cur_height / 2;
        cur_width = cur_width % 2 == 1 ? cur_width / 2 + 1 : cur_width / 2;

        cur_map.resize(cur_height);
        for (int y = 0; y < cur_height; y++) {
            cur_map.at(y).resize(cur_width);
        }

        // fill current level mipmap
        for (int y = 0; y < cur_height; y++) {
            for (int x = 0; x < cur_width; x++) {
                ZBufferPixel pixel{ .node_type = NodeType::eNonLeaf };

                // fill current node children
                for (int y_offset = 0; y_offset < 2; y_offset++) {
                    for (int x_offset = 0; x_offset < 2; x_offset++) {
                        int x_last = 2 * x + x_offset;
                        int y_last = 2 * y + y_offset;

                        const int child_index = 2 * y_offset + x_offset;
                        if (x_last >= last_width || y_last >= last_height) {
                            pixel.children_indices[child_index] = k_nil; // border
                        } else {
                            pixel.children_indices[child_index] = last_map.at(y_last).at(x_last);
                        }
                    }
                }
                if (pixel.children_indices[0] == k_nil) {
                    printf("nonleaf node should have at least 1 child\n");
                    assert(false);
                }

                // assign depth from previous rasterization info
                // so we need to launch hi-z after a basic z
                pixel.depth = pixels.at(pixel.children_indices[0]).depth;
                for (int i = 1; i < 4; i++) {
                    if (pixel.children_indices[i] != k_nil) {
                        pixel.depth = std::max(pixel.depth, pixels.at(pixel.children_indices[i]).depth);
                    }
                }

                // assign bb
                BoundingRect bb(pixels.at(pixel.children_indices[0]).bb);
                for (int i = 1; i < 4; i++) {
                    if (pixel.children_indices[i] != k_nil) {
                        bb.merge(pixels.at(pixel.children_indices[i]).bb);
                    }
                }
                pixel.bb = bb;

                pixel.parent_index = k_nil;

                // add new pixel
                PixelIndex pixel_index = pixels.size();
                pixels.push_back(pixel);
                cur_map.at(y).at(x) = pixel_index;

                // update children's parent info
                for (int i = 0; i < 4; i++) {
                    if (pixel.children_indices[i] != k_nil) {
                        pixels.at(pixel.children_indices[i]).parent_index = pixel_index;
                    }
                }
            }
        }
        std::swap(last_map, cur_map);
        if (last_map.size() != cur_height || last_map.at(0).size() != cur_width) {
            printf("Bad: swap scale not match!");
            assert(false);
        }
    }

    root_index = last_map.at(0).at(0);

    // Check potential errors
    if (root_index != pixels.size() - 1) {
        printf("Root is not the last element of node_array\n");
        assert(false);
    }
    for (int i = 0; i < pixels.size() - 1; i++) {
        if (pixels[i].parent_index < 0 || pixels[i].parent_index > pixels.size() - 1)
            printf("Father of node %d is invalid, the value is %d. array size %d\n",
                i, pixels[i].parent_index, pixels.size());

        if (pixels[i].node_type == NodeType::eLeaf) {
            for (int c = 0; c < 4; c++) {
                if (pixels[i].children_indices[c] != -1)
                    printf("Nodes %d are leave node but children's id not equal to -1\n", i);
            }
        }
    }
}

void ZBuffer::update_hierarchical_z_buf(int x, int y, ScalarType depth)
{
    // update from bottom to top
    for (PixelIndex index = screen_pixel_idx(x, y); index != k_nil; ) {
        PixelIndex parent_index = pixels.at(index).parent_index;
        if (parent_index == k_nil) {
            break;
        }

        auto& parent_pixel = pixels.at(parent_index);
        ScalarType prev_parent_depth = parent_pixel.depth;
        parent_pixel.depth = depth;
        for (int i = 0; i < 4; i++) {
            if (parent_pixel.children_indices[i] != k_nil) {
                ScalarType child_pixel_depth = pixels.at(parent_pixel.children_indices[i]).depth;
                if (child_pixel_depth > parent_pixel.depth) {
                    parent_pixel.depth = child_pixel_depth;
                }
            }
        }

        if (prev_parent_depth == parent_pixel.depth) {
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
        if (child_bb.contains(target_bb)) {
            return is_occluded(child_index, target_bb, depth);
        }
    }

    return pixel.depth < depth; // depth test
}

void ZBuffer::foreach_pixel(const std::function<void(int, int)>& func) const
{
    screen_bb.foreach_pixel(func);
}

}

#pragma once

#include <scene/bounding_box.hpp>
#include <zbuffer/color.hpp>

namespace rasterize {

using PixelIndex = int32_t;
constexpr PixelIndex k_nil = -1; // use -1 to indicate null

struct ZBufferPixel {
    enum NodeType {
        eLeaf,
        eNonLeaf
    } node_type{ eLeaf };
    PixelIndex parent_index{ k_nil };
    PixelIndex children_indices[4]{ k_nil, k_nil, k_nil, k_nil };

    ScalarType depth{ std::numeric_limits<ScalarType>::max() }; // z val, init to INF
    RgbColor color{ 0x00, 0x00, 0x00 };

    BoundingRect bb;
};

}

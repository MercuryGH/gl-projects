#pragma once

#include <scene/bounding_box.hpp>
#include <zbuffer/color.hpp>

namespace rasterize {

using PixelIndex = int32_t;
constexpr PixelIndex k_nil = -1; // use -1 to indicate null

enum NodeType {
    eLeaf,
    eNonLeaf
};

struct ZBufferPixel {
    NodeType node_type{ eLeaf };
    PixelIndex parent_index{ k_nil };
    PixelIndex children_indices[4]{ k_nil, k_nil, k_nil, k_nil };

    ScalarType depth{ std::numeric_limits<ScalarType>::max() }; // z val, init to INF
    RgbColor color{ 0x00, 0x00, 0x00 };

    BoundingRect bb;
};

}

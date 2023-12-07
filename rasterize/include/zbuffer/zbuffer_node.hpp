#pragma once

#include <scene/bounding_box.hpp>

namespace rasterize {

using PixelIndex = int32_t; // use -1 to indicate null

struct ZBufferNode {
    enum NodeType {
        eLeaf,
        eNonLeaf
    };

    NodeType node_type{ eLeaf };
    ScalarType depth{ std::numeric_limits<ScalarType>::max() }; // z val, init to INF
    RgbColor color;

    PixelIndex father{ -1 };
    PixelIndex children[4]{ -1, -1, -1, -1 };

    BoundingRect bb;
};

}
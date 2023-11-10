#pragma once

#include <scene/bounding_box.hpp>

namespace rasterize {

using PixelIndex = uint32_t;

struct ZBufferNode {
    enum NodeType {
        eLeaf,
        eNonLeaf
    };

    NodeType node_type;
    float depth; // z val
    RgbColor color;

    PixelIndex father;
    PixelIndex children[4];

    BoundingRect bb;
};

}
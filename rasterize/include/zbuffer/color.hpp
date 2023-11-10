#pragma once

#include <numbers>
#include <cstdint>

namespace rasterize {

struct RgbColor {
union {
    struct {
        uint8_t data[3];
    };
    struct {
        uint8_t r, g, b;
    };
};
};

}
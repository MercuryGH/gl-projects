#pragma once

#include <vector>
#include <util/types.hpp>

namespace pathtrace {

// float typed rgb
struct TextureRGBf {
    TextureRGBf(const std::vector<float>& data): data(data) {}

    TextureRGBf(int width, int height): width(width), height(height) {
        data.resize(width * height * n_channels);
    }

    Vector3 at(int u, int v) const;
    Vector3 at(ScalarType u, ScalarType v) const;
    Vector3 at(Vector2 uv) const;

    int width;
    int height;
    std::vector<float> data; // rgbrgb...

    constexpr static int n_channels = 3;
};

}
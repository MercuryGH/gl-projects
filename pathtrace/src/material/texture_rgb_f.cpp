#include <material/texture_rgb_f.hpp>
#include <util/math.hpp>

namespace pathtrace {

Vector3 TextureRGBf::at(int x, int y) const {
    x = util::clamp(x, 0, width);
    y = util::clamp(y, 0, height);

    const int idx = (y * width + x) * n_channels;
    
    return { data.at(idx), data.at(idx + 1), data.at(idx + 2) };
}

Vector3 TextureRGBf::at(ScalarType x, ScalarType y) const {
    return at(static_cast<int>(x), static_cast<int>(y));
}

Vector3 TextureRGBf::at(Vector2 uv) const {
    ScalarType u = uv.x;
    ScalarType v = uv.y;
    // inverse y axis
    return at(width * u, height * (1.0f - v));
}

}
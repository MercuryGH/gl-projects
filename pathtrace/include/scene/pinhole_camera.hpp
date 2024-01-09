#pragma once

#include <util/types.hpp>
#include <light/ray.hpp>

namespace pathtrace {

struct PinholeCamera {
    Vector3 eye;
    Vector3 lookat;
    Vector3 up;
    ScalarType fovy;
    ScalarType width;
    ScalarType height;

    void update();

    // cached world coordinate
    Vector3 pixel00_corner; // corner with min(x, y), position of pixel(0, 0)
    Vector3 delta_w; // width direction difference
    Vector3 delta_h; // height direction difference

    bool reverse_y{ true }; // reverse y-axis 

    // cast ray in world coordinate
    Ray cast_ray(int x, int y) const;
};

}
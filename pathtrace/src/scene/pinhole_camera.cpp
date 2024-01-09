#include <scene/pinhole_camera.hpp>

#include <util/math.hpp>
#include <glm/ext.hpp>

namespace pathtrace {

void PinholeCamera::update() {
    ScalarType aspect = width / height;
    ScalarType y_length = 2 * std::tan(glm::radians(fovy * 0.5f));
    ScalarType x_length = y_length * aspect;

    Vector4 np_corner_l = Vector4{ -x_length / 2, y_length / 2, -1, 1 };
    Vector4 pp_corner_l = Vector4{ x_length / 2, y_length / 2, -1, 1 };
    Vector4 nn_corner_l = Vector4{ -x_length / 2, -y_length / 2, -1, 1 };
    if (reverse_y == false) {
        np_corner_l.y = -np_corner_l.y;
        pp_corner_l.y = -pp_corner_l.y;
        nn_corner_l.y = -nn_corner_l.y;
    }

    // V^{-1}, to world coordinate
    auto lookat_inv = glm::inverse(glm::lookAt(eye, lookat, up));

}

Ray PinholeCamera::cast_ray(int x, int y) const {
    Vector3 origin = eye;

    return { origin, origin };
}

}
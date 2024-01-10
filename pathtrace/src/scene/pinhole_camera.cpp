#include <scene/pinhole_camera.hpp>

#include <util/math.hpp>
#include <util/random.hpp>
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
    Vector3 np_corner = Vector3(lookat_inv * np_corner_l);
    Vector3 pp_corner = Vector3(lookat_inv * pp_corner_l);
    Vector3 nn_corner = Vector3(lookat_inv * nn_corner_l);

    pixel00_corner = np_corner;
    delta_w = (pp_corner - np_corner) / width;
    delta_h = (nn_corner - np_corner) / height;
}

Ray PinholeCamera::cast_ray(int x, int y) const {
    Vector3 origin = eye;

    ScalarType random_w_offset = util::get_uniform_real_distribution(0.0f, 1.0f);
    ScalarType random_h_offset = util::get_uniform_real_distribution(0.0f, 1.0f);

    Vector3 random_offset = pixel00_corner + (x + random_w_offset - 0.5f) * delta_w + (y + random_h_offset - 0.5f) * delta_h;

    return { origin, random_offset - origin };
}

}
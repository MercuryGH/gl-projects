#include <scene/triangle.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <tuple>

namespace rasterize
{
bool Triangle::contains_point(Vector2 point) const
{
    // ref: https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
    const auto sign = [](Vector2 p1, Vector2 p2, Vector2 p3)
    {
        // cross product equivalent
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };
    const auto same_sign = [](ScalarType a, ScalarType b)
    {
        return (a < 0 && b < 0) || (a > 0 && b > 0);
    };

    ScalarType d[3];
    for (int i = 0; i < 3; i++)
    {
        d[i] = sign(point, p[i], p[(i + 1) % 3]);
        if (i > 0)
        {
            if (same_sign(d[i], d[i - 1]) == false)
            {
                return false;
            }
        }
    }

    return true;
}

std::tuple<ScalarType, ScalarType, ScalarType> Triangle::compute_barycentric_2d(Vector2 point) const
{
    // ref: Games 101 homework by Lingqi Yan
    const ScalarType x = point.x;
    const ScalarType y = point.y;

    ScalarType c1 = (x * (p[1].y - p[2].y) + (p[2].x - p[1].x) * y + p[1].x * p[2].y - p[2].x * p[1].y) / (p[0].x * (p[1].y - p[2].y) + (p[2].x - p[1].x) * p[0].y + p[1].x * p[2].y - p[2].x * p[1].y);
    ScalarType c2 = (x * (p[2].y - p[0].y) + (p[0].x - p[2].x) * y + p[2].x * p[0].y - p[0].x * p[2].y) / (p[1].x * (p[2].y - p[0].y) + (p[0].x - p[2].x) * p[1].y + p[2].x * p[0].y - p[0].x * p[2].y);
    ScalarType c3 = (x * (p[0].y - p[1].y) + (p[1].x - p[0].x) * y + p[0].x * p[1].y - p[1].x * p[0].y) / (p[2].x * (p[0].y - p[1].y) + (p[1].x - p[0].x) * p[2].y + p[0].x * p[1].y - p[1].x * p[0].y);

    return {c1, c2, c3};
}

ScalarType Triangle::interpolate_depth(Vector2 point) const
{
    Vector4 p[3];
    for (int i = 0; i < 3; i++) {
        p[i] = glm::vec4(this->p[i], 1.0f);
    }

    auto [alpha, beta, gamma] = compute_barycentric_2d(point);
    ScalarType w_reciprocal = 1.0 / (alpha / p[0].w + beta / p[1].w + gamma / p[2].w);
    ScalarType z_interpolated = (alpha * p[0].z / p[0].w + beta * p[1].z / p[1].w + gamma * p[2].z / p[2].w) * w_reciprocal;

    return z_interpolated;
}

RgbColor Triangle::get_color() const {
    // simple implementation
    return RgbColor{ 0x66, 0xcc, 0xff };
}

Triangle Triangle::vpv_transform(const renderer::CameraData& camera, Vector4 viewport) const {
    Triangle transformed_triangle;
    for (int i = 0; i < 3; i++) {
        transformed_triangle.p[i] = glm::project(p[i], Matrix4(1.0f), camera.proj * camera.view, viewport);
    }
    return transformed_triangle;
}

}

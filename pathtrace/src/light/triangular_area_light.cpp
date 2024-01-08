#include <light/triangular_area_light.hpp>

#include <util/random.hpp>
#include <util/math.hpp>

namespace pathtrace {

// ref: https://math.stackexchange.com/questions/538458/how-to-sample-points-on-a-triangle-surface-in-3d
Vector3 TriangularAreaLight::uniform_sample_point() const {
    ScalarType sqrt_a = std::sqrt(util::get_uniform_real_distribution(0.0, 1.0));
    ScalarType b = util::get_uniform_real_distribution(0.0, 1.0);

    return tri.p(0) * (1.0f - sqrt_a) + tri.p(1) * (sqrt_a * (1 - b)) + tri.p(2) * (sqrt_a * b);
}

}
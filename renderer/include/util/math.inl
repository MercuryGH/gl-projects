#include <cassert>

constexpr ScalarType k_pi = 3.141592653589793238463;
constexpr ScalarType k_inv_pi = 1.0 / k_pi;
constexpr ScalarType k_2pi = 2 * k_pi;
constexpr ScalarType k_inv_2pi = 1.0 / (2 * k_pi);

constexpr ScalarType k_eps = 1e-3;
constexpr auto k_max = std::numeric_limits<ScalarType>::max();
constexpr auto k_min = std::numeric_limits<ScalarType>::min();

namespace util {

template<typename T>
T clamp(T x, T a, T b)
{
    if (a > b) {
        assert(false);
    }
    return (x < a) ? a : (b < x) ? b : x;
}

template<typename T>
bool inside(T x, T a, T b)
{
    if (a > b) {
        assert(false);
    }
    return (a <= x && x <= b);
}

template<typename T>
int sign(T x)
{
    return x > 0 ? 1 : (x < 0 ? -1 : 0);
}

template<typename T>
T sq(T x)
{
    return x * x;
}

}
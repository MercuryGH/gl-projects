#include <util/math.hpp>

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
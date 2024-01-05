#include <util/random.hpp>

namespace util {

namespace {
    std::mt19937& get_rng() {
        static thread_local std::random_device dev;
        static thread_local std::mt19937 rng(dev());
        return rng;
    }
}

template<typename T>
T get_uniform_real_distribution(T l, T r) {
    std::uniform_real_distribution<T> dist(l, r);
    return dist(get_rng());
}

template<typename T>
T get_uniform_int_distribution(T l, T r) {
    std::uniform_int_distribution<T> dist(l, r);
    return dist(get_rng());
}

template double get_uniform_real_distribution<double>(double l, double r);
template int get_uniform_int_distribution<int>(int l, int r);
}

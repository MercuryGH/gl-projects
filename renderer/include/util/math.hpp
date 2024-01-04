#pragma once

#include <cassert>

#include <util/types.hpp>

namespace util {

template<typename T>
T clamp(T x, T a, T b);

template<typename T>
bool inside(T x, T a, T b);

template<typename T>
int sign(T x);

template<typename T>
T sq(T x);

}
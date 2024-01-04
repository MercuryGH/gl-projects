#pragma once

#include <random>

namespace util {

/**
 * U[l, r], T = float or double
*/
template<typename T>
T get_uniform_real_distribution(T l, T r);

/**
 * [l, ..., r], T = int
*/
template<typename T>
T get_uniform_int_distribution(T l, T r);

}
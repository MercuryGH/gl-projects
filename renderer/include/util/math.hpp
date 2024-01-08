#pragma once

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

Vector3 reflect(Vector3 wo, Vector3 normal);

Vector3 refract(Vector3 wo, Vector3 normal, ScalarType ior);

ScalarType fresnel_schlick(ScalarType cos, ScalarType ref_idx);

Vector3 local_to_world(Vector3 local, Vector3 unit_z);

Vector3 unit_sphere_to_cartesian(ScalarType cos_theta, ScalarType cos_phi);

Vector3 get_uniform_unit_sphere_distribution();

}

#include <util/math.inl>

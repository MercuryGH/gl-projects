#ifndef UTILS_SAMPLE_GLSL_
#define UTILS_SAMPLE_GLSL_

#include "constant.glsl"

vec3 uniform_sample_sphere_surface(vec2 rand) {
    float phi = rand.x * TWO_PI;
    float z = rand.y * 2.0 - 1.0;
    float r = sqrt(max(1.0 - z * z, 0.0));
    return vec3(cos(phi) * r, sin(phi) * r, z);
}

vec3 uniform_sample_sphere_volume(vec3 rand) {
    float r = pow(rand.z, 1.0 / 3.0);
    return uniform_sample_sphere_surface(rand.xy) * r;
}

vec3 uniform_sample_cone(vec2 rand, float cos_theta_max) {
    float cos_theta = (1.0 - rand.x) + rand.x * cos_theta_max;
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    float phi = rand.y * TWO_PI;
    return vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
}

#endif

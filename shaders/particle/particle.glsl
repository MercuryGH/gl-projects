#ifndef PARTICLE_GLSL_
#define PARTICLE_GLSL_

struct Particle {
    vec3 position;
    float mass;
    vec3 velocity;
    float life;
    vec3 acceleration;
    float size;
};

#endif

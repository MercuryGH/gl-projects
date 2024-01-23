#version 460

#extension GL_GOOGLE_include_directive : enable

#include "particle.glsl"

// Vertex shader attribute index (has location)
// in ...

// Program separation linkage (V out and F in has location)
layout(location = 0) out vec3 a_pos;
layout(location = 1) out vec3 a_norm;
layout(location = 2) out vec2 a_uv;

// SSBO binding. Qualifier "buffer" indicates this is a SSBO.
layout(binding = 0) buffer readonly Particles {
    Particle particles[];
};

// UBO with an interface block (has binding)
layout(binding = 1) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 view_inv;
} cam;

void main() {
    // gl_InstanceID is the index of current primitive in a instanced draw
    // get current particle
    Particle part = particles[gl_InstanceID];

    // current particle's attenuation
    float size = part.size * clamp(part.life, 0.0, 1.0);

    vec3 cam_pos = cam.view_inv[3].xyz;
    vec3 cam_up = cam.view_inv[1].xyz;

    vec3 forward = normalize(part.position - cam_pos);
    vec3 right = cross(forward, cam_up);

    vec3 pos_world;
    vec2 uv;
    
    // gl_VertexID is the index of current vertex
    // get the world position of the 4 corners vertices
    // The 4 corners form a rectangle, used for rendering.
    // part.position is the world pos of the center of the particle.
    if (gl_VertexID == 0) {
        pos_world = part.position - right * size - cam_up * size;
        uv = vec2(0.0, 0.0);
    } else if (gl_VertexID == 1) {
        pos_world = part.position + right * size - cam_up * size;
        uv = vec2(1.0, 0.0);
    } else if (gl_VertexID == 2) {
        pos_world = part.position + right * size + cam_up * size;
        uv = vec2(1.0, 1.0);
    } else { // if (gl_VertexID == 3) {
        pos_world = part.position - right * size + cam_up * size;
        uv = vec2(0.0, 1.0);
    }

    gl_Position = cam.proj * cam.view * vec4(pos_world, 1.0);
    a_pos = pos_world;
    a_norm = -forward;
    a_uv = uv;
}

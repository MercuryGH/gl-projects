#version 460

// Program separation linkage (V out and F in has location)
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_norm;
layout(location = 2) in vec2 a_uv;

layout(location = 0) out vec4 frag_color;

layout(binding = 2) uniform RenderParams {
    vec4 color;
} params;

layout(binding = 3) uniform sampler2D particle_tex;

void main() {
    vec4 color = texture(particle_tex, a_uv) * params.color;
    frag_color = color;
}

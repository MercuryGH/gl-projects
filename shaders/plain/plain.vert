#version 460

// attributes (V in)
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;

// V out
layout(location = 0) out vec3 frag_pos;
layout(location = 1) out vec3 normal;

// (not used)
// layout(location = 0) uniform mat4 model;

layout(binding = 1) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 view_inv;
} cam;

void main()
{
    gl_Position = cam.proj * cam.view * vec4(a_pos, 1.0);
    normal = a_normal;
    frag_pos = a_pos;
}

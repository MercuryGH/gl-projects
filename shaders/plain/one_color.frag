#version 460

layout(location = 0) in vec3 frag_pos;

layout(binding = 2) uniform RenderParams {
    vec4 color;
} params;

layout(location = 0) out vec4 frag_color;

void main()
{
    frag_color = params.color;
}
#version 460

layout(location = 0) in vec2 texcoord;

layout(location = 0) out vec4 frag_color;

layout(binding = 0) uniform sampler2D color_image;

float linear_to_srgb(float v) {
    return v < 0.0031308 ? v * 12.92 : pow(v, 1.0 / 2.4) * 1.055 - 0.055;
}

vec3 linear_to_srgb(vec3 v) {
    return vec3(linear_to_srgb(v.x), linear_to_srgb(v.y), linear_to_srgb(v.z));
}

void main() {
    vec3 color = texture(color_image, texcoord).xyz;
    frag_color = vec4(linear_to_srgb(color), 1.0);
}

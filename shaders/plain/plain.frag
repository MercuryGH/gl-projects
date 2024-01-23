#version 460

layout(location = 0) in vec3 frag_pos;
layout(location = 1) in vec3 normal;

layout(binding = 1) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 view_inv;
} cam;

layout(binding = 2) uniform RenderParams {
    vec4 color;
    bool two_side;
} params;

layout(location = 0) out vec4 frag_color;

vec3 view_pos()
{
    return cam.view_inv[3].xyz;
}

vec3 blinn_phong_shading()
{
    bool two_side = params.two_side;
    float two_side_control = -float(two_side == true); // 0 or -1

    vec3 light_pos = view_pos();
    vec3 light_color = vec3(1, 1, 1);

    float k_ambient = 0.1;
    vec3 ambient = k_ambient * light_color;

    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos - frag_pos);
    float dot_prod = dot(norm, light_dir);
    float diff = abs(max(dot_prod, two_side_control));
    vec3 diffuse = diff * light_color;

    float k_specular = 0.5;
    vec3 view_dir = normalize(view_pos() - frag_pos);
    vec3 halfway_dir = normalize(light_dir + view_dir);
    float dot_spec = dot(normal, halfway_dir);
    float abs_dot_spec = float(dot_prod == abs(dot_prod)) * abs(max(dot_spec, two_side_control));
    float spec = pow(abs_dot_spec, 35.0);

    vec3 specular = k_specular * spec * light_color;

    vec3 result = ambient + diffuse + specular;

    return result;
}

void main()
{
    vec3 color = blinn_phong_shading() * vec3(params.color);
    frag_color = vec4(color, 1.0);
}

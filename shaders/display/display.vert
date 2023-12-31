#version 460

layout(location = 0) out vec2 texcoord;

// define a full-screen quad using a single triangle:
// gl_VertexID = 0, 1, 2
// texcoord = (0, 0), (2, 0), (0, 2)
// gl_Position = (-1, 1), (3, 1), (-1, -3)
// Then in NDC there will be a [0,1]^2 texture uv

void main() {
    texcoord = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(texcoord.x * 2.0 - 1.0, 1.0 - texcoord.y * 2.0, 0.0, 1.0);
}

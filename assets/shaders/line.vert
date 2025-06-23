#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform Matrix {
    mat4 view;
    mat4 proj;
} matrix;

layout(set = 0, binding = 1) uniform Data {
    vec4 quaternion;
    vec3 root;
    float size;
} data;

vec3 rotate(vec3 v, vec4 q) {
    vec3 uv = cross(q.xyz, v);
    vec3 uuv = cross(q.xyz, uv);
    return v + 2.0 * (uv * q.w + uuv);
}

void main() {
    vec3 pos = rotate(position, data.quaternion) * data.size + data.root;
    gl_Position = matrix.proj * matrix.view * vec4(pos, 1.0);
    fragColor = color;
}

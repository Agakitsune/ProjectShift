#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec3 fragNormal;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view_proj;
} pc;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(pc.model)));
    gl_Position = pc.view_proj * pc.model * vec4(position, 1.0);
    fragNormal = normalize(normalMatrix * normal);
}

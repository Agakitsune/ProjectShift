#version 450

layout(location = 0) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    float ambientLight = 0.2; // Ambient light intensity
    vec3 lightDirection = normalize(vec3(0.0, 1.0, 0.0)); // Direction of the light source
    float diffuseLight = max(dot(fragNormal, lightDirection), 0.0); // Diffuse light intensity based on normal and light direction
    vec3 color = vec3(1, 0.38, 0.533) * (ambientLight + diffuseLight); // Base color of the cube with lighting
    outColor = vec4(pow(color, vec3(2.2)), 1.0); // Apply gamma correction to the output color
}

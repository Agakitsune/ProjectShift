#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragWorld;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D shadowMap; // Texture sampler

float PCF(vec4 shadow_coord) {
    vec2 tex_coord = shadow_coord.xy / shadow_coord.w; // Normalize texture coordinates
    tex_coord = tex_coord * 0.5 + 0.5; // Convert to [0, 1] range
    tex_coord.y = 1.0 - tex_coord.y; // Flip Y coordinate for correct sampling

    if (tex_coord.x < 0.0 || tex_coord.x > 1.0 || tex_coord.y < 0.0 || tex_coord.y > 1.0) {
        return 1.0; // Return 1.0 if coordinates are out of bounds
    }

    vec2 tex_size = textureSize(shadowMap, 0); // Get the size of the shadow map texture
    float result;

    vec2 offset = vec2(1.0) / tex_size; // Calculate offset for PCF sampling

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 sample_coord = tex_coord + vec2(x, y) * offset; // Calculate sample coordinates
            float sample_depth = texture(shadowMap, sample_coord).r; // Sample depth from shadow map
            if (sample_depth < shadow_coord.z / shadow_coord.w) {
                result += 1.0; // Increment result if the sample is in shadow
            }
        }
    }

    return result / 9.0; // Average the results of the 9 samples
}

void main() {
    const vec3 light_pos = vec3(1.0, 0.6, 0.5) * 100.0;
    mat4 light = mat4(1.0);

    // view matrix for the light source
    vec3 f = normalize(vec3(0.0) - light_pos);
    vec3 s = normalize(cross(vec3(0.0, 1.0, 0.0), f));
    vec3 u = cross(f, s);
    light[0] = vec4(s.x, u.x, -f.x, 0.0);
    light[1] = vec4(s.y, u.y, -f.y, 0.0);
    light[2] = vec4(s.z, u.z, -f.z, 0.0);
    light[3] = vec4(-dot(s, light_pos), -dot(u, light_pos), dot(f, light_pos), 1.0);

    ivec2 val = ivec2(floor(fragTexCoord * 10.0));
    vec3 color = vec3((val.x + val.y % 2) % 2 + 0.08);

    vec4 shadow = light * vec4(fragWorld, 1.0);
    float shadowFactor = PCF(shadow); // Calculate shadow factor using PCF

    outColor = vec4(min(color, 1.0) * shadowFactor, 1.0); // Output color based on texture coordinates
}

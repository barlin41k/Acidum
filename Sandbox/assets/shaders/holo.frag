#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 baseColor;
    float roughness;
    float metallic;
} pc;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec3 lightDir;
    vec3 viewPos;
} ubo;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(ubo.viewPos - fragPos);

    vec3 holoColor = vec3(0.0, 0.8, 1.0);

    float fresnel = pow(1.0 - max(dot(N, V), 0.0), 2.5);
    float scanline = sin(fragPos.y * 50.0) * 0.5 + 0.5;

    vec3 finalColor = holoColor * (fresnel * 2.0 + scanline * 0.3 + 0.2);
    float alpha = clamp(fresnel + 0.3, 0.0, 0.85);

    outColor = vec4(finalColor, alpha);
}
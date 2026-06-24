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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragPos;

void main() {
    vec4 vertexPos = vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * pc.model * vertexPos;
    
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = mat3(pc.model) * inNormal;
    fragPos = vec3(pc.model * vertexPos);
}
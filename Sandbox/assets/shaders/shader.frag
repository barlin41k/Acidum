#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

vec3 calculateDiffuseLight(vec3 normal, vec3 lightDir, vec3 lightColor) {
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightColor;
}

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    vec3 norm = normalize(fragNormal);

    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 lightColor = vec3(1.0);
    
    vec3 ambient = vec3(0.2);
    vec3 diffuse = calculateDiffuseLight(norm, lightDir, lightColor);

    vec3 finalLight = ambient + diffuse;

    outColor = vec4(finalLight * texColor.rgb, texColor.a);
}
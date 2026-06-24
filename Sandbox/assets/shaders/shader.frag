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

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;


const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) { // distribution of microfacets
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness) { // self-shadow of microfacets
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) { // final geometry
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) { // fresnel (reflection depends on viewing angle)
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


void main() {
    vec4 texColor = texture(texSampler, fragTexCoord) * pc.baseColor;
    vec3 albedo = texColor.rgb;

    vec3 N = normalize(fragNormal); // normal vector
    vec3 V = normalize(ubo.viewPos - fragPos); // vector from a point on an object to the camera (viewing direction)
    vec3 L = normalize(ubo.lightDir); // vector from a point to a light source
    vec3 H = normalize(V + L); // halfway vector (the ideal direction for the light to burst right into the camera)

    // we determine the material type.
    vec3 F0 = vec3(0.04); // dielectrics highlights are always white.
    F0 = mix(F0, albedo, pc.metallic); // if it's metal its F0 becomes equal to the albedo color.

    // roughness determines the chaos and scatter of these vectors m relative to N
    float NDF = DistributionGGX(N, H, pc.roughness); // the function returns the fraction of micromirrors rotated in the H direction, and the magnitude of this fraction depends on how far H has moved from N and how rough the surface is.
    float G = GeometrySmith(N, V, L, pc.roughness); // describes the loss of light due to microscopic relief
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0); // calculates the percentage of reflected light depending on the viewing angle

    vec3 numerator = NDF * G * F; // total amount of reflected energy
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // geometry and area correction
    vec3 specular = numerator / denominator; // final color of specular highlight

    vec3 kS = F; // the specular reflection coefficient kS is equal to the Fresnel value F (after all, Fresnel calculates how much is reflected)
    vec3 kD = vec3(1.0) - kS; // light cannot come from nowhere and cannot disappear into nowhere
    kD *= 1.0 - pc.metallic; // since metals completely absorb light passing through the surface and convert it into heat, they have no diffuse scattering

    float NdotL = max(dot(N, L), 0.0); // cos of the angle of incidence of light
    vec3 lightColor = vec3(10.0); // light color (strength)

    vec3 Lo = (kD * albedo / PI + specular) * lightColor * NdotL; // light outgoing

    vec3 ambient = vec3(0.03) * albedo;

    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0)); // this formula mathematically compresses the infinite range of brightness into the interval [0.0, 1.0]

    outColor = vec4(color, texColor.a);
}
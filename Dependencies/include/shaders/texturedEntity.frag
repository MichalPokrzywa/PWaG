#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 UV;
in vec3 ToCameraVector;
in vec4 LightSpaceFragPos;
in vec4 ViewSpace;
smooth in vec4 CurPos;
smooth in vec4 PrevPos;

layout (location = 0) out vec4 colorTexture;
layout (location = 1) out vec4 velocityTexture;

uniform vec3 color;
uniform vec3 lightPos;
uniform sampler2D shadowMap;
uniform sampler2D textures[8]; // Array of texture samplers
uniform float opacity;
uniform int receiveShadow;
uniform float blendFactors[8]; // Array of blend factors for each texture
uniform int textureCount;      // Number of textures to blend

float shadowCalculation(vec4 lightSpaceFragPos) {
    vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    if (currentDepth > 1.0)
        return 0;
    float bias = max(0.002 * (1.0 - dot(Normal, normalize(lightPos))), 0.0005);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    int sampleSize = 1;
    float shadow = 0.0;
    for(int x = -sampleSize; x <= sampleSize; ++x) {
        for(int y = -sampleSize; y <= sampleSize; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / ((sampleSize * 2 + 1) * (sampleSize * 2 + 1));
}

void main() {
    // Clamp UV coordinates to [0, 1] range
    vec2 clampedUV = clamp(UV, 0.0, 1.0);

    // Initialize blended color
    vec4 blendedColor = vec4(0.0);

    // Loop over the number of textures passed to the shader
    for (int i = 0; i < textureCount; i++) {
        vec4 textureColor = texture(textures[i], clampedUV);
        blendedColor += textureColor * blendFactors[i];
    }

    // Lighting calculations
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Shadow calculations
    float visibility = 1.0;
    if (receiveShadow == 1)
        visibility = 1.0 - shadowCalculation(LightSpaceFragPos) * 0.5;

    // Combine blended texture color with lighting and shadows
    vec3 fragColor = visibility * (diffuse * blendedColor.rgb);

    // Fog calculations
    float dist = abs(ViewSpace.z);
    float near = 100.0;
    float far = 500.0;
    float fogFactor = (far - dist) / (far - near);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 fogColor = vec3(0.968, 0.851, 0.667);
    vec3 finalColor = (1.0 - fogFactor) * fogColor + fogFactor * fragColor;

    // Output the final color with opacity
    colorTexture = vec4(finalColor, opacity * blendedColor.a);

    // Velocity (unchanged)
    vec2 a = (CurPos.xy / CurPos.w) * 0.5 + 0.5;
    vec2 b = (PrevPos.xy / PrevPos.w) * 0.5 + 0.5;
    vec2 difference = (a - b);
    difference.x = pow(difference.x, 3.0);
    difference.y = pow(difference.y, 3.0);
    velocityTexture = vec4(difference, 0.0, 1.0);
}
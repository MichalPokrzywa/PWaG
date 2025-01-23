#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 ToCameraVector;
in vec4 LightSpaceFragPos;
in vec4 ViewSpace;
in vec2 TexCoords; // Add texture coordinates
smooth in vec4 CurPos;
smooth in vec4 PrevPos;

layout (location = 0) out vec4 colorTexture;
layout (location = 1) out vec4 velocityTexture;

uniform vec3 color;
uniform vec3 lightPos; // Pozycja światła punktowego
uniform sampler2D shadowMap;
uniform sampler2D textureSampler; // Add texture sampler
uniform float opacity;
uniform int receiveShadow;
uniform bool hasTexture; // Add uniform to check if texture is provided

// Parametry światła punktowego
uniform float lightConstant;  // Stała attenuacja
uniform float lightLinear;    // Liniowa attenuacja
uniform float lightQuadratic; // Kwadratowa attenuacja

float shadowCalculation(vec4 lightSpaceFragPos) {
  vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;
  if (currentDepth > 1.0)
    return 0;
  float shadow = 0.0;
  float bias = max(0.002 * (1.0 - dot(Normal, normalize(lightPos))), 0.0005);
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  int sampleSize = 1;
  for(int x = -sampleSize; x <= sampleSize; ++x) {
    for(int y = -sampleSize; y <= sampleSize; ++y) {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / ((sampleSize * 2 + 1) * (sampleSize * 2 + 1));
}

void main() {
  vec3 fogColor = vec3(0.968, 0.851, 0.667);
  vec3 unitNormal = normalize(Normal);
  vec3 unitToCameraVector = normalize(ToCameraVector);

  // Światło punktowe
  vec3 lightColor = vec3(1.0, 1.0, 1.0); // Kolor światła
  vec3 lightDir = normalize(lightPos - FragPos); // Kierunek światła
  float distance = length(lightPos - FragPos); // Odległość od światła

  // Attenuacja (spadek intensywności światła)
  float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));

  // diffuse
  float diff = max(dot(unitNormal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // specular
  float specularStrength = 0.5;
  vec3 reflectedLightDir = reflect(-lightDir, unitNormal);
  float specularFactor = pow(max(dot(reflectedLightDir, unitToCameraVector), 0.0), 64);
  vec3 specular = specularStrength * specularFactor * lightColor;

  // shadow
  float visibility = 1.0;
  float shadow = 0.0;
  if (receiveShadow == 1)
    shadow = visibility * shadowCalculation(LightSpaceFragPos);

  // Texture sampling
  vec4 texColor = vec4(1.0); // Default color (white)
  if (hasTexture) {
    texColor = texture(textureSampler, TexCoords); // Sample the texture if provided
  }

  // Połącz oświetlenie z attenuacją (bez światła ambient)
  vec3 fragColor = ((1 - shadow) * (diffuse + specular)) * color * texColor.rgb * attenuation;

  // fog
  float dist = abs(ViewSpace.z);
  float near = 100.0;
  float far = 500.0;
  float fogFactor = (far - dist)/(far - near);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  vec3 finalColor = (1.0 - fogFactor) * fogColor + fogFactor * fragColor;
  colorTexture = vec4(finalColor, opacity * texColor.a); // Use texture alpha for opacity

  // velocity
  vec2 a = (CurPos.xy / CurPos.w) * 0.5 + 0.5;
  vec2 b = (PrevPos.xy / PrevPos.w) * 0.5 + 0.5;
  vec2 difference = (a - b);
  difference.x = pow(difference.x, 3.0);
  difference.y = pow(difference.y, 3.0);
  velocityTexture = vec4(difference, 0.0, 1.0);
}
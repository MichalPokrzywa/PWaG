#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;       // Original HDR scene texture
uniform sampler2D bloomTexture;      // Blurred bright areas texture
uniform float threshold;             // Brightness threshold for extraction
uniform float bloomStrength;         // Strength of the bloom effect
uniform bool horizontal;             // Direction of Gaussian blur (true = horizontal, false = vertical)
uniform int pass;                    // Current pass: 0 = brightness extraction, 1 = blur, 2 = bloom combination

// Gaussian blur weights
const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
    if (pass == 0) {
        // Brightness extraction pass
        vec3 color = texture(sceneTexture, TexCoords).rgb;
        float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722)); // Luminance calculation
        if (brightness > threshold) {
            FragColor = vec4(color, 1.0);
        } else {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
    } else if (pass == 1) {
        // Gaussian blur pass
        vec2 texOffset = 1.0 / textureSize(sceneTexture, 0); // Texel size
        vec3 result = texture(sceneTexture, TexCoords).rgb * weight[0]; // Current fragment's contribution

        if (horizontal) {
            for (int i = 1; i < 5; ++i) {
                result += texture(sceneTexture, TexCoords + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
                result += texture(sceneTexture, TexCoords - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
            }
        } else {
            for (int i = 1; i < 5; ++i) {
                result += texture(sceneTexture, TexCoords + vec2(0.0, texOffset.y * i)).rgb * weight[i];
                result += texture(sceneTexture, TexCoords - vec2(0.0, texOffset.y * i)).rgb * weight[i];
            }
        }

        FragColor = vec4(result, 1.0);
    } else if (pass == 2) {
        // Bloom combination pass
        vec3 sceneColor = texture(sceneTexture, TexCoords).rgb;
        vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;
        FragColor = vec4(sceneColor + bloomColor * bloomStrength, 1.0); // Additive blending
    }
}
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sunTexture;
uniform vec3 sunColor;
uniform float sunIntensity;

void main() {
    vec4 texColor = texture(sunTexture, TexCoords);
    FragColor = vec4(sunColor * sunIntensity, texColor.a); // Kolor słońca z intensywnością
}
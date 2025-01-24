#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float threshold;

void main()
{
    vec3 hdrColor = texture(hdrTexture, TexCoords).rgb;
    float luminance = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722)); // Luminancja
    FragColor = (luminance > threshold) ? hdrColor : vec3(0.0);
}
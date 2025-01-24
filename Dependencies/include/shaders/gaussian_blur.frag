#version 330 core
out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D srcTexture;
uniform bool horizontal;
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texelSize = 1.0 / textureSize(srcTexture, 0);
    vec3 result = texture(srcTexture, TexCoords).rgb * weight[0];

    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(srcTexture, TexCoords + vec2(texelSize.x * i, 0.0)).rgb * weight[i];
            result += texture(srcTexture, TexCoords - vec2(texelSize.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(srcTexture, TexCoords + vec2(0.0, texelSize.y * i)).rgb * weight[i];
            result += texture(srcTexture, TexCoords - vec2(0.0, texelSize.y * i)).rgb * weight[i];
        }
    }

    FragColor = result;
}
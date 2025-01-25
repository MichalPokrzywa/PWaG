#pragma once
#include <vector>

#include "shaders/ShaderProgram.h"
#include "textures/Texture.h"

class TextureShader : public ShaderProgram
{
protected:
    int location_projectionMatrix;
    int location_viewMatrix;
    int location_color;
    int location_light;
    int location_shadowMap;
    int location_opacity;
    int location_receiveShadow;
    int location_prevPVM;
    int location_textures;
    int location_textureCount;
    int location_blendFactors;
	void bindAttributes();
	void getAllUniformLocations();
    void loadTextures(const std::vector<Texture>& textures);
public:
	TextureShader();

	void render();
    void loadTextures(const std::vector<Texture>& textures, const std::vector<float>& blendFactors);
};


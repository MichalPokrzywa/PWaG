#ifndef SUNSHADER_H
#define SUNSHADER_H

#include "ShaderProgram.h"
#include "../include/textures/Texture.h"
#include <glm/glm.hpp>

class SunShader : public ShaderProgram {
private:
    int location_sunColor;
    int location_sunIntensity;
    int location_textures;
public:
    SunShader();
    ~SunShader();

    void bindAttributes() override;
    void getAllUniformLocations() override;

    void render();

};

#endif
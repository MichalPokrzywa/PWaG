#ifndef SUNSHADER_H
#define SUNSHADER_H

#include "ShaderProgram.h"
#include "../include/textures/Texture.h"
#include <glm/glm.hpp>

class SunShader : public ShaderProgram {
public:
    SunShader();
    ~SunShader();

    void bindAttributes() override;
    void getAllUniformLocations() override;

    void loadSunColor(const glm::vec3& color);
    void loadSunIntensity(float intensity);

private:
    int location_sunColor;
    int location_sunIntensity;
};

#endif
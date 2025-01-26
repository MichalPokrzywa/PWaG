#pragma once
#include "../include/shaders/ShaderProgram.h"
#include "../include//textures/Texture.h"
#include <glm/glm.hpp>
#include <vector>
class BloomShader : public ShaderProgram {
public:
    BloomShader();
    ~BloomShader();

    void init(unsigned int screenWidth, unsigned int screenHeight);
    void renderBloomEffect(unsigned int hdrTexture, unsigned int screenWidth, unsigned int screenHeight);
    unsigned int getBloomTexture() const;

private:
    void createFramebuffers(unsigned int screenWidth, unsigned int screenHeight);
    void createTextures(unsigned int screenWidth, unsigned int screenHeight);
    void bindAttributes() override;
    void getAllUniformLocations() override;
    void renderQuad();

    unsigned int fboID;
    unsigned int pingpongFBO[2];
    unsigned int colorBuffers[2];
    unsigned int pingpongColorBuffers[2];
    unsigned int rboDepth;

    unsigned int location_threshold;
    unsigned int location_bloomStrength;
    unsigned int location_horizontal;
    unsigned int location_sceneTexture;
    unsigned int location_bloomTexture;
    unsigned int location_pass;

    Texture bloomTexture;
};


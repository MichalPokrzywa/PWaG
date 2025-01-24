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

    unsigned int fboID; // Framebuffer dla ekstrakcji jasnych obszarów
    unsigned int pingpongFBO[2]; // Framebuffery do rozmycia Gaussa (ping-pong)
    unsigned int colorBuffers[2]; // Tekstury dla HDR i jasnych obszarów
    unsigned int pingpongColorBuffers[2]; // Tekstury do rozmycia Gaussa

    Texture bloomTexture; // Tekstura z efektem Bloom

    int location_threshold; // Lokacja uniforma dla progu jasnoœci
    int location_bloomStrength; // Lokacja uniforma dla si³y efektu Bloom
    int location_horizontal; // Lokacja uniforma dla kierunku rozmycia
};


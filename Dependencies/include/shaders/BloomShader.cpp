#include "BloomShader.h"
#include "glPrerequisites.h"
#include <iostream>

BloomShader::BloomShader() {
    const char* VERTEX_FILE = "./Dependencies/include/shaders/bloom_combine.vert";
    const char* FRAGMENT_FILE = "./Dependencies/include/shaders/bloom_combine.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

BloomShader::~BloomShader() {
    glDeleteFramebuffers(1, &fboID);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, colorBuffers);
    glDeleteTextures(2, pingpongColorBuffers);
}

void BloomShader::init(unsigned int screenWidth, unsigned int screenHeight) {
    createFramebuffers(screenWidth, screenHeight);
    createTextures(screenWidth, screenHeight);
    bindAttributes();
    getAllUniformLocations();
}

void BloomShader::createFramebuffers(unsigned int screenWidth, unsigned int screenHeight) {
    // G³ówny framebuffer dla ekstrakcji jasnych obszarów
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    // Tworzenie dwóch kolorowych buforów (HDR i jasne obszary)
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    // Bufor g³êbokoœci
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // Okreœlenie, które bufory maj¹ byæ u¿ywane do rysowania
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // Sprawdzenie, czy framebuffer jest kompletny
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer nie jest kompletny!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping-pong framebuffers dla rozmycia Gaussa
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "Ping-pong framebuffer nie jest kompletny!" << std::endl;
    }
}

void BloomShader::createTextures(unsigned int screenWidth, unsigned int screenHeight) {
    bloomTexture.setTextureID(colorBuffers[1]); // Ustawienie tekstury Bloom na drugi kolorowy bufor
}

void BloomShader::bindAttributes() {
    bindAttribute(0, "position"); // Powi¹zanie atrybutu pozycji
}

void BloomShader::getAllUniformLocations() {
    location_threshold = getUniformLocation("threshold");
    location_bloomStrength = getUniformLocation("bloomStrength");
    location_horizontal = getUniformLocation("horizontal");
}

void BloomShader::renderBloomEffect(unsigned int hdrTexture, unsigned int screenWidth, unsigned int screenHeight) {
    // 1. Ekstrakcja jasnych obszarów
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    start();
    loadFloat(location_threshold, 1.0f); // Ustawienie progu jasnoœci
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    // ... renderuj kwadrat pe³noekranowy ...

    // 2. Rozmycie Gaussa
    bool horizontal = true;
    bool first_iteration = true;
    for (unsigned int i = 0; i < 10; i++) { // 10 iteracji rozmycia
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        loadBool(location_horizontal, horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
        // ... renderuj kwadrat pe³noekranowy ...
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    // 3. £¹czenie wyników
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    loadFloat(location_bloomStrength, 0.04f); // Ustawienie si³y efektu Bloom
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
    // ... renderuj kwadrat pe³noekranowy ...

    stop();
}

unsigned int BloomShader::getBloomTexture() const {
    return bloomTexture.getID();
}
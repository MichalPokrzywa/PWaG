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
    glDeleteRenderbuffers(1, &rboDepth);
}

void BloomShader::init(unsigned int screenWidth, unsigned int screenHeight) {
    createFramebuffers(screenWidth, screenHeight);
    createTextures(screenWidth, screenHeight);
    bindAttributes();
    getAllUniformLocations();
}

void BloomShader::createFramebuffers(unsigned int screenWidth, unsigned int screenHeight) {
    // Main framebuffer for bright area extraction
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    // Create two color buffers (HDR and bright areas)
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

    // Depth buffer
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // Specify which color buffers to use for rendering
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping-pong framebuffers for Gaussian blur
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
            std::cerr << "Ping-pong framebuffer is not complete!" << std::endl;
    }
}

void BloomShader::createTextures(unsigned int screenWidth, unsigned int screenHeight) {
    bloomTexture.setTextureID(colorBuffers[1]); // Set Bloom texture to the second color buffer
}

void BloomShader::bindAttributes() {
    bindAttribute(0, "position"); // Bind position attribute
}

void BloomShader::getAllUniformLocations() {
    location_threshold = getUniformLocation("threshold");
    location_bloomStrength = getUniformLocation("bloomStrength");
    location_horizontal = getUniformLocation("horizontal");
    location_pass = getUniformLocation("pass");
    location_sceneTexture = getUniformLocation("sceneTexture");
    location_bloomTexture = getUniformLocation("bloomTexture");
}

void BloomShader::renderBloomEffect(unsigned int hdrTexture, unsigned int screenWidth, unsigned int screenHeight) {
    // 1. Brightness extraction pass
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    start();
    loadInt(location_pass, 0); // Set pass to brightness extraction
    loadFloat(location_threshold, 1.f); // Set brightness threshold
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    renderQuad();

    // 2. Gaussian blur pass
    bool horizontal = true;
    bool first_iteration = true;
    for (unsigned int i = 0; i < 10; i++) { // 10 blur iterations
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        loadInt(location_pass, 1); // Set pass to Gaussian blur
        loadBool(location_horizontal, horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorBuffers[!horizontal]);
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    // 3. Bloom combination pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    loadInt(location_pass, 2); // Set pass to bloom combination
    loadFloat(location_bloomStrength, 0.2f); // Set bloom strength
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
    renderQuad();

    stop();
}

void BloomShader::renderQuad() {
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    if (quadVAO == 0) {
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

unsigned int BloomShader::getBloomTexture() const {
    return bloomTexture.getID();
}
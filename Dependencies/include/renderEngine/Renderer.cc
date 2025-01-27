// Renderer.cc
#include "Renderer.h"
#include "glPrerequisites.h"
#include <common.h>
#include <./gameEngine/Game.h>
#include <entities/Entity.h>
#include <GLFW/glfw3.h>
#include <iostream>
using std::cout;

Renderer::Renderer(): seaShadowShader(true) {
  ShadowShader::init();
  bloomShader.init(ACTUAL_WIDTH, ACTUAL_HEIGHT);
  // generate buffer
  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

  // color
  glGenTextures(1, &colorTexture);
  glBindTexture(GL_TEXTURE_2D, colorTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ACTUAL_WIDTH, ACTUAL_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
  // velocity
  glGenTextures(1, &velocityTexture);
  glBindTexture(GL_TEXTURE_2D, velocityTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ACTUAL_WIDTH, ACTUAL_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, velocityTexture, 0);
  // draw to color attachment
  unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);

  // depth
  glGenRenderbuffers(1, &depthBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, ACTUAL_WIDTH, ACTUAL_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

Renderer::~Renderer() {}

void Renderer::render() {
    // 1. Render shadow map
    glViewport(0, 0, SHADOW::WIDTH, SHADOW::HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowShader::getFboID());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    seaShadowShader.render();
    entityShadowShader.render();
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Render scene to HDR framebuffer
    glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ShadowShader::getDepthMap().getID());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render background (e.g., sky)
    backgroundShader.render();
    // Render the rest of the scene
    entityShader.render();
    textureShader.render();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ShadowShader::getDepthMap().getID());
    seaShader.render();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Apply bloom effect
    glViewport(0, 0, ACTUAL_WIDTH, ACTUAL_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

    // 4. Apply motion blur
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture); // Use the bloomed texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, velocityTexture);
    if (Game::wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        bloomShader.renderBloomEffect(colorTexture, ACTUAL_WIDTH, ACTUAL_HEIGHT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        bloomShader.renderBloomEffect(colorTexture, ACTUAL_WIDTH, ACTUAL_HEIGHT);
    }

    // 5. Render UI
    uiShader.render();

    // 6. Update entity velocities
    entityShader.updateEntityVelocity();
}
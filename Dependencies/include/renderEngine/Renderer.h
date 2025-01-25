// Renderer.h
#pragma once
#include <shaders/BackgroundShader.h>
#include <shaders/UIShader.h>
#include <shaders/EntityShader.h>
#include <shaders/SeaShader.h>
#include <shaders/ShadowShader.h>
#include <shaders/MotionBlurShader.h>
#include <../../BloomShader.h>

#include <shaders/TextureShader.h>

class Renderer {
private:
  BackgroundShader backgroundShader;
  UIShader uiShader;
  EntityShader entityShader;
  TextureShader textureShader;
  SeaShader seaShader;
  ShadowShader seaShadowShader;
  ShadowShader entityShadowShader;
  MotionBlurShader motionBlurShader;
  BloomShader bloomShader;
  

  unsigned int frameBuffer;
  unsigned int colorTexture, velocityTexture;
  unsigned int depthBuffer;
public:
  Renderer();
  ~Renderer();

  void render();
};

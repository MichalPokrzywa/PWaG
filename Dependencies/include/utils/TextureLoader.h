#pragma once
#include "textures/Texture.h"
#include <string>

class TextureLoader {
public:
	static Texture loadTexture(const std::string& filePath);
};


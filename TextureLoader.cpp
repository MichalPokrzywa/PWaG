#define STB_IMAGE_IMPLEMENTATION
#include "TextureLoader.h"

#include <filesystem>
#include <utils/stb_image.h>
#include <iostream>

Texture TextureLoader::loadTexture(const std::string& filePath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "ERROR: Failed to load texture: " << filePath << std::endl;
        return Texture();
    }

    if (width <= 0 || height <= 0) {
        std::cerr << "ERROR: Invalid texture dimensions: " << width << "x" << height << std::endl;
        stbi_image_free(data);
        return Texture();
    }

    GLenum format = GL_RGB; // Default to RGB
    if (channels == 4) {
        format = GL_RGBA; // Use RGBA if the image has an alpha channel
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);


    stbi_image_free(data); // Free the image data after uploading to GPU

    return Texture(textureID, width * height, GL_TEXTURE_2D);
}
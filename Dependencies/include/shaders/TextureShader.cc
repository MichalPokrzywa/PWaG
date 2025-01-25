#include "glPrerequisites.h"
#include <common.h>
#include <entities/Entity.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Camera.h>
#include <iostream>
#include <shaders/TextureShader.h>
using std::vector;

TextureShader::TextureShader()
{
    const char* VERTEX_FILE = "Dependencies/include/shaders/texturedEntity.vert";
    const char* FRAGMENT_FILE = "Dependencies/include/shaders/texturedEntity.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

void TextureShader::bindAttributes()
{
    bindAttribute(0, "position");
    bindAttribute(1, "uv");
    bindAttribute(2, "normal");
}

void TextureShader::getAllUniformLocations()
{
    ShaderProgram::getAllUniformLocations();
    location_projectionMatrix = getUniformLocation("projectionMatrix");
    location_viewMatrix = getUniformLocation("viewMatrix");
    location_color = getUniformLocation("color");
    location_light = getUniformLocation("lightPos");
    location_shadowMap = getUniformLocation("shadowMap");
    location_opacity = getUniformLocation("opacity");
    location_receiveShadow = getUniformLocation("receiveShadow");
    location_prevPVM = getUniformLocation("prevPVM");
    location_textures = getUniformLocation("textures[0]"); // Location of the first texture sampler
    location_textureCount = getUniformLocation("textureCount"); // Location of textureCount uniform
    location_blendFactors = getUniformLocation("blendFactors[0]"); // Location of the first blend factor
}

void TextureShader::loadTextures(const std::vector<Texture>& textures, const std::vector<float>& blendFactors)
{
    // Set the number of textures
    loadInt(location_textureCount, static_cast<int>(textures.size()));

    // Bind texture units and set blend factors
    for (size_t i = 0; i < textures.size(); ++i) {
        // Bind texture unit to sampler
        loadInt(location_textures + i, static_cast<int>(i));

        // Set blend factor for the texture
        std::string blendFactorName = "blendFactors[" + std::to_string(i) + "]";
        int blendFactorLocation = getUniformLocation(blendFactorName.c_str());
        loadFloat(blendFactorLocation, blendFactors[i]);
    }
}

void TextureShader::render()
{
    start();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    glm::vec3 lightPos(Light::theOne().getPosition());
    loadInt(location_shadowMap, 0);
    loadVector3f(location_light, lightPos);
    loadMatrix4f(location_lightSpaceMatrix, Camera::primary().getLightSpaceMatrix());
    loadMatrix4f(location_viewMatrix, Camera::primary().getViewMatrix());
    loadMatrix4f(location_projectionMatrix, Camera::primary().getProjectionMatrix());

    glm::mat4 PV = Camera::primary().getPVMatrix();
    for (auto& entry : texturedEntities) {
        vector<Entity*>& entities = entry.second;
        entry.first->bind();
        for (int i = 0; i < entities.size(); ++i) {
            Entity* entity = entities.at(i);
            RawModel* model = entity->getModel();
            loadBool(location_receiveShadow, entity->getReceiveShadow());
            loadFloat(location_opacity, entity->getOpacity());
            loadVector3f(location_color, entity->getColor());
            loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());
            loadMatrix4f(location_prevPVM, PV * entity->getPrevTransformationMatrix());

            // Load textures for the entity
            if (Entity* texturedEntity = entity) {
                const std::vector<Texture>& textures = texturedEntity->getTextures();
                const std::vector<float>& blendFactors = texturedEntity->getBlendFactors(); // Assuming Entity has a getBlendFactors method
                loadTextures(textures, blendFactors);

                // Bind textures to texture units
                for (size_t j = 0; j < textures.size(); ++j) {
                    glActiveTexture(GL_TEXTURE0 + j); // Activate texture unit
                    glBindTexture(GL_TEXTURE_2D, textures[j].getID()); // Bind texture
                }
            }

            glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        }
        RawModel::unbind();
    }
    stop();
}
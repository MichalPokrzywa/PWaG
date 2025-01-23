// EntityShader.cc
#include "EntityShader.h"
#include "glPrerequisites.h"
#include <common.h>
#include <entities/Entity.h>
#include <entities/DynamicEntity.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Camera.h>
#include <entities/gameObjects/ParticleHolder.h>
#include <iostream>
using std::cout;
using std::vector;

EntityShader::EntityShader() {
  const char* VERTEX_FILE = "Dependencies/include/shaders/entity.vert";
  const char* FRAGMENT_FILE = "Dependencies/include/shaders/entity.frag";
  ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

void EntityShader::bindAttributes() {
  bindAttribute(0, "position");
  bindAttribute(1, "normal");
}

void EntityShader::getAllUniformLocations() {
  ShaderProgram::getAllUniformLocations();
  location_projectionMatrix = getUniformLocation("projectionMatrix");
  location_viewMatrix = getUniformLocation("viewMatrix");
  location_color = getUniformLocation("color");
  location_light = getUniformLocation("lightPos");
  location_shadowMap = getUniformLocation("shadowMap");
  location_opacity = getUniformLocation("opacity");
  location_receiveShadow = getUniformLocation("receiveShadow");
  location_prevPVM = getUniformLocation("prevPVM");
  location_textureSampler = getUniformLocation("textureSampler");
  location_hasTexture = getUniformLocation("hasTexture"); // Add this line
}

void EntityShader::render() {
    start();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    glm::vec3 lightPos(Light::theOne().getPosition());
    loadInt(location_shadowMap, 0);
    loadFloat(location_ambientLightIntensity, AMBIENT_LIGHT_INTENSITY);
    loadVector3f(location_light, lightPos);
    loadMatrix4f(location_lightSpaceMatrix, Camera::primary().getLightSpaceMatrix());
    loadMatrix4f(location_viewMatrix, Camera::primary().getViewMatrix());
    loadMatrix4f(location_projectionMatrix, Camera::primary().getProjectionMatrix());

    // Check if any entity has a texture
    bool hasTexture = false;
    for (auto& entry : staticEntities) {
        vector<Entity*>& entities = entry.second;
        for (int i = 0; i < entities.size(); ++i) {
            if (entities.at(i)->getTexture().getID() != 0) {
                hasTexture = true;
                break;
            }
        }
        if (hasTexture) break;
    }

    // Check dynamic entities for textures
    for (auto& entry : dynamicEntities) {
        vector<DynamicEntity*>& entities = entry.second;
        for (int i = 0; i < entities.size(); ++i) {
            if (entities.at(i)->getTexture().getID() != 0) {
                hasTexture = true;
                break;
            }
        }
        if (hasTexture) break;
    }

    // Check particles for textures
    vector<DynamicEntity*>& particles = ParticleHolder::getParticles();
    for (int i = 0; i < particles.size(); ++i) {
        if (particles.at(i)->getTexture().getID() != 0) {
            hasTexture = true;
            break;
        }
    }

    loadBool(location_hasTexture, hasTexture); // Pass hasTexture to the shader
    if (hasTexture) {
        loadInt(location_textureSampler, 1); // Bind texture to texture unit 1
    }

    glm::mat4 PV = Camera::primary().getPVMatrix();

    // Render static entities
    for (auto& entry : staticEntities) {
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

            // Bind the entity's texture if it exists
            if (entity->getTexture().getID() != 0) {
                std::cout << "Binding texture ID: " << entity->getTexture().getID() << " to unit 1" << std::endl;
                entity->getTexture().bindToUint(2); // Bind to texture unit 1
            }

            glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        }
        RawModel::unbind();
    }

    // Render dynamic entities
    for (auto& entry : dynamicEntities) {
        vector<DynamicEntity*>& entities = entry.second;
        entry.first->bind();
        for (int i = 0; i < entities.size(); ++i) {
            DynamicEntity* entity = entities.at(i);
            RawModel* model = entity->getModel();
            loadBool(location_receiveShadow, entity->getReceiveShadow());
            loadFloat(location_opacity, entity->getOpacity());
            loadVector3f(location_color, entity->getColor());
            loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());
            loadMatrix4f(location_prevPVM, PV * entity->getPrevTransformationMatrix());

            // Bind the entity's texture if it exists
            if (entity->getTexture().getID() != 0) {
                std::cout << "Binding texture ID: " << entity->getTexture().getID() << " to unit 1" << std::endl;
                entity->getTexture().bindToUint(1); // Bind to texture unit 1
            }

            glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        }
        RawModel::unbind();
    }

    // Render particles
    particles = ParticleHolder::getParticles();
    if (particles.size()) {
        particles[0]->getModel()->bind();
        for (int i = 0; i < particles.size(); ++i) {
            DynamicEntity* particle = particles.at(i);
            RawModel* model = particle->getModel();
            loadBool(location_receiveShadow, particle->getReceiveShadow());
            loadFloat(location_opacity, particle->getOpacity());
            loadVector3f(location_color, particle->getColor());
            loadMatrix4f(location_transformationMatrix, particle->getTransformationMatrix());
            loadMatrix4f(location_prevPVM, PV * particle->getPrevTransformationMatrix());

            // Bind the particle's texture if it exists
            if (particle->getTexture().getID() != 0) {
                std::cout << "Binding texture ID: " << particle->getTexture().getID() << " to unit 1" << std::endl;
                particle->getTexture().bindToUint(1); // Bind to texture unit 1
            }

            glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        }
        RawModel::unbind();
    }

    stop();
}

void EntityShader::updateEntityVelocity() {
  for (auto& entry: staticEntities) {
    vector<Entity*>& entities = entry.second;
    for (int i = 0; i < entities.size(); ++i) {
      entities.at(i)->updatePrevTransformation();
    }
  }
  for (auto& entry: dynamicEntities) {
    vector<DynamicEntity*>& entities = entry.second;
    for (int i = 0; i < entities.size(); ++i) {
      entities.at(i)->updatePrevTransformation();
    }
  }

  vector<DynamicEntity*>& particles = ParticleHolder::getParticles();
  for (int i = 0; i < particles.size(); ++i) {
    particles.at(i)->updatePrevTransformation();
  }
}

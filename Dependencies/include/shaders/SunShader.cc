#include "SunShader.h"
#include "glPrerequisites.h"
#include "entities/Entity.h"
#include "models/RawModel.h"

SunShader::SunShader() {
    const char* VERTEX_FILE = "Dependencies/include/shaders/sun.vert";
    const char* FRAGMENT_FILE = "Dependencies/include/shaders/sun.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

SunShader::~SunShader() {
    
}

void SunShader::bindAttributes() {
    bindAttribute(0, "position"); // Powi¹zanie atrybutu pozycji
    bindAttribute(1, "texCoords"); // Powi¹zanie atrybutu wspó³rzêdnych tekstury
}

void SunShader::getAllUniformLocations() {
    ShaderProgram::getAllUniformLocations();
    location_sunColor = getUniformLocation("sunColor");
    location_sunIntensity = getUniformLocation("sunIntensity");
    location_textures = getUniformLocation("sunTexture");
}

void SunShader::render()
{
    start();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    for (auto& entry : texturedEntities)
    {
	    std::vector<Entity*>& entities = entry.second;
        entry.first->bind();
        for (int i = 0; i < entities.size(); ++i) {
            Entity* entity = entities.at(i);
            RawModel* model = entity->getModel();
            loadFloat(location_sunIntensity, entity->getOpacity());
            loadVector3f(location_sunColor, entity->getColor());
            loadMatrix4f(location_transformationMatrix, entity->getTransformationMatrix());

            // Load textures for the entity
            if (Entity* texturedEntity = entity) {
                const std::vector<Texture>& textures = texturedEntity->getTextures();
                if(!texturedEntity->getCastShadow())
                {
                    loadInt(location_textures, 0);
                    glActiveTexture(GL_TEXTURE0); // Activate texture unit
                    glBindTexture(GL_TEXTURE_2D, textures[0].getID()); // Bind texture
                }
            }

            glDrawArrays(GL_TRIANGLES, 0, model->getVertexCount());
        }

		RawModel::unbind();

    }
    stop();
}

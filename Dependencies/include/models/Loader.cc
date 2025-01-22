// Loader.cc
#include "Loader.h"
#include "glPrerequisites.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/vec2.hpp>
using std::vector;

vector<unsigned int> Loader::vaos;
vector<unsigned int> Loader::vbos;


RawModel* Loader::loadFromOBJ(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open OBJ file " << filePath << "\n";
        return nullptr;
    }

    // Temporary storage for raw data from OBJ file
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUVs;
    std::vector<glm::vec3> tempNormals;

    // Final buffers for OpenGL
    std::vector<float> finalVertices;
    std::vector<float> finalTextures;
    std::vector<float> finalNormals;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") { // Vertex position
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        }
        else if (prefix == "vt") { // Texture coordinate
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            uv.y = 1.0f - uv.y; // Flip Y-axis for OpenGL
            tempUVs.push_back(uv);
        }
        else if (prefix == "vn") { // Normal
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (prefix == "f") { // Face
            std::vector<unsigned int> vIndex(4), tIndex(4), nIndex(4);
            char slash;
            int i = 0;

            // Parse up to 4 indices (quad support)
            while (iss >> vIndex[i] >> slash >> tIndex[i] >> slash >> nIndex[i]) {
                vIndex[i]--;
                tIndex[i]--;
                nIndex[i]--;
                i++;
                if (i >= 4) break; // Stop at 4 vertices for quads
            }

            if (i == 3) {
                // Triangle face
                for (int j = 0; j < 3; j++) {
                    glm::vec3 vertex = tempVertices[vIndex[j]];
                    glm::vec2 uv = tempUVs[tIndex[j]];
                    glm::vec3 normal = tempNormals[nIndex[j]];

                    finalVertices.push_back(vertex.x);
                    finalVertices.push_back(vertex.y);
                    finalVertices.push_back(vertex.z);

                    finalTextures.push_back(uv.x);
                    finalTextures.push_back(uv.y);

                    finalNormals.push_back(normal.x);
                    finalNormals.push_back(normal.y);
                    finalNormals.push_back(normal.z);

                    indices.push_back(finalVertices.size() / 3 - 1);
                }
            }
            else if (i == 4) {
                // Quad face, split into two triangles: (v0, v1, v2) and (v0, v2, v3)
                unsigned int tri1[] = { 0, 1, 2 };
                unsigned int tri2[] = { 0, 2, 3 };

                for (unsigned int t = 0; t < 2; t++) {
                    unsigned int* tri = (t == 0) ? tri1 : tri2;
                    for (int j = 0; j < 3; j++) {
                        glm::vec3 vertex = tempVertices[vIndex[tri[j]]];
                        glm::vec2 uv = tempUVs[tIndex[tri[j]]];
                        glm::vec3 normal = tempNormals[nIndex[tri[j]]];

                        finalVertices.push_back(vertex.x);
                        finalVertices.push_back(vertex.y);
                        finalVertices.push_back(vertex.z);

                        finalTextures.push_back(uv.x);
                        finalTextures.push_back(uv.y);

                        finalNormals.push_back(normal.x);
                        finalNormals.push_back(normal.y);
                        finalNormals.push_back(normal.z);

                        indices.push_back(finalVertices.size() / 3 - 1);
                    }
                }
            }
            else {
                std::cerr << "Error: Unsupported face format with " << i << " vertices\n";
            }
        }
    }
    file.close();

    // OpenGL Buffer setup
    unsigned int vaoID = createVAO();
    bindIndicesBuffer(indices);
    storeDataInAttributeList(0, 3, finalVertices); // Position
    storeDataInAttributeList(1, 2, finalTextures); // Texture coordinates
    storeDataInAttributeList(2, 3, finalNormals);  // Normals
    glBindVertexArray(0);

    return new RawModel(vaoID, indices.size());
}


RawModel* Loader::loadToVAO(vector<float>& data1, int data1Dimension, vector<float>& data2, int data2Dimension, vector<unsigned int>& indices) {
  unsigned int vaoID = createVAO();
  bindIndicesBuffer(indices);
  storeDataInAttributeList(0, data1Dimension, data1);
  storeDataInAttributeList(1, data2Dimension, data2);
  return new RawModel(vaoID, indices.size());
}

RawModel* Loader::loadToVAO(vector<float>& data1, int data1Dimension, vector<float>& data2, int data2Dimension) {
  unsigned int vaoID = createVAO();
  storeDataInAttributeList(0, data1Dimension, data1);
  storeDataInAttributeList(1, data2Dimension, data2);
  return new RawModel(vaoID, data1.size());
}

RawModel* Loader::loadToVAO(vector<float>& data, int dimension) {
  unsigned int vaoID = createVAO();
  storeDataInAttributeList(0, dimension, data);
  return new RawModel(vaoID, data.size(), 1);
}

void Loader::clean() {
  for (int i = 0; i < vaos.size(); ++i) {
    glDeleteVertexArrays(1, &vaos[i]);
  }
  for (int i = 0; i < vaos.size(); ++i) {
    glDeleteBuffers(1, &vbos[i]);
  }
}

void Loader::unbindVAO() {
    glBindVertexArray(0);
}
unsigned int Loader::createVAO() {
  unsigned int vaoID;
  glGenVertexArrays(1, &vaoID);
  vaos.push_back(vaoID);
  glBindVertexArray(vaoID);
  return vaoID;
}

void Loader::storeDataInAttributeList(unsigned int attrubuteNumber, int coordinateSize, vector<float>& data) {
    unsigned int vboID;
    glGenBuffers(1, &vboID); // Generate a VBO
    vbos.push_back(vboID); // Keep track of VBO for cleanup later

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    // Link the VBO to the VAO's attribute list
    glVertexAttribPointer(attrubuteNumber, coordinateSize, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attrubuteNumber);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the buffer
}

void Loader::bindIndicesBuffer(vector<unsigned int>& indices) {
    unsigned int iboID;
    glGenBuffers(1, &iboID); // Generate an IBO
    vbos.push_back(iboID);   // Track IBO for cleanup later

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

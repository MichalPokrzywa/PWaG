// RawModel.h
#pragma once
#include <string>
#include <vector>
class RawModel {
private:
  unsigned int vaoID;
  unsigned int vertexCount;
  int vbos;
  // Struktury danych dla wierzcho³ków, normalnych i UV
  struct Vec3 {
      float x, y, z;
  };
  struct Vec2 {
      float u, v;
  };
  // Vertex data containers
  std::vector<Vec3> vertices;
  std::vector<Vec3> normals;
  std::vector<Vec2> texCoords; // Add texture coordinates
public:
  RawModel(unsigned int vaoID, unsigned int vertexCount, int vbos = 2);

  unsigned int getVaoID() const;
  unsigned int getVertexCount() const;

  void bind();
  static void unbind(int vbos = 2);
};

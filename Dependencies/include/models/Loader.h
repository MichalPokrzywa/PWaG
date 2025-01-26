// Loader.h
#pragma once
#include "RawModel.h"
#include <vector>
#include <glm/vec3.hpp>
using std::vector;

class Loader {
private:
  static vector<unsigned int> vaos;
  static vector<unsigned int> vbos;

  static unsigned int createVAO();
  static void storeDataInAttributeList(unsigned int attrubuteNumber, int coordinateSize, vector<float>& data);
  static void bindIndicesBuffer(vector<unsigned int>& indices);
  void unbindVAO();
public:
  static void clean();
  static RawModel* loadFromOBJ(const std::string& filePath);
  static RawModel* loadToVAO(vector<float>& data1, int data1Dimension, vector<float>& data2, int data2Dimension, vector<unsigned int>& indices);
  static RawModel* loadToVAO(vector<float>& data1, int data1Dimension, vector<float>& data2, int data2Dimension, vector<float>& data3, int data3Dimension);
  static RawModel* loadToVAO(vector<float>& data1, int data1Dimension, vector<float>& data2, int data2Dimension);
  static RawModel* loadToVAO(vector<float>& data, int dimension);
};


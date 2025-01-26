// Geometry.h
#pragma once
#include "RawModel.h"
#include <glm/glm.hpp>
#include <string>

namespace Geometry {
  // models
  extern RawModel* tetrahedron;
  extern RawModel* cube;
  extern RawModel* sphere;
  extern RawModel* sun;
  extern RawModel* sea;
  extern RawModel* cockpit;
  extern RawModel* propeller;
  extern RawModel* quad;

  // Inicjalizuje wszystkie modele geometryczne
  void initGeometry();

  // Czyœci wszystkie za³adowane modele
  void cleanGeometry();

  // £aduje model OBJ i zwraca model RawModel
  bool loadModelFromOBJ(const std::string& filepath, RawModel*& outModel);
};

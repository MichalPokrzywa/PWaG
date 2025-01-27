// Light.cc
#include "Light.h"
#include <models/Geometry.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Light::Light() {}

Light::~Light() {}

void Light::setPosition(float x, float y, float z) {
  position.x = x;
  position.y = y;
  position.z = z;
}

const glm::vec3& Light::getPosition() {
  return position;
}

void Light::update(glm::vec3& pos) {

    // Przesunięcie światła względem samolotu
    glm::vec3 offset(90.0f, 210.0f, 210.0f); // Dostosuj te wartości według potrzeb
    glm::vec3 newPosition = offset;

    // Zaktualizuj pozycję światła
    position = newPosition;
    Light::singleton().setPosition(position.x, position.y, position.z);
}

Light& Light::singleton() {
  static Light light;
  return light;
}

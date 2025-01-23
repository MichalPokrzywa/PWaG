// Light.cc
#include "Light.h"
#include <models/Geometry.h>
#include <glm/gtc/matrix_transform.hpp>

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
	Light::theOne().setPosition(pos.x  , pos.y + 10, pos.z );
}

Light& Light::theOne() {
  static Light light;
  return light;
}

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

void Light::update() {
	glm::mat4 t = glm::mat4(1.0f);      // Macierz jednostkowa
	glm::mat4 t_1 = glm::mat4(1.0f);    // Macierz jednostkowa
	glm::mat4 r = glm::mat4(1.0f);      // Macierz jednostkowa

	glm::vec3 axis(0.0f, position.y, 0.0f);
	t = glm::translate(t, -axis);       // Translacja wstecz
	r = glm::rotate(r, 0.003f, axis);   // Rotacja
	t_1 = glm::translate(t_1, axis);    // Translacja w przód

	// Poprawione mno¿enie: macierz * wektor
	glm::vec4 newPosition = t_1 * r * t * glm::vec4(position, 1.0f);

	// Zaktualizuj pozycjê œwiat³a
	position = glm::vec3(newPosition);
	Light::theOne().setPosition(position.x, position.y, position.z);
}

Light& Light::theOne() {
  static Light light;
  return light;
}

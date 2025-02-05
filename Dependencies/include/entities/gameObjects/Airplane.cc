// Airplane.cc
#include "Airplane.h"
#include "Camera.h"
#include <common.h>
#include <maths/Object3D.h>
#include <maths/Maths.h>
#include <utils/Debug.h>
#include <io/MouseManager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>
#include <utils/TextureLoader.h>

using std::cout;
using std::vector;

float COLLISION_SPEED_X = 0;
float COLLISION_SPEED_Y = 0;
float COLLISION_DISPLACEMENT_X = 0;
float COLLISION_DISPLACEMENT_Y = 0;

const float PLANE_MIN_SPEED = 0.48f;
const float PLANE_MAX_SPEED = 0.64f;

glm::vec3 red(RED[0], RED[1], RED[2]);
glm::vec3 white(WHITE[0], WHITE[1], WHITE[2]);
glm::vec3 brown(BROWN[0], BROWN[1], BROWN[2]);
glm::vec3 brownDark(BROWNDARK[0], BROWNDARK[1], BROWNDARK[2]);
glm::vec3 pink(PINK[0], PINK[1], PINK[2]);

vector<Entity*> Airplane::rigidBody;

Airplane::Airplane() :
  speed(0.0f),
  axisX(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)),
  axisY(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)),
  axisZ(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)),
  cockpit(Geometry::cockpit, glm::vec3(-1.5f,-5.0f,0.0f), white,glm::vec3(1.f)),
  propeller(Geometry::propeller, glm::vec3(4.5f, 0.6f, 0.0f), brown, glm::vec3(2.0f, 1.0f, 1.0f)),
  blade1(Geometry::cube, glm::vec3(5.3f, 0.6f, 0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f)),
  blade2(Geometry::cube, glm::vec3(5.3f, 0.6f, 0.0f), brownDark, glm::vec3(0.1f, 8.0f, 1.0f)),
  body(Geometry::cube, glm::vec3(-0.8f, 1.5f, 0.0f), brown, glm::vec3(1.5f)),
  face(Geometry::cube, glm::vec3(-1.0f, 2.7f, 0.0f), pink, glm::vec3(1.0f)),
  hairSide(Geometry::cube, glm::vec3(-1.3f, 3.0f, 0.0f), brown, glm::vec3(1.2f, 0.4f, 1.2f)),
  hairBack(Geometry::cube, glm::vec3(-1.6f, 2.8f, 0.0f), brown, glm::vec3(0.2f, 0.8f, 1.0f))
{
    Texture texture = TextureLoader::loadTexture("./Dependencies/include/textures/camo2.jpg");
    Texture texture2 = TextureLoader::loadTexture("./Dependencies/include/textures/pink.jpg");
	cockpit.addTexture(texture);
	cockpit.addTexture(texture2);
    cockpit.getBlendFactors().push_back(0.5f); // Blend factor for texture1
    cockpit.getBlendFactors().push_back(0.5f); // Blend factor for texture2
  components.push_back(&cockpit);
  components.push_back(&propeller);
  components.push_back(&blade1);
  components.push_back(&blade2);
  components.push_back(&body);
  components.push_back(&face);
  components.push_back(&hairSide);
  components.push_back(&hairBack);

  rigidBody.push_back(&cockpit);
  cockpit.setBody(new Sphere(5.0f));
  // create hair
  for (int i = 0; i < 12; ++i) {
    int col = i % 3;
    int row = i / 3;
    float startX = -1.9f;
    float startY = 3.2f;
    float startZ = -0.4f;
    hair[i] = Entity(Geometry::cube, glm::vec3(startX + (float)row * 0.4f, startY, startZ + (float)col * 0.4f), brown , glm::vec3(0.4f));
    components.push_back(&(hair[i]));
  }

  for (int i = 0; i < components.size(); ++i) {
    if(components[i]->getTextures().size() > 0)
    {
        Entity::addTextureEntity(components[i]);
    }
    else
    {
		Entity::addEntity(components[i]);
    }
  }

  suspension.changeRotation(glm::vec3(0.0f, 0.0f, 1.0f), -0.3f);
  blade2.changeRotation(glm::vec3(axisX), glm::radians(90.0f));
  translate(AIRPLANE::X, AIRPLANE::Y, AIRPLANE::Z);
}

Airplane::~Airplane() {}

void Airplane::rotate(float dx, float dy, float dz, glm::vec3 center) {
  rotation += glm::vec3(dx, dy, dz);
  float angle = dx != 0.0f ? dx : dy != 0.0f ? dy : dz;
  glm::mat4 rotationMatrix = Maths::calculateRotationMatrix(dx, dy, dz, center);
  axisX = rotationMatrix * axisX;
  axisY = rotationMatrix * axisY;
  axisZ = rotationMatrix * axisZ;
  for (int i = 0; i < components.size(); ++i) {
    components[i]->changeRotation(rotationMatrix);
  }
}

void Airplane::translate(float dx, float dy, float dz) {
  position += glm::vec3(dx, dy, dz);
  glm::mat4 translationMatrix = Maths::calculateTranslationMatrix(dx, dy, dz);
  for (int i = 0; i < components.size(); ++i) {
    components[i]->changePosition(translationMatrix);
  }
}

void Airplane::updateHair() {
  static float hairAngle = 0.0f;
  for (int i = 0; i < 12; ++i) {
    float height = 0.3f + glm::cos(hairAngle + i / 3) * 0.1f;
    float dy = (height - hair[i].getScale().y) / 2;
    glm::vec3 translateVector = dy * glm::normalize(glm::vec3(axisY.x, axisY.y, axisY.z));
    hair[i].changePosition(translateVector.x, translateVector.y, translateVector.z);
    hair[i].setScale(0.4f, height, 0.4f);
  }
  hairAngle += 0.16f;
}

void Airplane::update() {
  if (GAME::HEALTH <= 0.0f) {
    totalRotation += 0.3f;
    y += 0.01f;
    if (y <= 2.0f)
      translate(0.0f, -y, 0.0f);
    if (totalRotation < 80.0f)
      rotate(0.0f, 0.0f, -glm::radians(0.3f), position);
  } else {
    speed = Maths::clamp(MouseManager::getX(), -0.5f, 0.5f, PLANE_MIN_SPEED, PLANE_MAX_SPEED);
    float targetX = Maths::clamp(MouseManager::getX(), -1.0f, 1.0f, -AIRPLANE::AMPWIDTH, -0.7f * AIRPLANE::AMPWIDTH);
    float targetY = Maths::clamp(MouseManager::getY(), -0.75f, 0.75f, AIRPLANE::Y - AIRPLANE::AMPHEIGHT, AIRPLANE::Y + AIRPLANE::AMPHEIGHT);

    COLLISION_DISPLACEMENT_X += COLLISION_SPEED_X;
    targetX += COLLISION_DISPLACEMENT_X;

    float deltaX = targetX - position.x;
    float deltaY = targetY - position.y;
    translate(deltaX * AIRPLANE::MOVE_SENSITIVITY, deltaY * AIRPLANE::MOVE_SENSITIVITY, 0.0f);
    float targetRotationZ = deltaY * AIRPLANE::ROTATE_SENSITITY;

    rotate(0.0f, 0.0f, targetRotationZ - rotation.z, position);
    rotation.z = targetRotationZ;


    COLLISION_SPEED_X += -COLLISION_DISPLACEMENT_X * 0.2f;
    if (COLLISION_SPEED_X > 0)
      COLLISION_DISPLACEMENT_X = 0;
    COLLISION_DISPLACEMENT_X += -COLLISION_DISPLACEMENT_X * 0.1f;
  }

  // update hair
  updateHair();
  // update propeller
  blade1.changeRotation(glm::vec3(axisX), glm::radians(10.0f));
  blade2.changeRotation(glm::vec3(axisX), glm::radians(10.0f));
  propeller.changeRotation(glm::vec3(axisX), glm::radians(10.0f));
  // move camera
  Camera::primary().chasePoint(position);
} 

void Airplane::knockBack(glm::vec3 otherPosition) {
  glm::vec3 distance = position - otherPosition;
  float length = glm::length(distance);
  COLLISION_SPEED_X = 20.0f * distance.x / length;
  AMBIENT_LIGHT_INTENSITY = 2.0f;
}

void Airplane::resetFallState() {
    totalRotation = 0.0f;
    y = 0.0f;
}

glm::vec3& Airplane::getPosition() {
	return position = this->position;
}


Entity& Airplane::getBody() {
  return cockpit;
}

Airplane& Airplane::singleton() {
  static Airplane airplane;
  return airplane;
}

// Game.cc
#include "Game.h"
#include "Collision.h"
#include <GLFW/glfw3.h>
#include <common.h>
#include <maths/Maths.h>
#include <entities/Entity.h>
#include <entities/gameObjects/Light.h>
#include <entities/gameObjects/Sky.h>
#include <entities/gameObjects/Airplane.h>
#include <entities/gameObjects/ObstacleHolder.h>
#include <entities/gameObjects/BatteryHolder.h>
#include <entities/gameObjects/ParticleHolder.h>
#include <entities/gameObjects/Camera.h>
#include <models/Geometry.h>
#include <renderEngine/DisplayManager.h>
#include <io/MouseManager.h>
#include <io/Parser.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
using std::cout;

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

float GAME::AIRPLANE_DISTANCE = 0.0f;
float GAME::MILES = 0.0f;
float GAME::HEALTH = 100.0f; // 100.0f; pamiêtaj zmieniæ
float TIMER = 0;
bool Game::wireframeMode = false;
bool Game::flyMode = false; // Inicjalizacja flagi

/* Helper function declaration */
void updateFPSCount(double& previousSecond, int& updates);

Game::Game() {
  currentTime = 0;
  lastTime = DisplayManager::getTime();
  previousSecond = lastTime;
  delta = 0;
  updates = 0;
}

Game::~Game() {
  DisplayManager::cleanDisplay();
  Geometry::cleanGeometry();
}

void Game::init() {
  Parser::parse();
  DisplayManager::createDisplay();
  Geometry::initGeometry();
  Light::singleton().setPosition(LIGHT::X, LIGHT::Y, LIGHT::Z);
}

Game& Game::singleton() {
  static Game game;
  return game;
}

void Game::run() {
    if (shouldUpdate()) {
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Wype³niony
        }
        // temporary code for updating game angle
        GAME::AIRPLANE_DISTANCE += GAME::SPEED;

        MouseManager::update();
        Camera::primary().update();
        Light::singleton().update(Airplane::singleton().getPosition());
        DisplayManager::prepareDisplay();

        // check collision
        Collision::checkCollisionAgainstPlane();
        ParticleHolder::singleton().update();

        renderer.render();
        ObstacleHolder::singleton().update();
        BatteryHolder::singleton().update();
        Sky::singleton().update();
        Airplane::singleton().update();
        DisplayManager::updateDisplay();

        // update health
        GAME::HEALTH -= 0.025f;
        GAME::HEALTH = Maths::clamp(-0.1f, GAME::HEALTH, 100.0f);
        if (GAME::HEALTH <= 0.0f && !reset) {
            reset = true;
            std::thread(&Game::resetGame, this).detach();
        }

        ++updates;
    }

    if (GAME::DISPLAY_FPS)
      updateFPSCount(previousSecond, updates);
}

bool Game::shouldRun() {
  return !DisplayManager::shouldCloseDisplay();
}

bool Game::shouldUpdate() {
  currentTime = DisplayManager::getTime();
  delta += currentTime - lastTime;
  lastTime = currentTime;
  if (delta >= 1.0 / GAME::FPS) {
    delta -= 1.0/ GAME::FPS;
    ++TIMER;
    return true;
  } else {
    return false;
  }
}

void Game::resetGame() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    GAME::HEALTH = 100.0f;
    GAME::MILES = 0.0f;
    TIMER = 0;


    Airplane::singleton().resetFallState(); // Resetowanie stanu spadania samolotu

	reset = false;
    cout << "Game has been reset.\n";
}


void updateFPSCount(double& previousSecond, int& updates) {
  if (DisplayManager::getTime() - previousSecond < 1.0) {
    return;
  }

  ++previousSecond;
  cout << "FPS: " << updates << "\n";
  updates = 0;
}
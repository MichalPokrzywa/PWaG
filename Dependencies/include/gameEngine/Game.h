// Game.h
#pragma once
#include <renderEngine/Renderer.h>

class Game {
private:
  Renderer renderer;

  double currentTime, lastTime, previousSecond, delta;
  bool reset = false;
  int updates = 0;
public:
  Game();
  ~Game();

  void run();
  bool shouldRun();
  bool shouldUpdate();
  void resetGame();

  static void init();
  static Game& theOne();
};

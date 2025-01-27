// main.cc
#include <gameEngine/Game.h>

int main() {
  Game::init();
  while (Game::singleton().shouldRun()) {
    Game::singleton().run();
  }
  return 0;
}

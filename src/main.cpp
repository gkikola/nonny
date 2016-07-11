#include <iostream>
#include <stdexcept>

#include "game.h"

int main() {
  try {
    Game game;
    game.run();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}

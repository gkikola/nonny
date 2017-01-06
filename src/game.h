#ifndef NONNY_GAME_H
#define NONNY_GAME_H

#include "puzzle.h"

class Game {
 public:
  Game();
  ~Game();

  void update(int elapsed_time);

  void load_puzzle(const std::string& filename);
 private:
  Puzzle* m_puzzle;
};

#endif

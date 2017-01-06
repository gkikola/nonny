#ifndef NONNY_GAME_H
#define NONNY_GAME_H

#include "puzzle.h"

enum class GameState { main_menu, opts_menu, puzzle_selection, puzzle };

class Game {
 public:
  Game();
  ~Game();

  GameState game_state() const { return m_state; }
  void update(int elapsed_time);

  void load_puzzle(const std::string& filename);

  void get_puzzle_coords(int* x, int* y) const;
  int cell_size() const { return m_cell_size; }

  const Puzzle& puzzle() const { return *m_puzzle; }
 private:
  Puzzle* m_puzzle;
  
  GameState m_state;
  int m_x, m_y;
  int m_cell_size;
};

#endif

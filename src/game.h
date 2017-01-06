#ifndef NONNY_GAME_H
#define NONNY_GAME_H

#include "puzzle.h"

enum class GameState { main_menu, opts_menu, puzzle_selection, puzzle };

class Game {
 public:
  Game();
  ~Game();

  GameState state() const { return m_state; }
  void update(int elapsed_time);

  void load_puzzle(const std::string& filename);

  void get_puzzle_coords(int* x, int* y) const;
  int cell_size() const { return m_cell_size; }

  void set_rule_dimensions(int row_rule_width, int col_rule_height);
  bool has_size_changed() const { return m_recalc_size; }

  void cell_coords_to_screen_coords(int x, int y, int* screen_x, int* screen_y);
  void screen_coords_to_cell_coords(int screen_x, int screen_y, int* x, int* y);
  
  const Puzzle& puzzle() const { return *m_puzzle; }
 private:
  Puzzle* m_puzzle;
  
  GameState m_state;
  int m_x, m_y;
  int m_cell_size;

  bool m_recalc_size;
  int m_row_rule_width;
  int m_col_rule_height;
};

#endif

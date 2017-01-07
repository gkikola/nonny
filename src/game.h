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
  void move_puzzle(int relx, int rely);
  int cell_size() const { return m_cell_size; }

  void set_rule_dimensions(int row_rule_width, int col_rule_height);
  bool has_size_changed() const { return m_recalc_size; }

  void update_screen_size(int width, int height);

  void age_cells(int max_age);

  void cell_coords_to_screen_coords(int x, int y, int* screen_x, int* screen_y);
  void screen_coords_to_cell_coords(int screen_x, int screen_y, int* x, int* y);

  int cell_grid_width();
  int cell_grid_height();
    
  int info_pane_width() const { return m_info_pane_width; }
  bool is_info_pane_visible() const { return m_info_pane_visible; }

  void set_cell(int x, int y, CellState state);
  void select_cell(int x, int y);
  void clear_selection();

  void zoom_in(int x, int y);
  void zoom_out(int x, int y);

  void get_selected_cell(int* x, int* y) const;
  bool is_cell_selected() const { return m_selected; }
  
  const Puzzle& puzzle() const { return *m_puzzle; }
 private:
  void default_zoom();
  void zoom(int amount, int x, int y);

  Puzzle* m_puzzle;
  
  GameState m_state;
  int m_grid_x, m_grid_y;
  int m_cell_size;
  int m_info_pane_width;
  bool m_info_pane_visible;

  int m_selection_x, m_selection_y;
  bool m_selected;

  bool m_recalc_size;
  int m_row_rule_width;
  int m_col_rule_height;

  int m_screen_width;
  int m_screen_height;
};

#endif

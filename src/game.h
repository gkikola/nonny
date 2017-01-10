#ifndef NONNY_GAME_H
#define NONNY_GAME_H

#include "info_pane.h"
#include "menu.h"
#include "puzzle.h"

enum class GameState { main_menu, opts_menu, about, puzzle_selection, puzzle };

class Game {
 public:
  Game(const std::string& data_dir, const std::string& save_dir);
  ~Game();

  GameState state() const { return m_state; }
  void set_state(GameState state);
  void update(int elapsed_time);

  void load_puzzle(const std::string& filename);
  bool is_puzzle_loaded() const { return m_puzzle_loaded; }

  void get_puzzle_coords(int* x, int* y) const;
  void move_puzzle(int relx, int rely);
  void make_selected_cell_visible();
  int cell_size() const { return m_cell_size; }

  void set_rule_dimensions(int row_rule_width, int col_rule_height);
  bool has_size_changed() const { return m_recalc_size; }

  void update_screen_size(int width, int height);

  void age_cells(int max_age);

  void cell_coords_to_screen_coords(int x, int y, int* screen_x, int* screen_y);
  void screen_coords_to_cell_coords(int screen_x, int screen_y, int* x, int* y);

  int cell_grid_width();
  int cell_grid_height();
    
  void set_cell(int x, int y, CellState state);
  void select_cell(int x, int y);
  void clear_selection();

  void zoom_in(int x, int y);
  void zoom_out(int x, int y);

  void get_selected_cell(int* x, int* y) const;
  bool is_cell_selected() const { return m_selected; }

  void quit();
  bool is_running() const { return m_running; }

  const std::string& data_dir() const { return m_data_dir; }
  const std::string& save_dir() const { return m_save_dir; }

  int screen_width() const { return m_screen_width; }
  int screen_height() const { return m_screen_height; }
  
  InfoPane& info_pane() { return *m_info_pane; }
  Menu& main_menu() { return *m_main_menu; }
  Menu& about_menu() { return *m_about_menu; }
  const Puzzle& puzzle() const { return *m_puzzle; }
 private:
  void setup_main_menu();
  void setup_about_menu();
  void default_zoom();
  void zoom(int amount, int x, int y);

  Puzzle* m_puzzle;
  bool m_puzzle_loaded;
  
  GameState m_state;
  GameState m_next_state;
  int m_grid_x, m_grid_y;
  int m_cell_size;
  int m_target_cell_size;
  int m_target_x, m_target_y;
  double m_zoom_delta;

  InfoPane* m_info_pane;
  Menu* m_main_menu;
  Menu* m_about_menu;

  int m_selection_x, m_selection_y;
  bool m_selected;

  bool m_recalc_size;
  int m_row_rule_width;
  int m_col_rule_height;

  int m_screen_width;
  int m_screen_height;

  bool m_running;

  std::string m_data_dir;
  std::string m_save_dir;
};

#endif

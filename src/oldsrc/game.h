#ifndef NONNY_GAME_H
#define NONNY_GAME_H

#include "puzzle/puzzle.hpp"

#include "collection_index.h"
#include "info_pane.h"
#include "menu.h"
#include "scrollbar.h"

enum class GameState { main_menu, opts_menu, about, collection_selection,
    puzzle_selection, puzzle };

enum class CellState { blank, marked, exedout };

class Game {
 public:
  Game(const std::string& data_dir, const std::string& save_dir,
       char filesystem_separator);
  ~Game();

  GameState state() const { return m_state; }
  void set_state(GameState state);
  void update(int elapsed_time);

  void load_puzzle(const std::string& filename);
  bool is_puzzle_loaded() const { return m_puzzle_loaded; }

  void open_puzzle_collection(const std::string& path);

  void get_puzzle_coords(int* x, int* y) const;
  void move_puzzle(int relx, int rely, bool instant = true);
  void make_selected_cell_visible();
  int cell_size() const { return m_cell_size; }

  int vert_scrollbar_width() const;
  int horiz_scrollbar_height() const;
  void get_play_area(int* x, int* y, int* width, int* height) const;

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
  char filesystem_separator() const { return m_filesystem_separator; }

  int screen_width() const { return m_screen_width; }
  int screen_height() const { return m_screen_height; }

  void escape_to_menu();
  
  InfoPane& info_pane() { return *m_info_pane; }
  Menu& main_menu() { return *m_main_menu; }
  Menu& about_menu() { return *m_about_menu; }
  Menu& collection_menu() { return *m_collection_menu; }
  Menu& puzzle_menu() { return *m_puzzle_menu; }
  Scrollbar& vscrollbar() { return *m_vscroll; }
  Scrollbar& hscrollbar() { return *m_hscroll; }
  const Puzzle& puzzle() const { return m_puzzle; }
 private:
  void setup_main_menu(bool full_reset);
  void setup_about_menu();
  void setup_collection_menu();
  void setup_puzzle_menu();
  void default_zoom();
  void zoom(int amount, int x, int y);
  void move_puzzle_in_bounds();
  void update_scrollbars();

  Puzzle m_puzzle;
  
  bool m_puzzle_loaded;

  CollectionIndex* m_collection_index;
  PuzzleCollection* m_collection;
  
  GameState m_state;
  GameState m_next_state;
  int m_grid_x, m_grid_y;
  int m_target_grid_x, m_target_grid_y;
  int m_cell_size;
  int m_target_cell_size;
  double m_zoom_delta;
  int m_target_x, m_target_y;
  double m_grid_x_delta, m_grid_y_delta;

  InfoPane* m_info_pane;
  Menu* m_main_menu;
  Menu* m_about_menu;
  Menu* m_collection_menu;
  Menu* m_puzzle_menu;
  Scrollbar* m_vscroll;
  Scrollbar* m_hscroll;

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
  char m_filesystem_separator;
};

#endif

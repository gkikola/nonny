#ifndef NONNY_GAME_H
#define NONNY_GAME_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "puzzle.h"

enum class DragType { screen, marks, xes, blank_marks, blank_xes };

class Game {
public:
  Game();
  ~Game();

  void run();
private:
  void reload_font();
  void SDL_cleanup();
  void draw();
  void draw_cells();
  void shade_cells();
  void draw_rules();
  SDL_Texture* rule_entry_to_texture(const RuleEntry& e);

  void zoom(int incr, int cell_x, int cell_y);
  void screen_coords_to_cell_coords(int x, int y, int& cell_x, int& cell_y);
  void cell_coords_to_screen_coords(int x, int y,
                                    int& screen_x, int& screen_y);
  int actual_grid_width();
  int actual_grid_height();
  int row_rule_width(int row, int buffer = 0);
  int col_rule_height(int col, int buffer = 0);
  
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* cell_sheet_tex;
  TTF_Font* main_font;

  Puzzle* puzzle;
  int x_pos, y_pos;
  int cell_size;
  const int cell_size_step = 5;

  int mouse_x, mouse_y;
  bool dragging;
  DragType drag_type;
  
  bool exit;
  std::string data_path;
  std::string save_path;

  const std::string cell_sheet_filename {"cell.png"};
  int cell_sheet_frame_size;
  const std::string main_font_filename {"FreeSans.ttf"};
};

#endif

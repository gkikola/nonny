#ifndef NONNY_RENDERER_H
#define NONNY_RENDERER_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "preview.h"
#include "puzzle.h"

class Control;
class Game;

class Renderer {
 public:
  Renderer(SDL_Window* window, Game* game, const std::string& data_dir);
  ~Renderer();

  void render_game();
  void render_control(const Preview* preview);
  
  void update(int elapsed_time);
 private:
  void SDL_error(const std::string& function);

  void reload_font(int font_size);
  SDL_Texture* rule_entry_to_texture(const RuleEntry& e,
                                     int* w = nullptr, int* h = nullptr);

  void draw_horiz_line(int x1, int x2, int y, int extra_thickness);
  void draw_vert_line(int x, int y1, int y2, int extra_thickness);
  
  void render_puzzle();
  void render_info_pane();
  void shade_cells();
  void draw_cells();
  void draw_rules();
  void draw_cell_selection();

  int row_rule_width(int row, int buffer);
  int col_rule_height(int col, int buffer);

  SDL_Window* m_window;
  SDL_Renderer* m_renderer;
  Game* m_game;
  
  SDL_Texture* m_cell_sheet;
  int m_cell_sheet_frame_size;
  int m_num_animation_frames;
  int m_time_until_cell_aging;
  
  TTF_Font* m_menu_font;
  TTF_Font* m_info_font;
  TTF_Font* m_rule_font;

  std::string m_data_dir;

  int m_widest_rule;
  int m_tallest_rule;
};

#endif

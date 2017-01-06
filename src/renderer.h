#ifndef NONNY_RENDERER_H
#define NONNY_RENDERER_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"

class Renderer {
 public:
  explicit Renderer(SDL_Window* window, const std::string& data_dir);
  ~Renderer();

  void render_game(Game& game);
 private:
  void SDL_error(const std::string& function);

  void reload_font(int font_size);
  SDL_Texture* rule_entry_to_texture(const RuleEntry& e,
                                     int* w = nullptr, int* h = nullptr);

  void render_puzzle(Game& game);
  void shade_cells(Game& game);
  void draw_cells(Game& game);
  void draw_rules(Game& game);

  int cell_grid_width(const Game& game);
  int cell_grid_height(const Game& game);
  int row_rule_width(Game& game, int row, int buffer);
  int col_rule_height(Game& game, int col, int buffer);

  SDL_Window* m_window;
  SDL_Renderer* m_renderer;
  
  SDL_Texture* m_cell_sheet;
  int m_cell_sheet_frame_size;
  int m_num_animation_frames;
  
  TTF_Font* m_menu_font;
  TTF_Font* m_info_font;
  TTF_Font* m_rule_font;

  std::string m_data_dir;

  int m_widest_rule;
  int m_tallest_rule;
};

#endif

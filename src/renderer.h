#ifndef NONNY_RENDERER_H
#define NONNY_RENDERER_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"

class Renderer {
 public:
  explicit Renderer(SDL_Window* window, Game* game,
                    const std::string& data_dir);
  ~Renderer();

  void render_game();

  void update(int elapsed_time);
 private:
  void SDL_error(const std::string& function);

  void reload_font(int font_size);
  SDL_Texture* rule_entry_to_texture(const RuleEntry& e,
                                     int* w = nullptr, int* h = nullptr);

  void render_puzzle();
  void render_info_pane();
  void shade_cells();
  void draw_cells();
  void draw_rules();

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

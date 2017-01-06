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

  SDL_Window* m_window;
  SDL_Renderer* m_renderer;
  SDL_Texture* m_cell_sheet;
  TTF_Font* m_menu_font;
  TTF_Font* m_info_font;
  TTF_Font* m_rule_font;

  std::string m_data_dir;

  int m_row_rule_width;
  int m_col_rule_height;
};

#endif

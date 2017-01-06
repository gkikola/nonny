#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"
#include "puzzle.h"

#include "renderer.h"

const std::string cell_sheet_filename = "cell.png";
const std::string font_filename = "FreeSans.ttf";

Renderer::Renderer(SDL_Window* window, const std::string& data_dir)
  : m_window{window}, m_data_dir{data_dir} {
  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED
                                  | SDL_RENDERER_PRESENTVSYNC);
  if (!m_renderer) SDL_error("SDL_CreateRenderer");
  
  //tell SDL that we want anisotropic filtering if available
  if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best") == SDL_FALSE) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  }

  //load sprite sheet
  std::string sprite_path = m_data_dir + cell_sheet_filename;
  m_cell_sheet = IMG_LoadTexture(m_renderer, sprite_path.c_str());
  if (!m_cell_sheet)
    throw std::runtime_error("IMG_LoadTexture: failed to load texture");

  //determine size of each animation frame and number of frames
  m_cell_sheet_frame_size = 0;
  m_num_animation_frames = 0;
  Uint32 fmt;
  int access, width, height;
  SDL_QueryTexture(m_cell_sheet, &fmt, &access, &width, &height);

  m_cell_sheet_frame_size = height / 2;
  if (m_cell_sheet_frame_size <= 0) {
    throw std::runtime_error("SDL_QueryTexture: "
                             "could not determine texture size");
  }

  m_num_animation_frames = width / m_cell_sheet_frame_size;

  //load fonts
  std::string font_path = m_data_dir + font_filename;
  m_menu_font = TTF_OpenFont(font_path.c_str(), 30);
  m_info_font = TTF_OpenFont(font_path.c_str(), 12);
}

Renderer::~Renderer() {
  if (m_menu_font) TTF_CloseFont(m_menu_font);
  if (m_info_font) TTF_CloseFont(m_info_font);
  if (m_rule_font) TTF_CloseFont(m_rule_font);
  if (m_cell_sheet) SDL_DestroyTexture(m_cell_sheet);
  if (m_renderer) SDL_DestroyRenderer(m_renderer);
}

void Renderer::SDL_error(const std::string& function) {
  std::string err_msg = function;
  err_msg += ": ";
  err_msg += SDL_GetError();

  if (m_renderer) SDL_DestroyRenderer(m_renderer);
  if (m_window) SDL_DestroyWindow(m_window);
  SDL_Quit();
  
  throw std::runtime_error(err_msg);
}

SDL_Texture* Renderer::rule_entry_to_texture(const RuleEntry& e,
                                             int* w, int* h) {
  std::string str = std::to_string(e.value);
  SDL_Color color = { 0, 0, 0, 255 };
  
  if (e.completed)
    color = { 96, 96, 96, 255 };
  if (e.hint)
    color = { 0, 0, 255, 255 };
  if (e.error)
    color = { 255, 0, 0, 255 };
      
  SDL_Surface* surface = TTF_RenderText_Blended(m_rule_font,
                                                str.c_str(), color);
  SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surface);
  SDL_FreeSurface(surface);

  //get texture size if requested
  if (w && h) {
    Uint32 fmt;
    int access;
    SDL_QueryTexture(tex, &fmt, &access, w, h);
  }

  return tex;
}

void Renderer::render_game(Game& game) {
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
  SDL_RenderClear(m_renderer);

  if (game.state() == GameState::puzzle)
    render_puzzle(game);
    
  //drawing code goes here
  
  SDL_RenderPresent(m_renderer);
}

void Renderer::render_puzzle(Game& game) {
  if (game.has_size_changed()) {
    reload_font(game.cell_size() * 3 / 5);
  }

  int spacing = game.cell_size() / 3;

  draw_cells(game);
}

void Renderer::draw_cells(Game& game) {
  shade_cells(game);

  int start_x, start_y;
  game.get_puzzle_coords(&start_x, &start_y);
  int cell_size = game.cell_size();

  for (int y = 0; y < game.puzzle().height(); ++y) {
    for (int x = 0; x < game.puzzle().width(); ++x) {
      int sheet_row, sheet_col;

      bool draw_cell = false;
      //determine animation frames
      if (game.puzzle().cell(x, y) == CellState::marked) {
        sheet_row = 0;
        sheet_col = game.puzzle().cell_age(x, y);
        draw_cell = true;
      } else if (game.puzzle().cell(x, y) == CellState::xedout) {
        sheet_row = 1;
        sheet_col = game.puzzle().cell_age(x, y);
        draw_cell = true;
      } else if (game.puzzle().cell(x, y) == CellState::blank) {
        //reverse the animation for cells being cleared
        if (game.puzzle().prev_cell_state(x, y) == CellState::marked)
          sheet_row = 0;
        else if (game.puzzle().prev_cell_state(x, y) == CellState::xedout)
          sheet_row = 1;

        if (game.puzzle().prev_cell_state(x, y) != CellState::blank) {
          sheet_col = m_num_animation_frames - game.puzzle().cell_age(x, y) - 1;
          draw_cell = true;
        }
      }
      
      SDL_Rect src, dest;
      src.x = sheet_col * m_cell_sheet_frame_size;
      src.y = sheet_row * m_cell_sheet_frame_size;
      src.w = src.h = m_cell_sheet_frame_size;

      game.cell_coords_to_screen_coords(x, y, &dest.x, &dest.y);
      dest.w = dest.h = game.cell_size();
      
      if (draw_cell)
        SDL_RenderCopy(m_renderer, m_cell_sheet, &src, &dest);
    }
  }

  //draw cell borders
  SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 255);
  
  for(int x = 0; x <= game.puzzle().width(); ++x) {
    int x_coord = start_x + x * (cell_size + 1);
    SDL_RenderDrawLine(m_renderer, x_coord, start_y, x_coord,
                       start_y + game.puzzle().height() * (cell_size + 1));

    if (x % 5 == 0) { //draw guide lines
      SDL_RenderDrawLine(m_renderer, x_coord - 1, start_y, x_coord - 1,
                         start_y + game.puzzle().height() * (cell_size + 1));
      SDL_RenderDrawLine(m_renderer, x_coord + 1, start_y, x_coord + 1,
                         start_y + game.puzzle().height() * (cell_size + 1));
    }
  }

  for(int y = 0; y <= game.puzzle().height(); ++y) {
    int y_coord = start_y + y * (cell_size + 1);
    SDL_RenderDrawLine(m_renderer, start_x, y_coord,
                       start_x + game.puzzle().width() * (cell_size + 1),
                       y_coord);

    if (y % 5 == 0) {
      SDL_RenderDrawLine(m_renderer, start_x, y_coord - 1,
                         start_x + game.puzzle().width() * (cell_size + 1),
                         y_coord - 1);
      SDL_RenderDrawLine(m_renderer, start_x, y_coord + 1,
                         start_x + game.puzzle().width() * (cell_size + 1),
                         y_coord + 1);
    }
  }
}

void Renderer::shade_cells(Game& game) {
  int start_x, start_y;
  game.get_puzzle_coords(&start_x, &start_y);
  int cell_size = game.cell_size();
  
  for (int y = 0; y < game.puzzle().height(); ++y) {
    for (int x = 0; x < game.puzzle().width(); ++x) {
      
      SDL_Rect rect;
      rect.x = 1 + start_x + x * (cell_size + 1);
      rect.y = 1 + start_y + y * (cell_size + 1);
      rect.w = rect.h = cell_size;

      if (x % 2 != y % 2)
        SDL_SetRenderDrawColor(m_renderer, 240, 240, 255, 255);
      else if (x % 2 == 0 && y % 2 == 0)
        SDL_SetRenderDrawColor(m_renderer, 230, 230, 255, 255);
      else
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
      
      SDL_RenderFillRect(m_renderer, &rect);
    }
  }
}

void Renderer::reload_font(int font_size) {
  if (m_rule_font)
    TTF_CloseFont(m_rule_font);

  std::string font_path = m_data_dir + font_filename;
  m_rule_font = TTF_OpenFont(font_path.c_str(), font_size);

  if (!m_rule_font)
    throw std::runtime_error("TTF_OpenFont: could not open font file");
}

#include <iostream>
#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"
#include "puzzle.h"

#include "renderer.h"

const std::string cell_sheet_filename = "cell.png";
const std::string font_filename_std = "FreeSans.ttf";
const std::string font_filename_bold = "FreeSansBold.ttf";

const int cell_age_time = 50;
const bool show_framerate = false;

Renderer::Renderer(SDL_Window* window, Game* game, const std::string& data_dir)
  : m_window{window}, m_game{game}, m_data_dir{data_dir}, m_framerate{0} {
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
  m_time_until_cell_aging = cell_age_time;
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
  std::string font_path = m_data_dir + font_filename_bold;
  m_title_font = TTF_OpenFont(font_path.c_str(), 32);

  font_path = m_data_dir + font_filename_std;
  m_control_font = TTF_OpenFont(font_path.c_str(), 24);

  if (m_title_font == NULL || m_control_font == NULL)
    throw std::runtime_error("TTF_OpenFont: could not open font file");
  
  m_rule_font = nullptr; //will be initialized when rendering starts
}

Renderer::~Renderer() {
  if (m_title_font) TTF_CloseFont(m_title_font);
  if (m_control_font) TTF_CloseFont(m_control_font);
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

void Renderer::render_game() {
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
  SDL_RenderClear(m_renderer);

  if (m_game->state() == GameState::puzzle) {
    render_puzzle();
    render_info_pane();
  }
    
  SDL_RenderPresent(m_renderer);
}

void Renderer::update(int elapsed_time) {
  //handle cell animation
  m_time_until_cell_aging -= elapsed_time;

  while (m_time_until_cell_aging < 0) {
    m_game->age_cells(m_num_animation_frames - 1);
    m_time_until_cell_aging += cell_age_time;
  }

  if (show_framerate)
    m_framerate = 1000.0 / elapsed_time;
}

void Renderer::draw_horiz_line(int x1, int x2, int y, int extra_thickness) {
  SDL_Rect rect;
  rect.x = x1;
  rect.y = y - extra_thickness;
  rect.w = x2 - x1;
  rect.h = 1 + 2 * extra_thickness;
  SDL_RenderFillRect(m_renderer, &rect);
}

void Renderer::draw_vert_line(int x, int y1, int y2, int extra_thickness) {
  SDL_Rect rect;
  rect.x = x - extra_thickness;
  rect.y = y1;
  rect.w = 1 + 2 * extra_thickness;
  rect.h = y2 - y1;
  SDL_RenderFillRect(m_renderer, &rect);
}

void Renderer::draw_text(TTF_Font* font, SDL_Color* color,
                         const std::string& str, int x, int y,
                         int width, int height) {
  SDL_Surface* surface = TTF_RenderUTF8_Blended(font, str.c_str(), *color);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
  SDL_FreeSurface(surface);

  int texture_width, texture_height, access;
  Uint32 fmt;
  SDL_QueryTexture(texture, &fmt, &access, &texture_width, &texture_height);

  if (width == 0) width = texture_width;
  if (height == 0) height = texture_height;
  
  SDL_Rect dest_rect;
  dest_rect.x = x + width / 2 - texture_width / 2;
  dest_rect.y = y + height / 2 - texture_height / 2;
  dest_rect.w = texture_width;
  dest_rect.h = texture_height;
  
  SDL_RenderCopy(m_renderer, texture, NULL, &dest_rect);
  SDL_DestroyTexture(texture);
}

void Renderer::render_puzzle() {
  if (!m_rule_font || m_game->has_size_changed()) {
    int font_size = m_game->cell_size() * 3 / 5;

    if (font_size <= 0)
      font_size = 1;
    
    reload_font(font_size);
  }

  draw_cells();
  draw_rules();
  draw_cell_selection();
  if (show_framerate)
    draw_framerate();
}

void Renderer::render_info_pane() {
  //get window dimensions
  int width, height;
  SDL_GetWindowSize(m_window, &width, &height);
  
  SDL_Rect info_pane;
  info_pane.x = 0;
  info_pane.y = 0;
  info_pane.w = m_game->info_pane().width();
  info_pane.h = height;

  SDL_SetRenderDrawColor(m_renderer, 123, 175, 212, 255);
  SDL_RenderFillRect(m_renderer, &info_pane);

  m_game->info_pane().draw(this);
}

void Renderer::draw_cells() {
  shade_cells();

  int start_x, start_y;
  m_game->get_puzzle_coords(&start_x, &start_y);
  int cell_size = m_game->cell_size();

  for (int y = 0; y < m_game->puzzle().height(); ++y) {
    for (int x = 0; x < m_game->puzzle().width(); ++x) {
      int sheet_row, sheet_col;

      bool draw_cell = false;
      //determine animation frames
      if (m_game->puzzle().cell(x, y) == CellState::marked) {
        sheet_row = 0;
        sheet_col = m_game->puzzle().cell_age(x, y);
        draw_cell = true;
      } else if (m_game->puzzle().cell(x, y) == CellState::exedout) {
        sheet_row = 1;
        sheet_col = m_game->puzzle().cell_age(x, y);
        draw_cell = true;
      } else if (m_game->puzzle().cell(x, y) == CellState::blank) {
        //reverse the animation for cells being cleared
        if (m_game->puzzle().prev_cell_state(x, y) == CellState::marked)
          sheet_row = 0;
        else if (m_game->puzzle().prev_cell_state(x, y) == CellState::exedout)
          sheet_row = 1;

        if (m_game->puzzle().prev_cell_state(x, y) != CellState::blank) {
          sheet_col = (m_num_animation_frames
                       - m_game->puzzle().cell_age(x, y) - 1);
          draw_cell = true;
        }
      }
      
      SDL_Rect src, dest;
      src.x = sheet_col * m_cell_sheet_frame_size;
      src.y = sheet_row * m_cell_sheet_frame_size;
      src.w = src.h = m_cell_sheet_frame_size;

      m_game->cell_coords_to_screen_coords(x, y, &dest.x, &dest.y);
      dest.w = dest.h = m_game->cell_size();
      
      if (draw_cell)
        SDL_RenderCopy(m_renderer, m_cell_sheet, &src, &dest);
    }
  }

  //draw cell borders
  SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 255);
  
  for(int x = 0; x <= m_game->puzzle().width(); ++x) {
    int x_coord = start_x + x * (cell_size + 1);
    SDL_RenderDrawLine(m_renderer, x_coord, start_y, x_coord,
                       start_y + m_game->puzzle().height() * (cell_size + 1));

    if (x % 5 == 0) { //draw guide lines
      SDL_RenderDrawLine(m_renderer, x_coord - 1, start_y, x_coord - 1,
                         start_y + m_game->puzzle().height() * (cell_size + 1));
      SDL_RenderDrawLine(m_renderer, x_coord + 1, start_y, x_coord + 1,
                         start_y + m_game->puzzle().height() * (cell_size + 1));
    }
  }

  for(int y = 0; y <= m_game->puzzle().height(); ++y) {
    int y_coord = start_y + y * (cell_size + 1);
    SDL_RenderDrawLine(m_renderer, start_x, y_coord,
                       start_x + m_game->puzzle().width() * (cell_size + 1),
                       y_coord);

    if (y % 5 == 0) {
      SDL_RenderDrawLine(m_renderer, start_x, y_coord - 1,
                         start_x + m_game->puzzle().width() * (cell_size + 1),
                         y_coord - 1);
      SDL_RenderDrawLine(m_renderer, start_x, y_coord + 1,
                         start_x + m_game->puzzle().width() * (cell_size + 1),
                         y_coord + 1);
    }
  }
}

void Renderer::shade_cells() {
  int start_x, start_y;
  m_game->get_puzzle_coords(&start_x, &start_y);
  int cell_size = m_game->cell_size();
  
  for (int y = 0; y < m_game->puzzle().height(); ++y) {
    for (int x = 0; x < m_game->puzzle().width(); ++x) {
      
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

void Renderer::draw_rules() {
  int cell_size = m_game->cell_size();
  const int buffer = cell_size / 3;
  int col_rule_bottom = 0;

  m_tallest_rule = m_widest_rule = 0;

  for (int i = 0; i < m_game->puzzle().width(); ++i) {
    int col_left_edge, col_top_edge;
    m_game->cell_coords_to_screen_coords(i, 0, &col_left_edge, &col_top_edge);

    int col_height = col_rule_height(i, buffer);
    int x = col_left_edge;
    int y = col_top_edge - col_height + buffer;

    if (col_height > m_tallest_rule)
      m_tallest_rule = col_height;

    if (y < buffer)
      y = buffer;

    //keep track of how far down the column numbers go
    if (col_rule_bottom < y + col_height)
      col_rule_bottom = y + col_height;

    for (auto entry : m_game->puzzle().get_col_rule(i)) {
      int w, h;
      SDL_Texture* tex = rule_entry_to_texture(entry, &w, &h);
      
      SDL_Rect dst_rect;
      dst_rect.x = col_left_edge + cell_size / 2 - w / 2;
      dst_rect.y = y;
      dst_rect.w = w;
      dst_rect.h = h;

      SDL_RenderCopy(m_renderer, tex, NULL, &dst_rect);
      SDL_DestroyTexture(tex);

      y += h + buffer;
    }
  }

  for (int j = 0; j < m_game->puzzle().height(); ++j) {
    int row_left_edge, row_top_edge;
    m_game->cell_coords_to_screen_coords(0, j, &row_left_edge, &row_top_edge);

    int row_width = row_rule_width(j, buffer);
    int x = row_left_edge - row_width + buffer;
    int y = row_top_edge;

    if (row_width > m_widest_rule)
      m_widest_rule = row_width;

    if (x < m_game->info_pane().width() + buffer
        && y >= col_rule_bottom - buffer)
      x = m_game->info_pane().width() + buffer;

    for (auto entry : m_game->puzzle().get_row_rule(j)) {
      int w, h;
      SDL_Texture* tex = rule_entry_to_texture(entry, &w, &h);

      SDL_Rect dst_rect;
      dst_rect.x = x;
      dst_rect.y = row_top_edge + cell_size / 2 - h / 2;
      dst_rect.w = w;
      dst_rect.h = h;

      SDL_RenderCopy(m_renderer, tex, NULL, &dst_rect);
      SDL_DestroyTexture(tex);

      x += w + buffer;
    }
  }

  m_game->set_rule_dimensions(m_widest_rule, m_tallest_rule);
}

void Renderer::draw_cell_selection() {
  if (m_game->is_cell_selected()) {
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);

    int grid_x, grid_y;
    m_game->get_puzzle_coords(&grid_x, &grid_y);
    int grid_width = m_game->cell_grid_width();
    int grid_height = m_game->cell_grid_height();

    int cell_size = m_game->cell_size();

    int sel_x, sel_y;
    m_game->get_selected_cell(&sel_x, &sel_y);
    
    int x1, x2, y1, y2;
    x1 = grid_x - m_widest_rule;
    x2 = grid_x + grid_width;
    y1 = grid_y + sel_y * (cell_size + 1);
    y2 = grid_y + (sel_y + 1) * (cell_size + 1);

    draw_horiz_line(x1, x2, y1, 1);
    draw_horiz_line(x1, x2, y2, 1);
    draw_vert_line(x1 + 1, y1, y2, 1);
    draw_vert_line(x2 - 1, y1, y2, 1);

    x1 = grid_x + sel_x * (cell_size + 1);
    x2 = grid_x + (sel_x + 1) * (cell_size + 1);
    y1 = grid_y - m_tallest_rule;
    y2 = grid_y + grid_height;

    draw_vert_line(x1, y1, y2, 1);
    draw_vert_line(x2, y1, y2, 1);
    draw_horiz_line(x1, x2, y1 + 1, 1);
    draw_horiz_line(x1, x2, y2 - 1, 1);
  }
}

void Renderer::draw_framerate() {
  std::string str = "Rendering ";
  str += std::to_string((int)round(m_framerate));
  str += " frames per second";

  SDL_Color color = { 0, 0, 0, 255 };
  draw_text(m_control_font, &color, str, m_game->info_pane().width() + 5, 5);
}

void Renderer::render_control(const Preview* preview) {
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

  SDL_Rect rect;
  preview->get_position(&rect.x, &rect.y);
  preview->get_size(&rect.w, &rect.h);
  SDL_RenderFillRect(m_renderer, &rect);

  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderDrawRect(m_renderer, &rect);
  
  int pixel_size = preview->pixel_size();
  
  for (int y = 0; y < m_game->puzzle().height(); ++y) {
    for (int x = 0; x < m_game->puzzle().width(); ++x) {
      if (m_game->puzzle().cell(x, y) == CellState::marked) {
        SDL_Rect pixel;
        pixel.x = rect.x + pixel_size * x;
        pixel.y = rect.y + pixel_size * y;
        pixel.w = pixel.h = pixel_size;
        SDL_RenderFillRect(m_renderer, &pixel);
      }
    }
  }
}

void Renderer::render_control(const StaticText* stat_text) {
  int x, y;
  stat_text->get_position(&x, &y);
  int width, height;
  stat_text->get_size(&width, &height);

  std::string str;
  stat_text->get_string(&str);
  
  SDL_Color color = { 0, 0, 0, 255 };
  TTF_Font* font;
  switch (stat_text->type()) {
  case StaticText::Type::heading:
    font = m_title_font;
    break;
  default:
  case StaticText::Type::standard:
    font = m_control_font;
    break;
  }
  
  draw_text(font, &color, str, x, y, width, height);
}

int Renderer::row_rule_width(int row, int buffer) {
  int width = buffer;

  for (auto entry : m_game->puzzle().get_row_rule(row)) {
    std::string entry_str = std::to_string(entry.value);
    int entry_width, entry_height;
    TTF_SizeText(m_rule_font, entry_str.c_str(), &entry_width, &entry_height);

    width += entry_width + buffer;
  }

  return width;
}

int Renderer::col_rule_height(int col, int buffer) {
  int height = buffer;

  for (auto entry : m_game->puzzle().get_col_rule(col)) {
    std::string entry_str = std::to_string(entry.value);
    int entry_width, entry_height;
    TTF_SizeText(m_rule_font, entry_str.c_str(), &entry_width, &entry_height);

    height += entry_height + buffer;
  }
  
  return height;
}

void Renderer::reload_font(int font_size) {
  if (m_rule_font)
    TTF_CloseFont(m_rule_font);

  std::string font_path = m_data_dir + font_filename_std;
  m_rule_font = TTF_OpenFont(font_path.c_str(), font_size);

  if (!m_rule_font)
    throw std::runtime_error("TTF_OpenFont: could not open font file");
}

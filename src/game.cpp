#include <stdexcept>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"

void get_paths(std::string* data_dir, std::string* save_dir);
char get_filesystem_separator();

Game::Game() : exit{false}, puzzle{nullptr}, x_pos{0}, y_pos{0}, cell_size{32},
               mouse_x{0}, mouse_y{0}, dragging{false},
               cell_sheet_tex{nullptr}, main_font{nullptr},
               renderer{nullptr}, window{nullptr} {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::string err_msg("SDL_Init: ");
    err_msg += SDL_GetError();
    throw std::runtime_error(err_msg);
  }

  get_paths(&data_path, &save_path);

  //load SDL_ttf
  if (TTF_Init() != 0) {
    SDL_Quit();
    throw std::runtime_error("TTF_Init: failed to initialize SDL_ttf");
  }

  window = SDL_CreateWindow("Nonny",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            640, 480,
                            SDL_WINDOW_RESIZABLE);

  if (!window) {
    std::string err_msg("SDL_CreateWindow: ");
    err_msg += SDL_GetError();

    SDL_cleanup();
    throw std::runtime_error(err_msg);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED
                                | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {
    std::string err_msg("SDL_CreateRenderer: ");
    err_msg += SDL_GetError();
    
    SDL_cleanup();
    throw std::runtime_error(err_msg);
  }

  //load sprite set
  std::string sprite_path = data_path + cell_sheet_filename;
  cell_sheet_tex = IMG_LoadTexture(renderer, sprite_path.c_str());
  if (!cell_sheet_tex) {
    SDL_cleanup();
    throw std::runtime_error("IMG_LoadTexture: failed to load texture");
  }

  //determine the size of each animation frame and the number of frames
  cell_sheet_frame_size = 0;
  num_animation_frames = 0;
  Uint32 fmt;
  int access, width, height;
  SDL_QueryTexture(cell_sheet_tex, &fmt, &access, &width, &height);

  cell_sheet_frame_size = height / 2;
  
  if (cell_sheet_frame_size <= 0) {
    SDL_cleanup();
    throw std::runtime_error("SDL_QueryTexture: "
                             "could not determine texture size");
  }

  num_animation_frames = width / cell_sheet_frame_size;

  puzzle = new Puzzle(data_path + "test.non");

  reload_font();

  time = SDL_GetTicks();
  time_until_cell_aging = cell_age_time;
}

Game::~Game() {
  delete puzzle;
  SDL_cleanup();
}

void Game::reload_font() {
  if (main_font)
    TTF_CloseFont(main_font);
  
  std::string font_path = data_path + main_font_filename;
  main_font = TTF_OpenFont(font_path.c_str(), cell_size * 3 / 5);
  if (!main_font) {
    SDL_cleanup();
    throw std::runtime_error("TTF_OpenFont: could not open font file");
  }
}

void Game::SDL_cleanup() {
  if (main_font) TTF_CloseFont(main_font);
  if (cell_sheet_tex) SDL_DestroyTexture(cell_sheet_tex);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  if (TTF_WasInit()) TTF_Quit();
  IMG_Quit();
  SDL_Quit();

  main_font = nullptr;
  cell_sheet_tex = nullptr;
  renderer = nullptr;
  window = nullptr;
}

void Game::run() {
  SDL_Event event;
  
  while (!exit) {
    while (SDL_PollEvent(&event)) {
      //process SDL event
      switch(event.type) {
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        mouse_x = event.button.x;
        mouse_y = event.button.y;

        switch (event.button.button) {
        default:
        case SDL_BUTTON_LEFT:
          if (event.type == SDL_MOUSEBUTTONUP)
            dragging = false;
          
          //check if mouse pointer is inside the grid
          if (mouse_x >= x_pos && mouse_x <= x_pos + actual_grid_width()
              && mouse_y >= y_pos && mouse_y <= y_pos + actual_grid_height()) {
            dragging = (event.type == SDL_MOUSEBUTTONDOWN);

            if (dragging) {
              SDL_CaptureMouse(SDL_TRUE);
              
              //determine what cell we are on
              int x, y;
              screen_coords_to_cell_coords(mouse_x, mouse_y, x, y);

              //if the cell is blank, mark it
              if (puzzle->cell(x, y) == CellState::blank) {
                puzzle->set_cell(x, y, CellState::marked);
                drag_type = DragType::marks;
              } else if (puzzle->cell(x, y) == CellState::marked) {
                puzzle->set_cell(x, y, CellState::blank);
                drag_type = DragType::blank_marks;
              } else {
                puzzle->set_cell(x, y, CellState::blank);
                drag_type = DragType::blank_xes;
              }
            }
            else SDL_CaptureMouse(SDL_FALSE);
            break;
          }

          //outside the grid, do screen-dragging action:
        case SDL_BUTTON_MIDDLE:
          dragging = (event.type == SDL_MOUSEBUTTONDOWN);

          if (dragging) {
            drag_type = DragType::screen;
            SDL_CaptureMouse(SDL_TRUE);
          }
          else SDL_CaptureMouse(SDL_FALSE);
          break;
        case SDL_BUTTON_RIGHT:
          if (event.type == SDL_MOUSEBUTTONUP)
            dragging = false;
          
          if (mouse_x >= x_pos && mouse_x <= x_pos + actual_grid_width()
              && mouse_y >= y_pos && mouse_y <= y_pos + actual_grid_height()) {
            dragging = (event.type == SDL_MOUSEBUTTONDOWN);

            if (dragging) {
              SDL_CaptureMouse(SDL_TRUE);
              
              //determine what cell we are on
              int x, y;
              screen_coords_to_cell_coords(mouse_x, mouse_y, x, y);

              //if the cell is blank, x it out
              if (puzzle->cell(x, y) == CellState::blank) {
                puzzle->set_cell(x, y, CellState::xedout);
                drag_type = DragType::xes;
              } else if (puzzle->cell(x, y) == CellState::marked) {
                puzzle->set_cell(x, y, CellState::blank);
                drag_type = DragType::blank_marks;
              } else {
                puzzle->set_cell(x, y, CellState::blank);
                drag_type = DragType::blank_xes;
              }
            }
            else SDL_CaptureMouse(SDL_FALSE);
          }
          
          break;
        }
        break;
      case SDL_MOUSEMOTION:
        if (dragging
            && (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_CAPTURE)) {
          if (drag_type == DragType::screen) {
            x_pos += (event.motion.x - mouse_x);
            y_pos += (event.motion.y - mouse_y);
          }
          
          mouse_x = event.motion.x;
          mouse_y = event.motion.y;
            
          if (drag_type != DragType::screen) {
            int x, y;
            screen_coords_to_cell_coords(mouse_x, mouse_y, x, y);

            CellState state;
            bool change_cell = false;
            if (drag_type == DragType::marks
                && puzzle->cell(x, y) == CellState::blank) {
              change_cell = true;
              state = CellState::marked;
            } else if (drag_type == DragType::xes
                       && puzzle->cell(x, y) == CellState::blank) {
              change_cell = true;
              state = CellState::xedout;
            }
            else if (drag_type == DragType::blank_marks
                     && puzzle->cell(x, y) == CellState::marked) {
              change_cell = true;
              state = CellState::blank;
            } else if (drag_type == DragType::blank_xes
                       && puzzle->cell(x, y) == CellState::xedout) {
              change_cell = true;
              state = CellState::blank;
            }

            if (change_cell)
              puzzle->set_cell(x, y, state);
          }
        }
        break;
      case SDL_MOUSEWHEEL:
        SDL_GetMouseState(&mouse_x, &mouse_y);

        if (event.wheel.y > 0)
          zoom(cell_size_step, mouse_x, mouse_y);
        else if (event.wheel.y < 0)
          zoom(-cell_size_step, mouse_x, mouse_y);
        break;
      case SDL_QUIT:
        exit = true;
        break;
      }
    }

    //calculate frame time
    Uint32 prev_time = time;
    time = SDL_GetTicks();
    int elapsed_time = time - prev_time;

    //handle cell animation
    time_until_cell_aging -= elapsed_time;
    while (time_until_cell_aging < 0) {
      age_cells();
      time_until_cell_aging += cell_age_time;
    }

    draw();
  }
}

void Game::draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  draw_cells();
  draw_rules();

  SDL_RenderPresent(renderer);
}

void Game::draw_cells() {
  SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
  
  for(int x = 0; x <= puzzle->width(); ++x) {
    SDL_RenderDrawLine(renderer, x_pos + x * (cell_size + 1), y_pos,
                       x_pos + x * (cell_size + 1),
                       y_pos + puzzle->height() * (cell_size + 1));
  }

  for(int y = 0; y <= puzzle->height(); ++y) {
    SDL_RenderDrawLine(renderer, x_pos, y_pos + y * (cell_size + 1),
                       x_pos + puzzle->width() * (cell_size + 1),
                       y_pos + y * (cell_size + 1));
  }

  shade_cells();

  for (int y = 0; y < puzzle->height(); ++y) {
    for (int x = 0; x < puzzle->width(); ++x) {
      int sheet_row, sheet_col;

      bool draw_cell = false;
      //determine animation frames
      if (puzzle->cell(x, y) == CellState::marked) {
        sheet_row = 0;
        sheet_col = puzzle->cell_age(x, y);
        draw_cell = true;
      } else if (puzzle->cell(x, y) == CellState::xedout) {
        sheet_row = 1;
        sheet_col = puzzle->cell_age(x, y);
        draw_cell = true;
      } else if (puzzle->cell(x, y) == CellState::blank) {
        //reverse the animation for cells being cleared
        if (puzzle->prev_cell_state(x, y) == CellState::marked)
          sheet_row = 0;
        else if (puzzle->prev_cell_state(x, y) == CellState::xedout)
          sheet_row = 1;

        if (puzzle->prev_cell_state(x, y) != CellState::blank) {
          sheet_col = num_animation_frames - puzzle->cell_age(x, y) - 1;
          draw_cell = true;
        }
      }
      
      SDL_Rect src, dest;
      src.x = sheet_col * cell_sheet_frame_size;
      src.y = sheet_row * cell_sheet_frame_size;
      src.w = src.h = cell_sheet_frame_size;

      cell_coords_to_screen_coords(x, y, dest.x, dest.y);
      dest.w = dest.h = cell_size;
      
      if (draw_cell)
        SDL_RenderCopy(renderer, cell_sheet_tex, &src, &dest);
    }
  }
}

void Game::shade_cells() {
  for (int y = 0; y < puzzle->height(); ++y) {
    for (int x = 0; x < puzzle->width(); ++x) {
      SDL_Rect rect;
      rect.x = 1 + x_pos + x * (cell_size + 1);
      rect.y = 1 + y_pos + y * (cell_size + 1);
      rect.w = rect.h = cell_size;

      if (x % 2 != y % 2)
        SDL_SetRenderDrawColor(renderer, 240, 240, 255, 255);
      else if (x % 2 == 0 && y % 2 == 0)
        SDL_SetRenderDrawColor(renderer, 230, 230, 255, 255);
      else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      
      SDL_RenderFillRect(renderer, &rect);
    }
  }
}

void Game::draw_rules() {
  const int buffer = cell_size / 3;
  int col_rule_bottom = 0;

  for (int i = 0; i < puzzle->width(); ++i) {
    int col_left_edge, col_top_edge;
    cell_coords_to_screen_coords(i, 0, col_left_edge, col_top_edge);

    int col_height = col_rule_height(i, buffer);
    int x = col_left_edge;
    int y = col_top_edge - col_height;

    if (y < 0)
      y = 0;

    //keep track of how far down the column numbers go
    if (col_rule_bottom < y + col_height)
      col_rule_bottom = y + col_height;

    for (auto entry : puzzle->get_col_rule(i)) {
      SDL_Texture* tex = rule_entry_to_texture(entry);

      Uint32 fmt;
      int access, w, h;
      SDL_QueryTexture(tex, &fmt, &access, &w, &h);
      
      SDL_Rect dst_rect;
      dst_rect.x = col_left_edge + cell_size / 2 - w / 2;
      dst_rect.y = y;
      dst_rect.w = w;
      dst_rect.h = h;

      SDL_RenderCopy(renderer, tex, NULL, &dst_rect);
      SDL_DestroyTexture(tex);

      y += h + buffer;
    }
  }

  for (int j = 0; j < puzzle->height(); ++j) {
    int row_left_edge, row_top_edge;
    cell_coords_to_screen_coords(0, j, row_left_edge, row_top_edge);

    int x = row_left_edge - row_rule_width(j, buffer);
    int y = row_top_edge;

    if (x < 0 && y >= col_rule_bottom)
      x = 0;

    for (auto entry : puzzle->get_row_rule(j)) {
      SDL_Texture* tex = rule_entry_to_texture(entry);

      Uint32 fmt;
      int access, w, h;
      SDL_QueryTexture(tex, &fmt, &access, &w, &h);
      
      SDL_Rect dst_rect;
      dst_rect.x = x;
      dst_rect.y = row_top_edge + cell_size / 2 - h / 2;
      dst_rect.w = w;
      dst_rect.h = h;

      SDL_RenderCopy(renderer, tex, NULL, &dst_rect);
      SDL_DestroyTexture(tex);

      x += w + buffer;
    }
  }
}

SDL_Texture* Game::rule_entry_to_texture(const RuleEntry& e) {
  std::string str = std::to_string(e.value);
  SDL_Color color = { 0, 0, 0, 255 };
  
  if (e.completed)
    color = { 96, 96, 96, 255 };
  if (e.hint)
    color = { 0, 0, 255, 255 };
  if (e.error)
    color = { 255, 0, 0, 255 };
      
  SDL_Surface* surf = TTF_RenderText_Blended(main_font,
                                             str.c_str(), color);
  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);

  return tex;
}

void Game::age_cells() {
  for (int x = 0; x < puzzle->width(); ++x)
    for (int y = 0; y < puzzle->height(); ++y)
      puzzle->age_cell(x, y, num_animation_frames - 1);
}

void Game::zoom(int incr, int x, int y) {
  x_pos += -incr * (x - x_pos) / (cell_size + 1);
  y_pos += -incr * (y - y_pos) / (cell_size + 1);
  
  cell_size += incr;
  if (cell_size < 0) cell_size = 0;

  reload_font();
}

void Game::screen_coords_to_cell_coords(int x, int y,
                                        int& cell_x, int& cell_y) {
  cell_x = (x - x_pos - 1) / (cell_size + 1);
  cell_y = (y - y_pos - 1) / (cell_size + 1);
}

void Game::cell_coords_to_screen_coords(int x, int y,
                                        int& screen_x, int& screen_y) {
  screen_x = x_pos + 1 + x * (cell_size + 1);
  screen_y = y_pos + 1 + y * (cell_size + 1);
}

int Game::actual_grid_width() {
  return puzzle->width() * (cell_size + 1) + 1;
}

int Game::actual_grid_height() {
  return puzzle->height() * (cell_size + 1) + 1;
}

int Game::row_rule_width(int row, int buffer) {
  int width = 0;

  for (auto entry : puzzle->get_row_rule(row)) {
    std::string entry_str = std::to_string(entry.value);
    int entry_width, entry_height;
    TTF_SizeText(main_font, entry_str.c_str(), &entry_width, &entry_height);

    width += entry_width + buffer;
  }
  
  return width;
}

int Game::col_rule_height(int col, int buffer) {
  int height = 0;

  for (auto entry : puzzle->get_col_rule(col)) {
    std::string entry_str = std::to_string(entry.value);
    int entry_width, entry_height;
    TTF_SizeText(main_font, entry_str.c_str(), &entry_width, &entry_height);

    height += entry_height + buffer;
  }
  
  return height;
}

void get_paths(std::string* data_dir, std::string* save_dir) {
  *data_dir = DATADIR;
  *data_dir += get_filesystem_separator();

  char* save_ptr = SDL_GetPrefPath(".", "nonny");
  *save_dir = save_ptr;
  SDL_free(save_ptr);
}

char get_filesystem_separator() {
  //SDL_GetBasePath is guaranteed to end with a separator
  char* path_ptr = SDL_GetBasePath();
  std::string path { path_ptr };
  SDL_free(path_ptr);

  if (path.length() > 0)
    return path[path.length() - 1];
  else
    return '/';
}

#include <stdexcept>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"

void get_paths(std::string* data_dir, std::string* save_dir);
char get_filesystem_separator();

Game::Game() : exit{false}, puzzle{nullptr}, x_pos{0}, y_pos{0},
               x_mouse{0}, y_mouse{0}, dragging_screen{false},
               cur_scale{1.0}, target_scale{1.0},
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

  //determine the width/height of each frame
  cell_sheet_frame_size = 0;
  Uint32 fmt;
  int access, width;
  SDL_QueryTexture(cell_sheet_tex, &fmt, &access, &width,
                   &cell_sheet_frame_size);
  if (cell_sheet_frame_size <= 0) {
    SDL_cleanup();
    throw std::runtime_error("SDL_QueryTexture: "
                             "could not determine texture size");
  }

  puzzle = new Puzzle(data_path + "test.non");

  std::string font_path = data_path + main_font_filename;
  main_font = TTF_OpenFont(font_path.c_str(), main_font_size);
  if (!main_font) {
    SDL_cleanup();
    throw std::runtime_error("TTF_OpenFont: could not open font file");
  }
}

Game::~Game() {
  delete puzzle;
  SDL_cleanup();
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
      switch(event.type) {
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
	x_mouse = event.button.x;
	y_mouse = event.button.y;
	
	switch (event.button.button) {
	default:
	case SDL_BUTTON_LEFT:
	  break;
	case SDL_BUTTON_MIDDLE:
	  dragging_screen = (event.type == SDL_MOUSEBUTTONDOWN);

	  if (dragging_screen) SDL_CaptureMouse(SDL_TRUE);
	  else SDL_CaptureMouse(SDL_FALSE);
	  break;
	}
	break;
      case SDL_MOUSEMOTION:
	if (dragging_screen
            && (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_CAPTURE)) {
	  x_pos += (event.motion.x - x_mouse);
	  y_pos += (event.motion.y - y_mouse);
	  x_mouse = event.motion.x;
	  y_mouse = event.motion.y;
	}
	break;
      case SDL_QUIT:
	exit = true;
	break;
      }
    }
    
    if (cur_scale - target_scale >= scale_step)
      cur_scale -= scale_step / scale_time;
    else if (target_scale - cur_scale >= scale_step)
      cur_scale += scale_step / scale_time;

    draw();
  }
}

void Game::draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  //shade cells
  int scaled_frame_size = (int)round(cur_scale * cell_sheet_frame_size);
  for (int y = 0; y < puzzle->height(); ++y) {
    for (int x = 0; x < puzzle->width(); ++x) {
      SDL_Rect rect;
      rect.x = x_pos + x * scaled_frame_size;
      rect.y = y_pos + y * scaled_frame_size;
      rect.w = rect.h = scaled_frame_size;

      if (x % 2 != y % 2)
	SDL_SetRenderDrawColor(renderer, 240, 240, 255, 255);
      else if (x % 2 == 0 && y % 2 == 0)
	SDL_SetRenderDrawColor(renderer, 230, 230, 255, 255);
      else
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      
      SDL_RenderFillRect(renderer, &rect);
    }
  }

  SDL_RenderPresent(renderer);
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

#include <stdexcept>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include "game.h"

const std::string cell_sheet_filename {"cells.bmp"};
const int cell_sheet_frame_size = 32;

void get_paths(std::string* data_dir, std::string* save_dir);
char get_filesystem_separator();

Game::Game() : exit{false}, puzzle{nullptr} {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::string err_msg("SDL_Init: ");
    err_msg += SDL_GetError();
    throw std::runtime_error(err_msg);
  }

  get_paths(&data_path, &save_path);

  window = SDL_CreateWindow("Nonny",
			    SDL_WINDOWPOS_CENTERED,
			    SDL_WINDOWPOS_CENTERED,
			    640, 480,
			    SDL_WINDOW_RESIZABLE);

  if (!window) {
    std::string err_msg("SDL_CreateWindow: ");
    err_msg += SDL_GetError();
    
    SDL_Quit();
    throw std::runtime_error(err_msg);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED
                                | SDL_RENDERER_PRESENTVSYNC);

  if (!renderer) {
    std::string err_msg("SDL_CreateRenderer: ");
    err_msg += SDL_GetError();
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw std::runtime_error(err_msg);
  }

  //load sprite set
  std::string sprite_path = data_path + cell_sheet_filename;
  SDL_Surface* loading_surface = SDL_LoadBMP(sprite_path.c_str());
  if (!loading_surface) {    
    std::string err_msg("SDL_LoadBMP: ");
    err_msg += SDL_GetError();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw std::runtime_error(err_msg);
  }

  cell_sheet_tex = SDL_CreateTextureFromSurface(renderer, loading_surface);
  SDL_FreeSurface(loading_surface);

  if (!cell_sheet_tex) {
    std::string err_msg("SDL_CreateTextureFromSurface: ");
    err_msg += SDL_GetError();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw std::runtime_error(err_msg);
  }

  puzzle = new Puzzle("default.non");
}

Game::~Game() {
  delete puzzle;
  
  SDL_DestroyTexture(cell_sheet_tex);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Game::run() {
  SDL_Event event;
  
  while (!exit) {
    SDL_PollEvent(&event);

    switch(event.type) {
    case SDL_QUIT:
      exit = true;
      break;
    }

    draw();
  }
}

void Game::draw() {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  //shade cells
  for (int y = 0; y < puzzle->height(); ++y) {
    for (int x = 0; x < puzzle->width(); ++x) {
      SDL_Rect rect;
      rect.x = 16 + x * cell_sheet_frame_size;
      rect.y = 16 + y * cell_sheet_frame_size;
      rect.w = rect.h = cell_sheet_frame_size;

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

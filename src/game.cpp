#include <stdexcept>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include "game.h"

Game::Game() : exit{false} {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::string err_msg("SDL_Init: ");
    err_msg += SDL_GetError();
    throw std::runtime_error(err_msg);
  }

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

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    std::string err_msg("SDL_CreateRenderer: ");
    err_msg += SDL_GetError();
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    throw std::runtime_error(err_msg);
  }
}

Game::~Game() {
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

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }
}

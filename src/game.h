#ifndef NONNY_GAME_H
#define NONNY_GAME_H

#include <string>
#include <SDL2/SDL.h>

#include "puzzle.h"

class Game {
public:
  Game();
  ~Game();

  void run();
private:
  void draw();
  
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* cell_sheet_tex;

  Puzzle* puzzle;

  bool exit;
  std::string data_path;
  std::string save_path;
};

#endif

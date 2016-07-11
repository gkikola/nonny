#include <SDL2/SDL.h>

class Game {
public:
  Game();
  ~Game();

  void run();
private:
  SDL_Window* window;
  SDL_Renderer* renderer;

  bool exit;
};

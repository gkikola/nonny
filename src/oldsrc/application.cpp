#include <cctype>
#include <stdexcept>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "config.h"
#include "game.h"
#include "input_handler.h"
#include "renderer.h"

#include "application.h"

const int default_win_width = 800;
const int default_win_height = 600;
const std::string default_win_title = "Nonny";

void get_paths(std::string* data_dir, std::string* save_dir);
char get_filesystem_separator();

Application::Application() : m_game{nullptr}, m_window{nullptr},
                             m_renderer{nullptr}, m_input_handler{nullptr} {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) SDL_error("SDL_Init");

  if (TTF_Init() != 0) {
    SDL_Quit();
    throw std::runtime_error("TTF_Init: failed to initialize SDL_ttf");
  }
  
  m_window = SDL_CreateWindow(default_win_title.c_str(),
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              default_win_width, default_win_height,
                              SDL_WINDOW_RESIZABLE);
  if (!m_window) SDL_error("SDL_CreateWindow");

  get_paths(&m_data_dir, &m_save_dir);

  m_game = new Game(m_data_dir, m_save_dir, get_filesystem_separator());
  m_game->update_screen_size(default_win_width, default_win_height);

  m_renderer = new Renderer(m_window, m_game);
  m_input_handler = new InputHandler(m_window, m_game);
}

Application::~Application() {  
  cleanup();
}

void Application::cleanup() {
  if (m_renderer) delete m_renderer;
  if (m_game) delete m_game;
  if (m_window) SDL_DestroyWindow(m_window);
  if (TTF_WasInit()) TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void Application::SDL_error(const std::string& function) {
  std::string err_msg = function;
  err_msg += ": ";
  err_msg += SDL_GetError();
  
  cleanup();

  throw std::runtime_error(err_msg);
}

void Application::run() {
  SDL_Event event;
  Uint32 time = SDL_GetTicks();
  
  while (m_game->is_running()) {
    while (SDL_PollEvent(&event)) {
      //process SDL event
      switch (event.type) {
      case SDL_WINDOWEVENT:

        switch (event.window.event) {
        case SDL_WINDOWEVENT_RESIZED:
          m_game->update_screen_size(event.window.data1, event.window.data2);
        }
        
        break;
      case SDL_MOUSEBUTTONDOWN:
        m_input_handler->mouse_press(event.button.button, true);
        break;
      case SDL_MOUSEBUTTONUP:
        m_input_handler->mouse_press(event.button.button, false);
        break;
      case SDL_MOUSEMOTION:
        m_input_handler->mouse_move(event.motion.x, event.motion.y);
        break;
      case SDL_MOUSEWHEEL:
        m_input_handler->mouse_wheel(event.wheel.y, event.wheel.direction);
        break;
      case SDL_KEYDOWN:
        m_input_handler->key_press(event.key.keysym.sym, true);
        break;
      case SDL_KEYUP:
        m_input_handler->key_press(event.key.keysym.sym, false);
        break;
      case SDL_QUIT:
        m_game->quit();
        break;
      }
    }

    //calculate frame time
    Uint32 prev_time = time;
    time = SDL_GetTicks();
    int elapsed_time = time - prev_time;

    //handle game frame updates
    m_input_handler->update(elapsed_time);
    m_game->update(elapsed_time);
    m_renderer->update(elapsed_time);
    
    //handle rendering
    m_renderer->render_game();

    //give control back to the OS
    SDL_Delay(1);
  }
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

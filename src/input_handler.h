#ifndef NONNY_INPUT_HANDLER_H
#define NONNY_INPUT_HANDLER_H

#include <map>
#include <vector>
#include <SDL2/SDL.h>

class Game;

class InputHandler {
 public:
  explicit InputHandler(SDL_Window* window, Game* game);

  void set_default_controls();
  
  void update(int elapsed_time);
  void mouse_move(int x, int y);
  void mouse_press(Uint8 button, bool down = true);
  void mouse_wheel(int y, Uint32 orientation);
  void key_press(SDL_Keycode key, bool down = true);
 private:
  enum class KeyAction { move_left, move_right, move_up, move_down,
      screen_left, screen_right, screen_up, screen_down,
      mark, exout, zoom_in, zoom_out, next_control,
      open_menu, open_help };

  void set_key_mapping();
  void associate_key(KeyAction action, SDL_Keycode key);

  void move_selection(bool horizontal, int amount);

  bool is_point_in_grid(int x, int y) const;
  bool is_point_in_preview(int x, int y) const;
  
  Game* m_game;
  SDL_Window* m_window;
  
  enum class DragType { screen, preview, marks, exes, blank_marks, blank_exes };
  enum class MouseLockType { no_lock, to_row, to_col };

  std::map<SDL_Keycode, KeyAction> m_key_mapping;
  bool m_reverse_mouse;
  
  int m_mouse_x, m_mouse_y;
  int m_prev_mouse_x, m_prev_mouse_y;
  int m_prev_cell_x, m_prev_cell_y;
  bool m_mouse_dragging;
  DragType m_mouse_drag_type;
  MouseLockType m_mouse_lock_type;
  int m_mouse_lock_pos;
  int m_mouse_lock_time;
  bool m_kb_dragging;
  DragType m_kb_drag_type;

  int m_move_screen_horiz;
  int m_move_screen_vert;
  double m_movement_duration;
};

#endif
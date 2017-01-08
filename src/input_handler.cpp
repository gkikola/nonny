#include <cmath>
#include <map>
#include <vector>
#include <SDL2/SDL.h>

#include "game.h"
#include "puzzle.h"

#include "input_handler.h"

InputHandler::InputHandler(SDL_Window* window, Game* game)
  : m_game{game}, m_window{window}, m_reverse_mouse{false},
    m_mouse_x{0}, m_mouse_y{0}, m_prev_mouse_x{0}, m_prev_mouse_y{0},
    m_mouse_dragging{false}, m_kb_dragging{false},
    m_mouse_lock_type{MouseLockType::no_lock} {
  set_key_mapping();
}

void InputHandler::set_key_mapping() {
  set_default_controls();
}

void InputHandler::associate_key(KeyAction action, SDL_Keycode key) {
  m_key_mapping[key] = action;
}

void InputHandler::mouse_move(int x, int y) {
  int grid_x, grid_y;
  m_game->get_puzzle_coords(&grid_x, &grid_y);

  int grid_width = m_game->cell_grid_width();
  int grid_height = m_game->cell_grid_height();

  if (x >= grid_x && x < grid_x + grid_width
      && y >= grid_y && y < grid_y + grid_height) {
    int cell_x, cell_y;
    m_game->screen_coords_to_cell_coords(x, y, &cell_x, &cell_y);

    if (m_mouse_lock_type == MouseLockType::to_row)
      cell_y = m_mouse_lock_pos;
    else if (m_mouse_lock_type == MouseLockType::to_col)
      cell_x = m_mouse_lock_pos;

    m_game->select_cell(cell_x, cell_y);
  } else { //outside grid
    m_game->clear_selection();
  }

  if (m_mouse_dragging
      && (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MOUSE_CAPTURE)) {
    if (m_mouse_drag_type == DragType::screen) {
      m_game->move_puzzle(x - m_mouse_x, y - m_mouse_y);
    }

    m_mouse_x = x;
    m_mouse_y = y;

    if (m_mouse_drag_type != DragType::screen) {
      if (m_mouse_lock_type == MouseLockType::to_row)
        m_mouse_y = m_prev_mouse_y;
      else if (m_mouse_lock_type == MouseLockType::to_col)
        m_mouse_x = m_prev_mouse_x;

      int cell_x, cell_y;
      m_game->screen_coords_to_cell_coords(m_mouse_x, m_mouse_y,
                                           &cell_x, &cell_y);

      //lock mouse position if necessary
      int prev_x, prev_y;
      m_game->screen_coords_to_cell_coords(m_prev_mouse_x, m_prev_mouse_y,
                                           &prev_x, &prev_y);

      if (m_mouse_lock_type == MouseLockType::no_lock) {
        if (cell_x != prev_x && cell_y != prev_y) {
          m_prev_mouse_x = m_mouse_x;
          m_prev_mouse_y = m_mouse_y;
        } else if (cell_x == prev_x && abs(cell_y - prev_y) > 2) {
          m_mouse_lock_type = MouseLockType::to_col;
          m_mouse_lock_pos = cell_x;
        } else if (cell_y == prev_y && abs(cell_x - prev_x) > 2) {
          m_mouse_lock_type = MouseLockType::to_row;
          m_mouse_lock_pos = cell_y;
        }
      }

      //update cells
      CellState state;
      bool change_cell = false;
      if (m_mouse_drag_type == DragType::marks
          && m_game->puzzle().cell(cell_x, cell_y) == CellState::blank) {
        change_cell = true;
        state = CellState::marked;
      } else if (m_mouse_drag_type == DragType::exes
                 && m_game->puzzle().cell(cell_x, cell_y) == CellState::blank) {
        change_cell = true;
        state = CellState::exedout;
      } else if (m_mouse_drag_type == DragType::blank_marks
                 && m_game->puzzle().cell(cell_x, cell_y)
                 == CellState::marked) {
        change_cell = true;
        state = CellState::blank;
      } else if (m_mouse_drag_type == DragType::blank_exes
                 && m_game->puzzle().cell(cell_x, cell_y)
                 == CellState::exedout) {
        change_cell = true;
        state = CellState::blank;
      }

      if (change_cell)
        m_game->set_cell(cell_x, cell_y, state);
    }
  }
}

void InputHandler::mouse_press(Uint8 button, bool down) {
  if (m_reverse_mouse) {
    if (button = SDL_BUTTON_LEFT) button = SDL_BUTTON_RIGHT;
    else if (button = SDL_BUTTON_RIGHT) button = SDL_BUTTON_LEFT;
  }

  SDL_GetMouseState(&m_mouse_x, &m_mouse_y);
  
  int grid_x, grid_y;
  m_game->get_puzzle_coords(&grid_x, &grid_y);

  int grid_width = m_game->cell_grid_width();
  int grid_height = m_game->cell_grid_height();
  
  switch (button) {
  case SDL_BUTTON_LEFT:
    if (!down) {
      m_mouse_dragging = false;
      m_mouse_lock_type = MouseLockType::no_lock;
    }

    //see if mouse is inside the grid    
    if (m_mouse_x >= grid_x && m_mouse_x <= grid_x + grid_width
        && m_mouse_y >= grid_y && m_mouse_y <= grid_y + grid_height) {
      m_mouse_dragging = down;

      if (down) {
        SDL_CaptureMouse(SDL_TRUE);

        //determine what cell we are on
        int x, y;
        m_game->screen_coords_to_cell_coords(m_mouse_x, m_mouse_y, &x, &y);

        //keep track of mouse position for locking drags
        m_prev_mouse_x = m_mouse_x;
        m_prev_mouse_y = m_mouse_y;

        //if the cell is blank, mark it
        if (m_game->puzzle().cell(x, y) == CellState::blank) {
          m_game->set_cell(x, y, CellState::marked);
          m_mouse_drag_type = DragType::marks;
        } else if (m_game->puzzle().cell(x, y) == CellState::marked) {
          m_game->set_cell(x, y, CellState::blank);
          m_mouse_drag_type = DragType::blank_marks;
        } else {
          m_game->set_cell(x, y, CellState::blank);
          m_mouse_drag_type = DragType::blank_exes;
        }
      } else { //mouse button up
        SDL_CaptureMouse(SDL_FALSE);
      }

      break;
    }

    //outside the grid, do screen_dragging action:
  case SDL_BUTTON_MIDDLE:
    m_mouse_dragging = down;

    if (down) {
      m_mouse_drag_type = DragType::screen;
      SDL_CaptureMouse(SDL_TRUE);
    }
    else SDL_CaptureMouse(SDL_FALSE);
    break;
  case SDL_BUTTON_RIGHT:
    if (!down) {
      m_mouse_dragging = false;
      m_mouse_lock_type = MouseLockType::no_lock;
    }

    if (m_mouse_x >= grid_x && m_mouse_x <= grid_x + grid_width
        && m_mouse_y >= grid_y && m_mouse_y <= grid_y + grid_height) {
      m_mouse_dragging = down;

      if (down) {
        SDL_CaptureMouse(SDL_TRUE);

        //determine what cell we are on
        int x, y;
        m_game->screen_coords_to_cell_coords(m_mouse_x, m_mouse_y, &x, &y);

        //keep track of mouse position for locking drags
        m_prev_mouse_x = m_mouse_x;
        m_prev_mouse_y = m_mouse_y;
        m_mouse_lock_type = MouseLockType::no_lock;

        //if the cell is blank, ex it out
        if (m_game->puzzle().cell(x, y) == CellState::blank) {
          m_game->set_cell(x, y, CellState::exedout);
          m_mouse_drag_type = DragType::exes;
        } else if (m_game->puzzle().cell(x, y) == CellState::marked) {
          m_game->set_cell(x, y, CellState::blank);
          m_mouse_drag_type = DragType::blank_marks;
        } else {
          m_game->set_cell(x, y, CellState::blank);
          m_mouse_drag_type = DragType::blank_exes;
        }
      } else { //mouse button up
        SDL_CaptureMouse(SDL_FALSE);
      }
    }
    break;
  }
}

void InputHandler::mouse_wheel(int y, Uint32 orientation) {
  SDL_GetMouseState(&m_mouse_x, &m_mouse_y);

  if (orientation = SDL_MOUSEWHEEL_FLIPPED) {
    y = -y;
  }
  
  if (y < 0)
    m_game->zoom_in(m_mouse_x, m_mouse_y);
  else if (y > 0)
    m_game->zoom_out(m_mouse_x, m_mouse_y);
}

void InputHandler::key_press(SDL_Keycode key, bool down) {
  if (down) {
    //search for key in map
    auto iter = m_key_mapping.find(key);

    if (iter != m_key_mapping.end()) {
      KeyAction action = iter->second;
      switch (action) {
      case KeyAction::move_left:
        move_selection(true, -1);
        break;
      case KeyAction::move_right:
        move_selection(true, 1);
        break;
      case KeyAction::move_up:
        move_selection(false, -1);
        break;
      case KeyAction::move_down:
        move_selection(false, 1);
        break;
      }
    }
  }
}

void InputHandler::move_selection(bool horizontal, int amount) {
  int sel_x, sel_y;
  m_game->get_selected_cell(&sel_x, &sel_y);

  int* change = nullptr;
  int max = 0;

  if (horizontal) {
    change = &sel_x;
    max = m_game->puzzle().width() - 1;
  } else {
    change = &sel_y;
    max = m_game->puzzle().height() - 1;
  }

  if (m_game->is_cell_selected()) //only change selection if it's visible
    *change += amount;

  //do bounds checking on appropriate value
  if (*change < 0) {
    if (m_kb_dragging)
      *change = 0;
    else
      *change = max;
  } else if (*change > max) {
    if (m_kb_dragging)
      *change = max;
    else
      *change = 0;
  }

  m_game->select_cell(sel_x, sel_y);
}

void InputHandler::set_default_controls() {
  associate_key(KeyAction::move_left, SDLK_LEFT);
  associate_key(KeyAction::move_left, SDLK_KP_4);
  associate_key(KeyAction::move_right, SDLK_RIGHT);
  associate_key(KeyAction::move_right, SDLK_KP_6);
  associate_key(KeyAction::move_up, SDLK_UP);
  associate_key(KeyAction::move_up, SDLK_KP_8);
  associate_key(KeyAction::move_down, SDLK_DOWN);
  associate_key(KeyAction::move_down, SDLK_KP_2);

  associate_key(KeyAction::screen_left, SDLK_a);
  associate_key(KeyAction::screen_right, SDLK_d);
  associate_key(KeyAction::screen_up, SDLK_w);
  associate_key(KeyAction::screen_down, SDLK_s);

  associate_key(KeyAction::mark, SDLK_RETURN);
  associate_key(KeyAction::mark, SDLK_SPACE);
  associate_key(KeyAction::exout, SDLK_LCTRL);
  associate_key(KeyAction::exout, SDLK_RCTRL);

  associate_key(KeyAction::zoom_in, SDLK_PAGEUP);
  associate_key(KeyAction::zoom_out, SDLK_PAGEDOWN);

  associate_key(KeyAction::next_control, SDLK_TAB);
  
  associate_key(KeyAction::open_menu, SDLK_ESCAPE);
  associate_key(KeyAction::open_help, SDLK_F1);
}

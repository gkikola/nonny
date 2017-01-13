#include <cmath>
#include <iostream>
#include <map>
#include <vector>
#include <SDL2/SDL.h>

#include "control_container.h"
#include "game.h"
#include "puzzle.h"

#include "input_handler.h"

const int move_speed = 1024; //pixels per second
const int default_mouse_lock_time = 150; //ms

InputHandler::InputHandler(SDL_Window* window, Game* game)
  : m_game{game}, m_window{window}, m_reverse_mouse{false},
    m_mouse_x{0}, m_mouse_y{0}, m_prev_mouse_x{0}, m_prev_mouse_y{0},
    m_mouse_dragging{false}, m_kb_dragging{false},
    m_mouse_lock_type{MouseLockType::no_lock},
    m_move_screen_horiz{0}, m_move_screen_vert{0},
    m_movement_duration{0.0} {
  set_key_mapping();
}

void InputHandler::set_key_mapping() {
  set_default_controls();
}

void InputHandler::associate_key(KeyAction action, SDL_Keycode key) {
  m_key_mapping[key] = action;
}

void InputHandler::update(int elapsed_time) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  
  int cell_x, cell_y;
  m_game->screen_coords_to_cell_coords(mouse_x, mouse_y, &cell_x, &cell_y);
  
  //move the screen
  m_movement_duration += elapsed_time;
  int move_x = m_move_screen_horiz * (m_movement_duration / 1000);
  int move_y = m_move_screen_vert * (m_movement_duration / 1000);

  //accumulate time until there's enough for a movement
  if (move_x != 0 || move_y != 0) {
    m_movement_duration = 0;
    m_game->move_puzzle(move_x, move_y);
  }

  //release mouse lock if enough time has passed
  if (m_mouse_lock_type != MouseLockType::no_lock) {
    if (cell_x == m_prev_cell_x && cell_y == m_prev_cell_y)
      m_mouse_lock_time -= elapsed_time;
    else
      m_mouse_lock_time = default_mouse_lock_time;

    if (m_mouse_lock_time <= 0) {
      m_mouse_lock_time = 0;
      m_mouse_lock_type = MouseLockType::no_lock;
    }
  }

  m_game->screen_coords_to_cell_coords(mouse_x, mouse_y,
                                       &m_prev_cell_x, &m_prev_cell_y);
}

void InputHandler::mouse_move(int x, int y) {
  if (m_game->state() == GameState::main_menu) {
    mouse_move_controls(&m_game->main_menu(), x, y);
  } else if (m_game->state() == GameState::about) {
    mouse_move_controls(&m_game->about_menu(), x, y);
  } else if (m_game->state() == GameState::puzzle) {
    //make sure mouse doesn't skip over cells when moving fast
    if (m_mouse_dragging) {
      if (x - m_mouse_x > m_game->cell_size()) {
        mouse_move(x - m_game->cell_size(), y);
      } else if (m_mouse_x - x > m_game->cell_size()) {
        mouse_move(x + m_game->cell_size(), y);
      } else if (y - m_mouse_y > m_game->cell_size()) {
        mouse_move(x, y - m_game->cell_size());
      } else if (m_mouse_y - y > m_game->cell_size()) {
        mouse_move(x, y + m_game->cell_size());
      }
    }

    if (x < m_game->info_pane().width())
      mouse_move_controls(&m_game->info_pane(), x, y);

    int grid_x, grid_y;
    m_game->get_puzzle_coords(&grid_x, &grid_y);

    int grid_width = m_game->cell_grid_width();
    int grid_height = m_game->cell_grid_height();

    if (x >= m_game->info_pane().width()
        && x >= grid_x && x < grid_x + grid_width - 1
        && y >= grid_y && y < grid_y + grid_height - 1) {
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

      if (m_mouse_drag_type == DragType::control
          && is_point_in_preview(x, y))
        preview_jump();

      if (m_mouse_drag_type != DragType::screen
          && m_mouse_drag_type != DragType::control) {
        if (m_mouse_lock_type == MouseLockType::to_row)
          m_mouse_y = m_prev_mouse_y;
        else if (m_mouse_lock_type == MouseLockType::to_col)
          m_mouse_x = m_prev_mouse_x;

        int cell_x, cell_y;
        m_game->screen_coords_to_cell_coords(m_mouse_x, m_mouse_y,
                                             &cell_x, &cell_y);

        //reset in case there was a lock
        m_mouse_x = x;
        m_mouse_y = y;

        //lock mouse position if necessary
        int prev_x, prev_y;
        m_game->screen_coords_to_cell_coords(m_prev_mouse_x, m_prev_mouse_y,
                                             &prev_x, &prev_y);

        if (m_mouse_lock_type == MouseLockType::no_lock) {
          if (cell_x != prev_x && cell_y != prev_y) {
            m_prev_mouse_x = m_mouse_x;
            m_prev_mouse_y = m_mouse_y;
          } else if (cell_x == prev_x && abs(cell_y - prev_y) > 0) {
            m_mouse_lock_type = MouseLockType::to_col;
            m_mouse_lock_pos = cell_x;
            m_mouse_lock_time = default_mouse_lock_time;
          } else if (cell_y == prev_y && abs(cell_x - prev_x) > 0) {
            m_mouse_lock_type = MouseLockType::to_row;
            m_mouse_lock_pos = cell_y;
            m_mouse_lock_time = default_mouse_lock_time;
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
                   && m_game->puzzle().cell(cell_x, cell_y)
                   == CellState::blank) {
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
  } //if state = puzzle
}

void InputHandler::mouse_press(Uint8 button, bool down) {
  if (m_reverse_mouse) {
    if (button = SDL_BUTTON_LEFT) button = SDL_BUTTON_RIGHT;
    else if (button = SDL_BUTTON_RIGHT) button = SDL_BUTTON_LEFT;
  }

  SDL_GetMouseState(&m_mouse_x, &m_mouse_y);

  if (m_game->state() == GameState::main_menu) {
    mouse_press_controls(&m_game->main_menu(), button, down);
  } else if (m_game->state() == GameState::about) {
    mouse_press_controls(&m_game->about_menu(), button, down);
  } else if (m_game->state() == GameState::puzzle) {
    mouse_press_controls(&m_game->info_pane(), button, down);
    
    int grid_x, grid_y;
    m_game->get_puzzle_coords(&grid_x, &grid_y);

    int grid_width = m_game->cell_grid_width();
    int grid_height = m_game->cell_grid_height();
  
    switch (button) {
    case SDL_BUTTON_LEFT:
      if (!down) {
        m_mouse_dragging = false;
        m_mouse_lock_type = MouseLockType::no_lock;
        SDL_CaptureMouse(SDL_FALSE);
      }

      //see if mouse is inside the grid    
      if (is_point_in_grid(m_mouse_x, m_mouse_y)) {
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
      } else { //see if mouse is on the preview
        if (down && is_point_in_preview(m_mouse_x, m_mouse_y)) {
          m_mouse_drag_type = DragType::control;
          m_mouse_dragging = true;
          preview_jump();
          SDL_CaptureMouse(SDL_TRUE);
          break;
        }
      }

      //outside the grid, do screen_dragging action:
    case SDL_BUTTON_MIDDLE:
      if (!down) {
        m_mouse_dragging = false;
        SDL_CaptureMouse(SDL_FALSE);
      } else if (m_mouse_x > m_game->info_pane().width()) {
        m_mouse_dragging = true;
        m_mouse_drag_type = DragType::screen;
        SDL_CaptureMouse(SDL_TRUE);
      }
      break;
    case SDL_BUTTON_RIGHT:
      if (!down) {
        m_mouse_dragging = false;
        m_mouse_lock_type = MouseLockType::no_lock;
        SDL_CaptureMouse(SDL_FALSE);
      }

      if (is_point_in_grid(m_mouse_x, m_mouse_y)) {
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
  } //if game is in puzzle state and mouse is in play area
}

void InputHandler::mouse_wheel(int y, Uint32 orientation) {
  if (m_game->state() == GameState::puzzle) {
    SDL_GetMouseState(&m_mouse_x, &m_mouse_y);

    if (m_mouse_x > m_game->info_pane().width()) {
      if (orientation = SDL_MOUSEWHEEL_FLIPPED) {
        y = -y;
      }
  
      if (y < 0)
        m_game->zoom_in(m_mouse_x, m_mouse_y);
      else if (y > 0)
        m_game->zoom_out(m_mouse_x, m_mouse_y);
    }
  }
}

void InputHandler::key_press(SDL_Keycode key, bool down) {
  if (m_game->state() == GameState::main_menu) {
    key_press_controls(&m_game->main_menu(), key, down);
  } else if (m_game->state() == GameState::about) {
    key_press_controls(&m_game->about_menu(), key, down);
  } else if (m_game->state() == GameState::puzzle) {
    key_press_controls(&m_game->info_pane(), key, down);
  
    if (down) {
      //search for key in map
      auto iter = m_key_mapping.find(key);

      //get selected cell in case we need it
      int sel_x, sel_y;
      m_game->get_selected_cell(&sel_x, &sel_y);

      if (iter != m_key_mapping.end()) {
        KeyAction action = iter->second;
        switch (action) {
        case KeyAction::screen_left:
          m_move_screen_horiz = move_speed;
          m_movement_duration = 0.0;
          break;
        case KeyAction::screen_right:
          m_move_screen_horiz = -move_speed;
          m_movement_duration = 0.0;
          break;
        case KeyAction::screen_up:
          m_move_screen_vert = move_speed;
          m_movement_duration = 0.0;
          break;
        case KeyAction::screen_down:
          m_move_screen_vert = -move_speed;
          m_movement_duration = 0.0;
          break;
        case KeyAction::zoom_in:
        case KeyAction::zoom_out:
          {
            int x = (m_game->screen_width() + m_game->info_pane().width()) / 2;
            int y = m_game->screen_height() / 2;

            if (action == KeyAction::zoom_in)
              m_game->zoom_in(x, y);
            else if (action == KeyAction::zoom_out)
              m_game->zoom_out(x, y);
          }
          break;
        case KeyAction::move_left:
          move_selection(true, -1);
          m_game->info_pane().clear_selection();
          break;
        case KeyAction::move_right:
          move_selection(true, 1);
          m_game->info_pane().clear_selection();
          break;
        case KeyAction::move_up:
          move_selection(false, -1);
          m_game->info_pane().clear_selection();
          break;
        case KeyAction::move_down:
          move_selection(false, 1);
          m_game->info_pane().clear_selection();
          break;
        case KeyAction::mark:
        case KeyAction::mark_alt:
          if (m_game->is_cell_selected() && !m_kb_dragging) {
            m_kb_dragging = true;

            CellState state;
            bool change_state = false;
            if (m_game->puzzle().cell(sel_x, sel_y) == CellState::blank) {
              m_kb_drag_type = DragType::marks;
              state = CellState::marked;
              change_state = true;
            } else if (m_game->puzzle().cell(sel_x, sel_y)
                       == CellState::marked) {
              m_kb_drag_type = DragType::blank_marks;
              state = CellState::blank;
              change_state = true;
            } else if (m_game->puzzle().cell(sel_x, sel_y)
                       == CellState::exedout) {
              m_kb_drag_type = DragType::blank_exes;
              state = CellState::blank;
              change_state = true;
            }

            if (change_state)
              m_game->set_cell(sel_x, sel_y, state);
          } else { //no selection
            //is an info pane button selected?
            Control* control = m_game->info_pane().selection();
            
            if (control == nullptr)
              m_game->select_cell(sel_x, sel_y); //if not, show cell selection
          }
          break;
        case KeyAction::exout:
          if (m_game->is_cell_selected() && !m_kb_dragging) {
            m_kb_dragging = true;

            CellState state;
            bool change_state = false;
            if (m_game->puzzle().cell(sel_x, sel_y) == CellState::blank) {
              m_kb_drag_type = DragType::exes;
              state = CellState::exedout;
              change_state = true;
            } else if (m_game->puzzle().cell(sel_x, sel_y)
                       == CellState::marked) {
              m_kb_drag_type = DragType::blank_marks;
              state = CellState::blank;
              change_state = true;
            } else if (m_game->puzzle().cell(sel_x, sel_y)
                       == CellState::exedout) {
              m_kb_drag_type = DragType::blank_exes;
              state = CellState::blank;
              change_state = true;
            }

            if (change_state)
              m_game->set_cell(sel_x, sel_y, state);
          } else { //no selection
            Control* control = m_game->info_pane().selection();

            if (control == nullptr)
              m_game->select_cell(sel_x, sel_y);
          }
          break;
        }

        if (m_kb_dragging) {
          if (action == KeyAction::move_left || action == KeyAction::move_right
              || action == KeyAction::move_up
              || action == KeyAction::move_down) {
            CellState state;
            bool change_state = false;
            m_game->get_selected_cell(&sel_x, &sel_y);

            switch (m_kb_drag_type) {
            case DragType::marks:
              if (m_game->puzzle().cell(sel_x, sel_y) == CellState::blank) {
                state = CellState::marked;
                change_state = true;
              }
              break;
            case DragType::exes:
              if (m_game->puzzle().cell(sel_x, sel_y) == CellState::blank) {
                state = CellState::exedout;
                change_state = true;
              }
              break;
            case DragType::blank_marks:
              if (m_game->puzzle().cell(sel_x, sel_y) == CellState::marked) {
                state = CellState::blank;
                change_state = true;
              }
              break;
            case DragType::blank_exes:
              if (m_game->puzzle().cell(sel_x, sel_y) == CellState::exedout) {
                state = CellState::blank;
                change_state = true;
              }
              break;
            } //switch

            if (change_state)
              m_game->set_cell(sel_x, sel_y, state);
          } //if action is move
        } //if m_kb_dragging

        //move camera if selection is offscreen
        if (action == KeyAction::move_left || action == KeyAction::move_right
            || action == KeyAction::move_up || action == KeyAction::move_down
            || action == KeyAction::mark || action == KeyAction::exout) {
          m_game->make_selected_cell_visible();
        } //if action involves cell selection
      } //if key found in map
    } else { //key is up
      auto iter = m_key_mapping.find(key);

      if (iter != m_key_mapping.end()) {
        KeyAction action = iter->second;

        switch (action) {
        case KeyAction::screen_left:
        case KeyAction::screen_right:
          m_move_screen_horiz = 0;
          break;
        case KeyAction::screen_up:
        case KeyAction::screen_down:
          m_move_screen_vert = 0;
          break;
        case KeyAction::mark:
        case KeyAction::mark_alt:
        case KeyAction::exout:
          m_kb_dragging = false;
          break;
        } //switch
      } //if key mapping was found
    } //if key is up
  } //if game is in puzzle state
}

void InputHandler::preview_jump() {
  int preview_x, preview_y;
  m_game->info_pane().get_preview_position(&preview_x, &preview_y);
  int preview_w, preview_h;
  m_game->info_pane().get_preview_size(&preview_w, &preview_h);

  double x_proportion = (m_mouse_x - preview_x) / (double)preview_w;
  double y_proportion = (m_mouse_y - preview_y) / (double)preview_y;

  int center_x, center_y;
  center_x = std::round(x_proportion * m_game->cell_grid_width());
  center_y = std::round(y_proportion * m_game->cell_grid_height());

  int new_x, new_y;
  new_x = (m_game->screen_width()
           + m_game->info_pane().width())/ 2 - center_x;
  new_y = m_game->screen_height() / 2 - center_y;

  int old_x, old_y;
  m_game->get_puzzle_coords(&old_x, &old_y);
          
  m_game->move_puzzle(new_x - old_x, new_y - old_y);
}

bool InputHandler::is_point_in_grid(int x, int y) const {
  int grid_x, grid_y;
  m_game->get_puzzle_coords(&grid_x, &grid_y);
  int grid_width = m_game->cell_grid_width();
  int grid_height =  m_game->cell_grid_height();
  
  return (x > m_game->info_pane().width()
          && x >= grid_x && x <= grid_x + grid_width
          && y >= grid_y && y <= grid_y + grid_height);
}

bool InputHandler::is_point_in_preview(int x, int y) const {
  int preview_x, preview_y;
  m_game->info_pane().get_preview_position(&preview_x, &preview_y);
  int preview_w, preview_h;
  m_game->info_pane().get_preview_size(&preview_w, &preview_h);

  return (x >= preview_x && x <= preview_x + preview_w
          && y >= preview_y && y <= preview_y + preview_h);
}

bool InputHandler::is_point_in_control(int x, int y, Control* control) const {
  int cx, cy;
  control->get_position(&cx, &cy);
  int cwidth, cheight;
  control->get_size(&cwidth, &cheight);

  return (x >= cx && x <= cx + cwidth
          && y >= cy && y <= cy + cheight);
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

void InputHandler::mouse_move_controls(ControlContainer* controls,
                                       int x, int y) {
  for (Control* control : *controls) {
    control->mouse_move(x, y);
  }
}

void InputHandler::mouse_press_controls(ControlContainer* controls,
                                        Uint8 button, bool down) {
  for (Control* control : *controls) {
    switch (button) {
    case SDL_BUTTON_LEFT:
      control->mouse_press(Control::MouseAction::left,
                           m_mouse_x, m_mouse_y, down);
      break;
    case SDL_BUTTON_MIDDLE:
      control->mouse_press(Control::MouseAction::middle,
                           m_mouse_x, m_mouse_y, down);
      break;
    case SDL_BUTTON_RIGHT:
      control->mouse_press(Control::MouseAction::right,
                           m_mouse_x, m_mouse_y, down);
      break;
    }
  }
}

void InputHandler::key_press_controls(ControlContainer* controls,
                                      SDL_Keycode key, bool down) {
  auto iter = m_key_mapping.find(key);

  if (iter != m_key_mapping.end()) {
    KeyAction action = iter->second;
    switch (action) {
    case KeyAction::next_control:
      if (down) {
        //if we're in puzzle mode, hide the grid selection
        if (m_game->state() == GameState::puzzle)
          m_game->clear_selection();
        
        if (SDL_GetModState() & KMOD_SHIFT)
          controls->select_next(true);
        else
          controls->select_next(false);
      }
      break;
    case KeyAction::move_left:
    case KeyAction::move_up:
      if (down && m_game->state() != GameState::puzzle)
        controls->select_next(true);
      break;
    case KeyAction::move_right:
    case KeyAction::move_down:
      if (down && m_game->state() != GameState::puzzle)
        controls->select_next(false);
      break;
    case KeyAction::mark:
      for (Control* control : *controls)
        control->key_press(Control::KeyAction::activate, down);
      break;
    case KeyAction::mark_alt:
      for (Control* control : *controls)
        control->key_press(Control::KeyAction::depress, down);
      break;
    }
  }
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
  associate_key(KeyAction::mark, SDLK_KP_ENTER);
  associate_key(KeyAction::mark_alt, SDLK_SPACE);
  associate_key(KeyAction::exout, SDLK_LCTRL);
  associate_key(KeyAction::exout, SDLK_RCTRL);

  associate_key(KeyAction::zoom_in, SDLK_PAGEUP);
  associate_key(KeyAction::zoom_out, SDLK_PAGEDOWN);

  associate_key(KeyAction::next_control, SDLK_TAB);
  
  associate_key(KeyAction::open_menu, SDLK_ESCAPE);
  associate_key(KeyAction::open_help, SDLK_F1);
}

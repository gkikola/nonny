/* Nonny -- Play and create nonogram puzzles.
 * Copyright (C) 2017 Gregory Kikola.
 *
 * This file is part of Nonny.
 *
 * Nonny is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nonny is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nonny.  If not, see <http://www.gnu.org/licenses/>.
 */
/* Written by Gregory Kikola <gkikola@gmail.com>. */

#ifndef NONNY_INPUT_HANDLER_HPP
#define NONNY_INPUT_HANDLER_HPP

#include <vector>
#include "input/key.hpp"
#include "video/point.hpp"

class InputHandler {
public:
  InputHandler();
  virtual ~InputHandler() { }

  void update(unsigned ticks);
  
  void process_key_event(Keyboard::Key key, bool down);
  void process_mouse_button_event(Mouse::Button button, bool down);
  void process_mouse_wheel_event(int vert, int horiz);
  void process_mouse_move_event(int x, int y);

  bool was_key_pressed(Keyboard::Key key);
  bool was_key_released(Keyboard::Key key);
  bool is_key_down(Keyboard::Key key);

  bool was_mouse_button_pressed(Mouse::Button button);
  bool was_mouse_button_released(Mouse::Button button);
  bool is_mouse_button_down(Mouse::Button button);

private:
  Point m_mouse;
  Point m_prev_mouse;
  
  std::vector<bool> m_keys;
  std::vector<bool> m_prev_keys;
  bool m_need_key_update = false;

  std::vector<bool> m_buttons;
  std::vector<bool> m_prev_buttons;
  bool m_need_button_update = false;
};

#endif

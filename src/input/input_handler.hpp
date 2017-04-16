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

#include "input/key.hpp"

class InputHandler {
public:
  InputHandler() { }
  virtual ~InputHandler() { }

  void process_key_event(Keyboard::Key key, bool down);
  void process_mouse_button_event(Mouse::Button button, bool down);
  void process_mouse_wheel_event(int vert, int horiz);
  void process_mouse_move_event(int x, int y);

private:
  int m_mouse_x = 0;
  int m_mouse_y = 0;
  int m_mouse_rel_x = 0;
  int m_mouse_rel_y = 0;
};

#endif

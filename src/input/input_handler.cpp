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

#include "input/input_handler.hpp"

InputHandler::InputHandler()
  : m_mouse(0, 0),
    m_prev_mouse(0, 0),
    m_keys(Keyboard::num_keys, false),
    m_prev_keys(Keyboard::num_keys, false),
    m_buttons(Mouse::num_buttons, false),
    m_prev_buttons(Mouse::num_buttons, false)
{
}

void InputHandler::update(unsigned ticks)
{
  if (m_need_key_update) {
    m_need_key_update = false;
    m_prev_keys.clear();
    m_prev_keys.insert(m_prev_keys.end(), Keyboard::num_keys, false);
    m_keys.swap(m_prev_keys);
  }

  if (m_need_button_update) {
    m_need_button_update = false;
    m_prev_buttons.clear();
    m_prev_buttons.insert(m_prev_buttons.end(), Mouse::num_buttons, false);
    m_buttons.swap(m_prev_buttons);
  }
}

void InputHandler::process_key_event(Keyboard::Key key, bool down)
{
  m_keys[key] = down;
  m_need_key_update = true;
}

void InputHandler::process_mouse_button_event(Mouse::Button button, bool down)
{
  m_buttons[button] = down;
  m_need_button_update = true;
}

void InputHandler::process_mouse_wheel_event(int vert, int horiz)
{
}

void InputHandler::process_mouse_move_event(int x, int y)
{
  m_prev_mouse = m_mouse;
  m_mouse = Point(x, y);
}

bool InputHandler::was_key_pressed(Keyboard::Key key)
{
  return !m_prev_keys[key] && m_keys[key];
}

bool InputHandler::was_key_released(Keyboard::Key key)
{
  return m_prev_keys[key] && !m_keys[key];
}

bool InputHandler::is_key_down(Keyboard::Key key)
{
  return m_keys[key];
}

bool InputHandler::was_mouse_button_pressed(Mouse::Button button)
{
  return !m_prev_buttons[button] && m_buttons[button];
}

bool InputHandler::was_mouse_button_released(Mouse::Button button)
{
  return m_prev_buttons[button] && !m_buttons[button];
}

bool InputHandler::is_mouse_button_down(Mouse::Button button)
{
  return m_buttons[button];
}

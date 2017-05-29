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

#include <stdexcept>
#include "config.h"

#ifdef NONNY_INPUT_SDL
#include "input/sdl/sdl_input_handler.hpp"
#endif

InputHandler::InputHandler()
  : m_mouse(0, 0),
    m_prev_mouse(0, 0),
    m_keys(Keyboard::num_keys, false),
    m_prev_keys(Keyboard::num_keys, false),
    m_num_presses(Keyboard::num_keys, 0),
    m_buttons(Mouse::num_buttons, false),
    m_prev_buttons(Mouse::num_buttons, false),
    m_button_dbl_click(Mouse::num_buttons, false)
{
}

std::unique_ptr<InputHandler> InputHandler::create()
{
#ifdef NONNY_INPUT_SDL
  return std::make_unique<SDLInputHandler>();
#else
  throw std::runtime_error("InputHandler::create: "
                           "input handler not implemented");
#endif
}

void InputHandler::update(unsigned ticks)
{
  m_prev_keys = m_keys;
  m_prev_buttons = m_buttons;
  m_prev_mouse = m_mouse;
  m_mwheel_vert = 0;
  m_mwheel_horiz = 0;
  for (auto it = m_button_dbl_click.begin();
       it != m_button_dbl_click.end();
       ++it)
    *it = false;
  m_num_presses.clear();
  m_num_presses.insert(m_num_presses.end(), Keyboard::num_keys, 0);
  m_characters = "";
}

void InputHandler::process_key_event(Keyboard::Key key, bool down)
{
  m_keys[key] = down;
  if (down)
    ++m_num_presses[key];
}

void InputHandler::process_text_input_event(const std::string& text)
{
  m_characters += text;
}

void InputHandler::process_mouse_button_event(Mouse::Button button, bool down,
                                              int num_clicks)
{
  m_buttons[button] = down;

  if (down && num_clicks > 1)
    m_button_dbl_click[button] = true;
  else if (num_clicks == 1)
    m_button_dbl_click[button] = false;
}

void InputHandler::process_mouse_wheel_event(int vert, int horiz)
{
  m_mwheel_vert += vert;
  m_mwheel_horiz += horiz;
}

void InputHandler::process_mouse_move_event(int x, int y)
{
  m_mouse = Point(x, y);
}

bool InputHandler::was_key_pressed(Keyboard::Key key) const
{
  return !m_prev_keys[key] && m_keys[key];
}

bool InputHandler::was_key_released(Keyboard::Key key) const
{
  return m_prev_keys[key] && !m_keys[key];
}

bool InputHandler::is_key_down(Keyboard::Key key) const
{
  return m_keys[key];
}

int InputHandler::num_key_presses(Keyboard::Key key) const
{
  return m_num_presses[key];
}

bool InputHandler::is_ctrl_down() const
{
  return is_key_down(Keyboard::Key::lctrl)
    || is_key_down(Keyboard::Key::rctrl);
}

bool InputHandler::is_shift_down() const
{
  return is_key_down(Keyboard::Key::lshift)
    || is_key_down(Keyboard::Key::rshift);
}

bool InputHandler::is_alt_down() const
{
  return is_key_down(Keyboard::Key::lalt)
    || is_key_down(Keyboard::Key::ralt);
}

bool InputHandler::was_mouse_button_pressed(Mouse::Button button) const
{
  return !m_prev_buttons[button] && m_buttons[button];
}

bool InputHandler::was_mouse_button_released(Mouse::Button button) const
{
  return m_prev_buttons[button] && !m_buttons[button];
}

bool InputHandler::is_mouse_button_down(Mouse::Button button) const
{
  return m_buttons[button];
}

bool InputHandler::was_mouse_button_double_clicked(Mouse::Button button) const
{
  return m_button_dbl_click[button];
}

bool InputHandler::was_mouse_moved() const
{
  return rel_mouse_x() || rel_mouse_y();
}

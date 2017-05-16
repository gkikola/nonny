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

#include <memory>
#include <string>
#include <vector>
#include "input/key.hpp"
#include "video/point.hpp"

class InputHandler {
public:
  InputHandler();
  virtual ~InputHandler() { }

  InputHandler(const InputHandler&) = delete;
  InputHandler& operator=(const InputHandler&) = delete;

  static std::unique_ptr<InputHandler> create();

  virtual void update(unsigned ticks);

  virtual void capture_mouse() = 0;
  virtual void release_mouse() = 0;

  virtual void set_cursor(Mouse::Cursor cursor) = 0;
  virtual void reset_cursor() = 0;
  virtual Mouse::Cursor cursor() const = 0;
  
  virtual void process_key_event(Keyboard::Key key, bool down);
  virtual void process_text_input_event(const std::string& text);
  virtual void process_mouse_button_event(Mouse::Button button, bool down,
                                          unsigned num_clicks = 1);
  virtual void process_mouse_wheel_event(int vert, int horiz);
  virtual void process_mouse_move_event(int x, int y);

  virtual bool was_key_pressed(Keyboard::Key key) const;
  virtual bool was_key_released(Keyboard::Key key) const;
  virtual bool is_key_down(Keyboard::Key key) const;
  virtual unsigned num_key_presses(Keyboard::Key key) const;

  std::string chars_entered() const { return m_characters; }

  virtual bool was_mouse_button_pressed(Mouse::Button button) const;
  virtual bool was_mouse_button_released(Mouse::Button button) const;
  virtual bool is_mouse_button_down(Mouse::Button button) const;
  virtual bool was_mouse_button_double_clicked(Mouse::Button button) const;

  virtual bool was_mouse_moved() const;
  virtual Point mouse_position() const { return m_mouse; }
  virtual Point prev_mouse_position() const { return m_prev_mouse; }
  virtual int rel_mouse_x() const { return m_mouse.x() - m_prev_mouse.x(); }
  virtual int rel_mouse_y() const { return m_mouse.y() - m_prev_mouse.y(); }

  virtual int vert_mouse_wheel_scroll() const { return m_mwheel_vert; }
  virtual int horiz_mouse_wheel_scroll() const { return m_mwheel_horiz; }
  
private:
  Point m_mouse;
  Point m_prev_mouse;
  int m_mwheel_vert = 0;
  int m_mwheel_horiz = 0;
  
  std::vector<bool> m_keys;
  std::vector<bool> m_prev_keys;
  std::vector<unsigned> m_num_presses;

  std::vector<bool> m_buttons;
  std::vector<bool> m_prev_buttons;
  std::vector<bool> m_button_dbl_click;

  std::string m_characters;
};

#endif

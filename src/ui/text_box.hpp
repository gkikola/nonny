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

#ifndef NONNY_TEXT_BOX_HPP
#define NONNY_TEXT_BOX_HPP

#include "ui/control.hpp"

class Font;

/*
 * A text box control.
 */
class TextBox : public Control {
public:
  TextBox() = default;
  TextBox(const Font& font) : m_font(&font) { calc_size(); }

  const std::string& get_text() const { return m_text; }
  void set_text(const std::string& text);

  void select_all();
  
  using UIPanel::update;
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;
private:
  void calc_size();
  int pos_to_screen_coord(int pos) const;
  int screen_coord_to_pos(int x) const;
  void read_chars(InputHandler& input);
  void handle_arrow_keys(InputHandler& input);
  void handle_delete_keys(InputHandler& input);
  void handle_home_end(InputHandler& input);
  void handle_mouse_input(InputHandler& input, const Rect& region);
  
  const Font* m_font = nullptr;
  std::string m_text;
  int m_cursor = 0;
  int m_visible = 0;
  int m_sel_start = 0;
  int m_sel_length = 0;
  bool m_dragging_sel = false;
  int m_drag_start_pos = 0;
  bool m_cursor_visible = true;
  int m_cursor_duration = 0;
};

#endif

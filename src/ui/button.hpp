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

#ifndef NONNY_BUTTON_HPP
#define NONNY_BUTTON_HPP

#include <functional>
#include <string>
#include "ui/control.hpp"

class Font;

class Button : public Control {
public:
  Button() = default;
  explicit Button(const Font& font)
    : m_font(&font) { calc_size(); }
  Button(const Font& font, const std::string& label)
    : m_font(&font), m_label(label) { calc_size(); }
  virtual ~Button() { }

  Button(const Button&) = default;
  Button(Button&&) = default;
  Button& operator=(const Button&) = default;
  Button& operator=(Button&&) = default;

  void register_callback(std::function<void()> fn) { m_operation = fn; }
  void press() { if (m_operation) m_operation(); }

  using UIPanel::update; //make update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

protected:
  void draw_background(Renderer& renderer) const;
  void draw_sel_rect(Renderer& renderer) const;
  void draw_label(Renderer& renderer) const;

  bool m_depressed = false;
  bool m_mouse_hover = false;

private:
  void calc_size();

  std::function<void()> m_operation;
  const Font* m_font = nullptr;
  std::string m_label;
  int m_label_width = 0;
};

#endif

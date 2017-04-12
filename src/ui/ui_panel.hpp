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

#ifndef NONNY_UI_PANEL_HPP
#define NONNY_UI_PANEL_HPP

#include <memory>
#include <vector>
#include "main/utility.hpp"

class InputHandler;
class Renderer;

class UIPanel {
public:
  UIPanel() { }
  UIPanel(const Rect& boundary) : m_boundary(boundary) { }
  virtual ~UIPanel() { }

  virtual void scroll(int x, int y);
  virtual void set_visible(const Rect& visible);
  
  virtual void update(unsigned ticks, InputHandler& input);
  virtual void draw(Renderer& renderer) const;

  inline void add_child(std::shared_ptr<UIPanel> child);

protected:
  Rect m_boundary;
  Rect m_visible;
  std::vector<std::shared_ptr<UIPanel>> m_children;
};


/* implementation */

inline void UIPanel::add_child(std::shared_ptr<UIPanel> child)
{
  m_children.push_back(child);
}

#endif

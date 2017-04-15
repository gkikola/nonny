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

#ifndef NONNY_SCROLLBAR_HPP
#define NONNY_SCROLLBAR_HPP

#include "ui/ui_panel.hpp"
#include "utility/utility.hpp"

class Scrollbar : public UIPanel {
public:
  explicit Scrollbar(UIPanelPtr target, bool vertical = true)
    : m_scroll_target(target), m_vertical(vertical) { }
  Scrollbar(const Rect& boundary, UIPanelPtr target, bool vertical = true)
    : UIPanel(boundary), m_scroll_target(target), m_vertical(vertical) { }
  
  void update(unsigned ticks, InputHandler& input);
  void draw(Renderer& renderer) const;
private:
  UIPanelPtr m_scroll_target;
  Point m_scroll_pos;
  bool m_vertical;
};

#endif

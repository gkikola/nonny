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

#include "ui/ui_panel.hpp"

#include "input/input_handler.hpp"
#include "main/utility.hpp"
#include "video/renderer.hpp"

void UIPanel::scroll(int x, int y)
{
  m_boundary.x += x;
  m_boundary.y += y;

  for (auto pchild : m_children)
    pchild->scroll(x, y);
}

void UIPanel::set_visible(const Rect& visible)
{
  m_visible = intersection(visible, m_boundary);
  for (auto pchild : m_children)
    pchild->set_visible(m_visible);
}

void UIPanel::update(unsigned ticks, InputHandler& input)
{
  for (auto pchild : m_children)
    pchild->update(ticks, input);
}

void UIPanel::draw(Renderer& renderer) const
{
  for (auto pchild : m_children)
    pchild->draw(renderer);
}

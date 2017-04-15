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
#include "utility/utility.hpp"
#include "video/renderer.hpp"

void UIPanel::scroll(int x, int y)
{
  m_boundary.x += x;
  m_boundary.y += y;
}

void UIPanel::set_visible_region(const Rect& visible)
{
  m_visible = intersection(visible, m_boundary);
}

void UIPanel::move(int x, int y)
{
  m_boundary.x = x;
  m_boundary.y = y;
}

void UIPanel::resize(unsigned width, unsigned height)
{
  m_boundary.width = width;
  m_boundary.height = height;
}

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

#include "ui/scrolling_panel.hpp"

#include "input/input_handler.hpp"
#include "video/renderer.hpp"

constexpr unsigned scrollbar_width = 16;

void ScrollingPanel::attach_panel(std::shared_ptr<UIPanel> child)
{
  m_main_panel = child;
}

void ScrollingPanel::update(unsigned ticks, InputHandler& input)
{
}

void ScrollingPanel::draw(Renderer& renderer) const
{
}

void ScrollingPanel::draw_scrollbar(Renderer& renderer, bool vertical) const
{
}

void ScrollingPanel::resize(unsigned width, unsigned height)
{
  UIPanel::resize(width, height);
}

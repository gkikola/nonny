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
  m_hscroll = Scrollbar(child, false);
  m_vscroll = Scrollbar(child, true);
  resize(m_boundary.width(), m_boundary.height());
}

void ScrollingPanel::update(unsigned ticks, InputHandler& input,
                            const Rect& active_region)
{
  if (m_main_panel)
    m_main_panel->update(ticks, input, active_region);
  m_hscroll.update(ticks, input, active_region);
  m_vscroll.update(ticks, input, active_region);
}

void ScrollingPanel::draw(Renderer& renderer, const Rect& region) const
{
  if (m_main_panel)
    m_main_panel->draw(renderer, region);
  
  m_hscroll.draw(renderer, region);
  m_vscroll.draw(renderer, region);
}

void ScrollingPanel::resize(unsigned width, unsigned height)
{
  UIPanel::resize(width, height);

  //figure out if we need scrollbars
  bool need_horz = false;
  bool need_vert = false;
  if (m_main_panel) {
    if (m_main_panel->boundary().width() > width)
      need_horz = true;
    if (m_main_panel->boundary().height() > height)
      need_vert = true;
  }

  //move scrollbars into position
  unsigned adjusted_width = m_boundary.width();
  unsigned adjusted_height = m_boundary.height();
  if (need_horz)
    adjusted_height -= scrollbar_width;
  if (need_vert)
    adjusted_width -= scrollbar_width;
  m_hscroll.move(m_boundary.x(),
                 m_boundary.y() + m_boundary.height() - scrollbar_width);
  m_hscroll.resize(adjusted_width, scrollbar_width);
  m_vscroll.move(m_boundary.x() + m_boundary.width() - scrollbar_width,
                 m_boundary.y());
  m_vscroll.resize(scrollbar_width, adjusted_height);
}

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
  if (!m_children.empty())
    m_children.clear();

  m_main_panel = child;
  m_children.push_back(child);
  m_scroll_pos = Point{ 0, 0 };
  set_child_visibility();
}

void ScrollingPanel::draw(Renderer& renderer) const
{
  m_main_panel->draw(renderer);

  if (m_hscroll)
    draw_scrollbar(renderer, false);
  if (m_vscroll)
    draw_scrollbar(renderer, true);
}

void ScrollingPanel::draw_scrollbar(Renderer& renderer, bool vertical) const
{
  Rect area;
  renderer.set_draw_color(Color(196, 196, 196));
  
  if (vertical) {
    area.x = m_boundary.x + m_boundary.width - scrollbar_width;
    area.y = m_boundary.y;
    area.width = scrollbar_width;
    area.height = m_boundary.height;
    if (m_hscroll) area.height -= scrollbar_width;
  } else {
    area.x = m_boundary.x;
    area.y = m_boundary.y + m_boundary.height - scrollbar_width;
    area.width = m_boundary.width;
    if (m_vscroll) area.width -= scrollbar_width;
    area.height = scrollbar_width;
  }
  renderer.fill_rect(area);
}

void ScrollingPanel::resize(unsigned width, unsigned height)
{
  UIPanel::resize(width, height);
  set_child_visibility();
}

void ScrollingPanel::set_child_visibility()
{
  const Rect& child_bound = m_main_panel->boundary();
  const Rect& child_vis = m_main_panel->visible();

  //do we need scrollbars?
  m_hscroll = m_vscroll = false;
  if (child_bound.width > m_boundary.width)
    m_hscroll = true;
  if (child_bound.height > m_boundary.height)
    m_vscroll = true;

  //set visible region to whole panel minus scrollbar area
  Rect new_visible = m_boundary;
  if (m_hscroll) new_visible.height -= scrollbar_width;
  if (m_vscroll) new_visible.width -= scrollbar_width;
  new_visible = intersection(child_vis, new_visible);
  m_main_panel->set_visible(new_visible);
}

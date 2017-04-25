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

#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "video/renderer.hpp"

constexpr unsigned scrollbar_width = 16;
const Color scrollbar_background_color(196, 196, 196);

void ScrollingPanel::attach_panel(UIPanelPtr child)
{
  m_main_panel = child;
  m_hscroll = Scrollbar(child, false);
  m_vscroll = Scrollbar(child, true);
  resize(m_boundary.width(), m_boundary.height());
  center_panel_horiz();
  center_panel_vert();
}

void ScrollingPanel::update(unsigned ticks, InputHandler& input,
                            const Rect& active_region)
{
  Rect main_region = m_boundary;
  if (m_hscroll_active) {
    m_hscroll.update(ticks, input, active_region);
    main_region.height() -= scrollbar_width;
  }
  if (m_vscroll_active) {
    m_vscroll.update(ticks, input, active_region);
    main_region.width() -= scrollbar_width;
  }

  if (m_main_panel)
    m_main_panel->update(ticks, input,
                         intersection(main_region, active_region));
}

void ScrollingPanel::draw(Renderer& renderer, const Rect& region) const
{
  if (m_main_panel)
    m_main_panel->draw(renderer, region);

  if (m_hscroll_active)
    m_hscroll.draw(renderer, region);
  if (m_vscroll_active)
    m_vscroll.draw(renderer, region);

  //fill in the little square in the corner that separates the scrollbars
  if (m_hscroll_active && m_vscroll_active) {
    Rect rect(m_boundary.x() + m_boundary.width()
              - m_vscroll.boundary().width(),
              m_boundary.y() + m_boundary.height()
              - m_hscroll.boundary().height(),
              m_vscroll.boundary().width(),
              m_hscroll.boundary().height());
    rect = intersection(rect, region);
    renderer.set_draw_color(scrollbar_background_color);
    renderer.fill_rect(rect);
  }
}

void ScrollingPanel::resize(unsigned width, unsigned height)
{
  UIPanel::resize(width, height);

  //figure out if we need scrollbars
  m_hscroll_active = false;
  m_vscroll_active = false;
  if (m_main_panel) {
    if (m_main_panel->boundary().height() > height)
      m_vscroll_active = true;
    if (m_main_panel->boundary().width() > width)
      m_hscroll_active = true;
  }

  //move scrollbars into position
  unsigned adjusted_width = m_boundary.width();
  unsigned adjusted_height = m_boundary.height();
  if (m_hscroll_active)
    adjusted_height -= scrollbar_width;
  if (m_vscroll_active)
    adjusted_width -= scrollbar_width;
  m_hscroll.move(m_boundary.x(),
                 m_boundary.y() + m_boundary.height() - scrollbar_width);
  m_hscroll.resize(adjusted_width, scrollbar_width);
  m_vscroll.move(m_boundary.x() + m_boundary.width() - scrollbar_width,
                 m_boundary.y());
  m_vscroll.resize(scrollbar_width, adjusted_height);

  //center panel if it's smaller than the scrolling panel
  if (!m_hscroll_active)
    center_panel_horiz();
  else if(m_main_panel->boundary().x() > m_boundary.x())
    m_main_panel->move(m_boundary.x(), m_main_panel->boundary().y());
  if (!m_vscroll_active)
    center_panel_vert();
  else if(m_main_panel->boundary().y() > m_boundary.y())
    m_main_panel->move(m_main_panel->boundary().x(), m_boundary.y());
}

void ScrollingPanel::center_panel_vert()
{
  m_main_panel->move(m_main_panel->boundary().x(),
                     m_boundary.y()
                     + static_cast<int>(m_boundary.height() / 2)
                     - static_cast<int>(m_main_panel->boundary().height()
                                        / 2));
}

void ScrollingPanel::center_panel_horiz()
{
  m_main_panel->move(m_boundary.x()
                     + static_cast<int>(m_boundary.width() / 2)
                     - static_cast<int>(m_main_panel->boundary().width() / 2),
                     m_main_panel->boundary().y());
}

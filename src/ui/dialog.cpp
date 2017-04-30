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

#include "ui/dialog.hpp"

void Dialog::add_control(ControlPtr control)
{
  m_controls.push_back(control);
  m_need_reposition = true;
}

void Dialog::focus_prev()
{
  if (m_focused == m_controls.begin())
    m_focused = m_controls.end();

  if (m_focused != m_controls.begin())
    --m_focused;
}

void Dialog::focus_next()
{
  if (m_focused == m_controls.end())
    m_focused = m_controls.begin();

  if (m_focused != m_controls.end())
    ++m_focused;
}

void Dialog::update(unsigned ticks, InputHandler& input,
                    const Rect& active_region)
{
  if (m_need_reposition) {
    m_need_reposition = false;
    position_controls();
  }
  
  for (auto& c : m_controls)
    c->update(ticks, input, active_region);
}

void Dialog::draw(Renderer& renderer, const Rect& region) const
{
  for (auto& c : m_controls)
    c->draw(renderer, region);
}

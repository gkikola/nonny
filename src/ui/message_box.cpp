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

#include "ui/message_box.hpp"

#include <algorithm>
#include "color/color.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

const Color background_color(192, 192, 192);
constexpr int spacing = 8;

MessageBox::MessageBox(const Font& text_font,
                       const std::string& text,
                       int width)
  : m_text_font(&text_font),
    m_text(text)
{
  resize(width, spacing * 2);
}

void MessageBox::position_controls()
{
  m_need_reposition = false;

  //figure out how wide the controls are
  int control_wd = spacing;
  for (auto& c : m_controls)
    control_wd += c->boundary().width() + spacing;

  //center the controls at the bottom of the message box
  int x = m_boundary.x() + m_boundary.width() / 2 - control_wd / 2
    + spacing;
  int y = m_boundary.y() + m_boundary.height() - spacing;
  for (auto& c : m_controls) {
    c->move(x, y - c->boundary().height());
    x += c->boundary().width() + spacing;
  }
}

void MessageBox::recalculate_size()
{
  int width = m_boundary.width();
  int height = 2 * spacing;

  int control_wd = spacing;
  int control_ht = spacing;
  for (auto& c : m_controls) {
    control_wd += c->boundary().width() + spacing;
    control_ht = std::max(control_ht, c->boundary().height());
  }

  width = std::max(width, control_wd);
  height += control_ht + spacing;

  int text_wd, text_ht;
  m_text_font->text_size_wrapped(m_text, width - 2 * spacing,
                                 &text_wd, &text_ht);
  height += text_ht + spacing;

  resize(width, height);
}

void MessageBox::draw(Renderer& renderer, const Rect& region) const
{
  Rect rect = intersection(m_boundary, region);
  renderer.set_draw_color(background_color);
  renderer.fill_rect(rect);
  renderer.set_draw_color(default_colors::black);
  renderer.draw_rect(rect);

  Point text_pos(m_boundary.x() + spacing, m_boundary.y() + spacing);
  renderer.draw_text_wrapped(text_pos, *m_text_font, m_text,
                             m_boundary.width() - 2 * spacing, true);
  
  Dialog::draw(renderer, region);
}

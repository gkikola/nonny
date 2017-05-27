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

#include "ui/draw_tool_panel.hpp"

#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "video/rect.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"

constexpr unsigned spacing = 4;
constexpr unsigned num_tools = 5;
const Color background_color(225, 235, 238);

void DrawToolPanel::update(unsigned ticks, InputHandler& input,
                           const Rect& active_region)
{
  Rect region = intersection(m_boundary, active_region);
  Point cursor = input.mouse_position();
  if (input.was_mouse_button_pressed(Mouse::Button::left)
      && region.contains_point(cursor)) {
    unsigned tool_size = m_texture.height() / 2;
    unsigned selection = (cursor.x() - m_boundary.x())
      / (tool_size + spacing);
    m_cur_tool = static_cast<Tool>(selection);
  }
}

void DrawToolPanel::draw(Renderer& renderer, const Rect& region) const
{
  unsigned tool_size = m_texture.height() / 2;
  Rect src(0, 0, tool_size, tool_size);
  Rect dest(m_boundary.x(), m_boundary.y(), tool_size, tool_size);

  unsigned selection = static_cast<unsigned>(m_cur_tool);
  for (unsigned i = 0; i < num_tools; ++i) {
    if (i == selection)
      src.y() = 0;
    else
      src.y() = tool_size;

    renderer.set_draw_color(background_color);
    renderer.fill_rect(dest);
    renderer.copy_texture(m_texture, src, intersection(dest, region));
    if (i == selection) {
      renderer.set_draw_color(default_colors::black);
      renderer.draw_rect(dest);
    }
    src.x() += tool_size;
    dest.x() += tool_size + spacing;
  }
}

void DrawToolPanel::calc_size()
{
  unsigned tool_size = m_texture.height() / 2;
  resize(tool_size * num_tools + spacing * (num_tools - 1), tool_size);
}

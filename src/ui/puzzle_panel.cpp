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

#include "ui/puzzle_panel.hpp"

#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "utility/utility.hpp"
#include "video/renderer.hpp"

PuzzlePanel::PuzzlePanel(Font& rule_font, const Texture& cell_texture,
                         Puzzle& puzzle)
  : m_rule_font(rule_font), m_cell_texture(cell_texture), m_puzzle(&puzzle)
{
}

void PuzzlePanel::update(unsigned ticks, InputHandler& input,
                         const Rect& active_region)
{
}

void PuzzlePanel::draw(Renderer& renderer, const Rect& region) const
{
  if (m_puzzle) {
    renderer.set_viewport(region);
    renderer.set_draw_color(default_colors::black);
    draw_grid_lines(renderer);
  
    renderer.set_viewport();
  }
}

void PuzzlePanel::draw_grid_lines(Renderer& renderer) const
{
  unsigned width = m_puzzle->width();
  unsigned height = m_puzzle->height();
  for (unsigned x = 0; x <= width; ++x) {
    Point start(m_boundary.x() + x * (m_cell_size + 1), m_boundary.y());
    Point end(start.x(), m_boundary.y() + height * (m_cell_size + 1));
    renderer.draw_line(start, end);
  }
  for (unsigned y = 0; y <= height; ++y) {
    Point start(m_boundary.x(), m_boundary.y() + y * (m_cell_size + 1));
    Point end(m_boundary.x() + width * (m_cell_size + 1), start.y());
    renderer.draw_line(start, end);
  }
}

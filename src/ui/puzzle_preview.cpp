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

#include "ui/puzzle_preview.hpp"

#include "puzzle/puzzle.hpp"
#include "video/renderer.hpp"

void PuzzlePreview::update(unsigned ticks, InputHandler& input,
                           const Rect& active_region)
{
}

void PuzzlePreview::draw(Renderer& renderer, const Rect& region) const
{
  if (m_puzzle) {
    renderer.set_clip_rect(region);

    //draw background
    renderer.set_draw_color(default_colors::white);
    renderer.fill_rect(m_boundary);

    //calculate sizes and offsets
    unsigned p_width = m_puzzle->width();
    unsigned p_height = m_puzzle->height();
    unsigned horiz_pixel_size = 0, vert_pixel_size = 0;
    if (p_width)
      horiz_pixel_size = m_boundary.width() / m_puzzle->width();
    if (p_height)
      vert_pixel_size = m_boundary.height() / m_puzzle->height();
    unsigned offset_x = (m_boundary.width()
                         - horiz_pixel_size * m_puzzle->width()) / 2;
    unsigned offset_y = (m_boundary.height()
                         - vert_pixel_size * m_puzzle->height()) / 2;

    //draw the "pixels"
    for (unsigned y = 0; y < m_puzzle->height(); ++y) {
      for (unsigned x = 0; x < m_puzzle->width(); ++x) {
        auto cell = (*m_puzzle)[x][y];

        if (cell.state == PuzzleCell::State::filled) {
          renderer.set_draw_color(cell.color);

          Rect r(m_boundary.x() + offset_x + x * horiz_pixel_size,
                 m_boundary.y() + offset_y + y * vert_pixel_size,
                 horiz_pixel_size, vert_pixel_size);
          renderer.fill_rect(r);
        }
      }
    }
    
    renderer.set_clip_rect();
  }
}

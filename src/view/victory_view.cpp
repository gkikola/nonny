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

#include "view/victory_view.hpp"

#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "video/rect.hpp"
#include "video/renderer.hpp"
#include "view/view_manager.hpp"

void VictoryView::update(unsigned ticks, InputHandler& input)
{
}

void VictoryView::draw(Renderer& renderer)
{
  if (m_puzzle.width() > 0 && m_puzzle.height() > 0) {
    unsigned height = m_height;
    unsigned width = height * m_puzzle.width() / m_puzzle.height();
    if (width > m_width) {
      width = m_width;
      height = width * m_puzzle.height() / m_puzzle.width();
    }

    Rect region(m_width / 2 - width / 2, m_height / 2 - height / 2,
                width, height);
    renderer.set_draw_color(default_colors::white);
    renderer.fill_rect(region);

    for (unsigned j = 0; j != m_puzzle.height(); ++j) {
      for (unsigned i = 0; i != m_puzzle.width(); ++i) {
        Rect pixel(region.x() + region.width() / m_puzzle.width() * i,
                   region.y() + region.height() / m_puzzle.height() * j,
                   region.width() / m_puzzle.width(),
                   region.height() / m_puzzle.height());
        if (m_puzzle[i][j].state == PuzzleCell::State::filled) {
          renderer.set_draw_color(m_puzzle[i][j].color);
          renderer.fill_rect(pixel);
        }
      }
    }
  }
}

void VictoryView::resize(unsigned width, unsigned height)
{
  View::resize(width, height);
}

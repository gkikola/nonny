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

#include "ui/puzzle_info_panel.hpp"

#include "puzzle/puzzle.hpp"
#include "video/font.hpp"

PuzzleInfoPanel::PuzzleInfoPanel(Font& title_font, Font& info_font,
                                 Font& button_font)
  : m_title_font(title_font),
    m_info_font(info_font),
    m_button_font(button_font)
{
}

PuzzleInfoPanel::PuzzleInfoPanel(Font& title_font, Font& info_font,
                                 Font& button_font, Puzzle& puzzle)
  : m_title_font(title_font),
    m_info_font(info_font),
    m_button_font(button_font)
{
  attach_puzzle(puzzle);
}

void PuzzleInfoPanel::attach_puzzle(Puzzle& puzzle)
{
  m_puzzle = &puzzle;
}

void PuzzleInfoPanel::update(unsigned ticks, InputHandler& input,
                             const Rect& active_region)
{
}

void PuzzleInfoPanel::draw(Renderer& renderer, const Rect& region) const
{
}

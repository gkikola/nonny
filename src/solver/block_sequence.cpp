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

#include "solver/block_sequence.hpp"

BlockSequence::BlockSequence(unsigned line_size,
                             const std::vector<PuzzleClue>& line_clues,
                             Init init_type)
  : m_line_size(line_size)
{
  for (const auto& clue : line_clues) {
    Block b;
    b.length = clue.value;
    b.color = clue.color;
    m_blocks.push_back(b);
  }

  switch (init_type) {
  default:
  case Init::left:
    flush_left();
    break;
  case Init::right:
    flush_right();
    break;
  }
}

void BlockSequence::flush_left()
{
  unsigned pos = 0;
  Color prev_color;
  for (unsigned block = 0; block < size(); ++block) {
    //if previous block had same color, we need a gap
    if (block != 0 && m_blocks[block].color == prev_color)
      ++pos;

    m_blocks[block].pos = pos;
    prev_color = m_blocks[block].color;

    //next position
    pos += m_blocks[block].length;
  }
}

void BlockSequence::flush_right()
{
  unsigned pos = m_line_size;
  Color prev_color;
  for (unsigned index = size(); index > 0; --index) {
    unsigned block = index - 1;
    //need gap if previous color is the same
    if (block != size() - 1
        && m_blocks[block].color == prev_color)
      --pos;

    m_blocks[block].pos = pos - m_blocks[block].length;
    prev_color = m_blocks[block].color;

    pos -= m_blocks[block].length;
  }
}

bool BlockSequence::slide_right()
{
  if (empty())
    return false;

  unsigned pos = size() - 1;
  unsigned boundary = m_line_size;
  while (true) {
    if (m_blocks[pos].pos + m_blocks[pos].length < boundary) {
      ++m_blocks[pos].pos;
      for (unsigned i = pos + 1; i < size(); ++i) {
        m_blocks[i].pos = m_blocks[i - 1].pos + m_blocks[i - 1].length;
        if (m_blocks[i].color == m_blocks[i - 1].color)
          ++m_blocks[i].pos;
      }
      return true;
    } else {
      boundary -= m_blocks[pos].length;
      if (pos > 0 && m_blocks[pos - 1].color == m_blocks[pos].color)
        --boundary;
      if (pos == 0)
        break;
      --pos;
    }
  }
  return false;
}

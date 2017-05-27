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

BlockSequence::BlockSequence(int line_size,
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
  int pos = 0;
  Color prev_color;
  for (int block = 0; block < static_cast<int>(size()); ++block) {
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
  int pos = m_line_size;
  Color prev_color;
  for (int index = size(); index > 0; --index) {
    int block = index - 1;
    //need gap if previous color is the same
    if (block != static_cast<int>(size()) - 1
        && m_blocks[block].color == prev_color)
      --pos;

    m_blocks[block].pos = pos - m_blocks[block].length;
    prev_color = m_blocks[block].color;

    pos -= m_blocks[block].length;
  }
}

bool BlockSequence::slide_left()
{
  if (empty())
    return false;

  int pos = 0;
  int boundary = 0;
  while (pos < static_cast<int>(size())) {
    if (m_blocks[pos].pos > boundary) {
      --m_blocks[pos].pos;
      for (int index = pos; index > 0; --index) {
        int i = index - 1;
        m_blocks[i].pos = m_blocks[i + 1].pos - m_blocks[i].length;
        if (m_blocks[i].color == m_blocks[i + 1].color)
          --m_blocks[i].pos;
      }
      return true;
    } else {
      boundary += m_blocks[pos].length;
      if (pos + 1 < static_cast<int>(size())
          && m_blocks[pos + 1].color == m_blocks[pos].color)
        ++boundary;
      ++pos;
    }
  }

  return false;
}

bool BlockSequence::slide_right()
{
  if (empty())
    return false;

  int pos = size() - 1;
  int boundary = m_line_size;
  while (true) {
    if (m_blocks[pos].pos + m_blocks[pos].length < boundary) {
      ++m_blocks[pos].pos;
      for (int i = pos + 1; i < static_cast<int>(size()); ++i) {
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

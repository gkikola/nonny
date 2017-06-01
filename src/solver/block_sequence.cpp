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

#include <stdexcept>
#include "puzzle/puzzle_cell.hpp"
#include "puzzle/puzzle_line.hpp"

BlockSequence::BlockSequence(PuzzleLine& line)
  : m_line(line)
{
  if (m_line.clues().size() != 1 || m_line.clues()[0].value != 0) {
    for (int i = 0; i < static_cast<int>(m_line.clues().size()); ++i) {
      Block b;
      b.pos = 0;
      b.length = m_line.clues()[i].value;
      b.color = m_line.clues()[i].color;
      m_blocks.push_back(b);
    }
  }
}

bool BlockSequence::is_valid() const
{
  int num_blocks = size();
  int pos = 0;
  //for each block
  for (int block = 0; block < num_blocks; ++block) {
    //make sure all cells before the block are clear
    while (pos < m_blocks[block].pos) {
      if (m_line[pos].state == PuzzleCell::State::filled)
        return false;
      ++pos;
    }
    //make sure all cells within the block are filled with the correct color
    while (pos < m_blocks[block].pos + m_blocks[block].length) {
      if (m_line[pos].state == PuzzleCell::State::crossed_out)
        return false;
      else if (m_line[pos].state == PuzzleCell::State::filled
               && m_line[pos].color != m_blocks[block].color)
        return false;
      ++pos;
    }
  }
  //make sure all cells after the last block are clear
  while (pos < m_line.size()) {
    if (m_line[pos].state == PuzzleCell::State::filled)
      return false;
    ++pos;
  }
  return true;
}

bool BlockSequence::arrange_left()
{
  int pos = 0;
  int num_blocks = size();
  //for each block, left-to-right
  for (int i = 0; i < num_blocks; ++i) {
    //move block into current position
    m_blocks[i].pos = pos;
    //slide block to the right until it's in a valid position
    while (!is_block_valid(m_blocks[i])) {
      ++m_blocks[i].pos;
      //if we fall off the line, then it can't be solved
      if (m_blocks[i].pos + m_blocks[i].length > m_line.size())
        return false;
    }
    //move position to the right of current block
    pos = m_blocks[i].pos + m_blocks[i].length;
    //make sure blocks with the same color have a gap
    if (i + 1 < num_blocks
        && m_blocks[i].color == m_blocks[i + 1].color)
      ++pos;
  }

  /*
   * At this point the blocks are arranged in their leftmost positions
   * without covering any crossed out cells. However, their may be
   * filled cells to the right which aren't covered by these blocks,
   * so we need to check for that.
   */
  int block = num_blocks - 1;
  pos = m_line.size() - 1;
  while (pos >= 0 && block >= 0) {
    //if cell at pos is filled but uncovered, shift the blocks right
    //until it's covered
    while ((m_line[pos].state == PuzzleCell::State::filled
            && m_blocks[block].pos + m_blocks[block].length - 1 < pos)
           || !is_block_valid(m_blocks[block])) {
      if (!force_block_right(block)) {
        //can't go right any more, line isn't solvable
        return false;
      }

      //moving the block may have left another block in an invalid state
      //so go back to the end
      block = num_blocks - 1;
      pos = m_line.size() - 1;
    }

    //if pos is still uncovered, then it must not have been filled,
    //so move to the next cell to the left
    if (m_blocks[block].pos + m_blocks[block].length < pos) {
      --pos;
    } else {
      //pos is covered by the current block, so we can move on
      //move pos just before current block and switch to the next block over
      pos = m_blocks[block].pos - 1;
      --block;
    }
  }

  return true;
}

/*bool BlockSequence::arrange_left_absolute()
{
  int pos = 0;
  int num_blocks = size();
  //for each block, left-to-right
  for (int i = 0; i < num_blocks; ++i) {
    //move block into current position
    m_blocks[i].pos = pos;
    //slide block to the right until it's in a valid position
    while (!is_block_valid(m_blocks[i])) {
      ++m_blocks[i].pos;
      //if we fall off the line, then it can't be solved
      if (m_blocks[i].pos + m_blocks[i].length > m_line.size())
        return false;
    }
    //move position to the right of current block
    pos = m_blocks[i].pos + m_blocks[i].length;
    //make sure blocks with the same color have a gap
    if (i + 1 < num_blocks
        && m_blocks[i].color == m_blocks[i + 1].color)
      ++pos;
  }

  //slide the line right until it's in a valid state
  while (!is_valid()) {
    if (!slide_right())
      return false;
  }

  return true;
  }*/

bool BlockSequence::arrange_right()
{
  int pos = m_line.size();
  int num_blocks = size();
  //for each block, right-to-left
  for (int i = num_blocks - 1; i >= 0; --i) {
    //move block into current position
    m_blocks[i].pos = pos - m_blocks[i].length;
    //slide block to the left until it's in a valid position
    while (!is_block_valid(m_blocks[i])) {
      --m_blocks[i].pos;
      //if we fall off the line, then it can't be solved
      if (m_blocks[i].pos < 0)
        return false;
    }
    //move position to the left of current block
    pos -= m_blocks[i].length;
    //make sure blocks with the same color have a gap
    if (i - 1 >= 0
        && m_blocks[i].color == m_blocks[i - 1].color)
      --pos;
  }

  int block = 0;
  pos = 0;
  while (pos < m_line.size() && block < num_blocks) {
    //while pos is filled but uncovered, we need to shift blocks left
    //until it's covered
    while ((m_line[pos].state == PuzzleCell::State::filled
            && m_blocks[block].pos > pos)
           || !is_block_valid(m_blocks[block])) {
      if (!force_block_left(block)) {
        //can't go any further, line isn't solvable
        return false;
      }

      //moving the block may have left a previous block in an invalid state
      //so go back to the start
      block = 0;
      pos = 0;
    }
    if (m_blocks[block].pos > pos)
      ++pos;
    else {
      pos = m_blocks[block].pos + m_blocks[block].length;
      ++block;
    }
  }

  return true;
}

bool BlockSequence::force_block_left(int index)
{
  //if this is the first block, make sure it's within bounds
  if (index == 0) {
    if (m_blocks[index].pos > 0)
      --m_blocks[index].pos;
    else //we're stuck
      return false;
  } else { //if this is not the first block, make sure we aren't touching prev
    if ((m_blocks[index].color == m_blocks[index - 1].color
         && m_blocks[index].pos
         <= m_blocks[index - 1].pos + m_blocks[index - 1].length + 1)
        || m_blocks[index].pos
        <= m_blocks[index - 1].pos + m_blocks[index - 1].length) {
      //touching prev block, so need to push it left, recursively
      if (!force_block_left(index - 1))
        return false;
    }
    --m_blocks[index].pos;
  }

  return true;
}

bool BlockSequence::force_block_right(int index)
{
  int num_blocks = size();
  //if this is the last block, make sure it's within bounds
  if (index == num_blocks - 1) {
    if (m_blocks[index].pos + m_blocks[index].length < m_line.size())
      ++m_blocks[index].pos;
    else //we're stuck
      return false;
  } else { //if this is not the last block, make sure we aren't touching next
    if ((m_blocks[index].color == m_blocks[index + 1].color
         && m_blocks[index].pos + m_blocks[index].length + 1
         >= m_blocks[index + 1].pos)
        || m_blocks[index].pos + m_blocks[index].length
        >= m_blocks[index + 1].pos) {
      //touching next block, so need to move is right, recursively
      if (!force_block_right(index + 1))
        return false;
    }
    ++m_blocks[index].pos;
  }

  return true;
}

bool BlockSequence::slide_left()
{
  int num_blocks = size();
  int block = 0;
  while (block < num_blocks) {
    //see if there's room to move the current block to the left
    if (move_block_left(block)) {
      //move leftward blocks back to the right
      for (int i = block - 1; i >= 0; --i) {
        m_blocks[i].pos = m_blocks[i + 1].pos - m_blocks[i].length;
        if (m_blocks[i].color == m_blocks[i + 1].color)
          --m_blocks[i].pos;
      }
      if (is_valid())
        return true;
      else {
        block = 0;
      }
    } else {
      //can't go any further, move to the next block from the left
      ++block;
    }
  }
  return false;
}

bool BlockSequence::slide_right()
{
  int num_blocks = size();
  int block = num_blocks - 1;
  while (block >= 0) {
    //see if there's room to move the current block to the right
    if (move_block_right(block)) {
      //move rightward blocks back to the left
      for (int i = block + 1; i < num_blocks; ++i) {
        m_blocks[i].pos = m_blocks[i - 1].pos + m_blocks[i - 1].length;
        if (m_blocks[i].color == m_blocks[i - 1].color)
          ++m_blocks[i].pos;
      }

      if (is_valid())
        return true;
      else {
        block = num_blocks - 1;
      }
    } else {
      //can't go any further, move to the next block from the right
      --block;
    }
  }
  return false;
}

bool BlockSequence::move_block_left(int index)
{
  int original_pos = m_blocks[index].pos;
  do {
    --m_blocks[index].pos;
    if (index > 0) {
      if (m_blocks[index].color == m_blocks[index - 1].color
          && m_blocks[index].pos
          < m_blocks[index - 1].pos + m_blocks[index - 1].length + 1) {
        m_blocks[index].pos = original_pos;
        return false;
      } else if (m_blocks[index].pos
                 < m_blocks[index - 1].pos + m_blocks[index - 1].length) {
        m_blocks[index].pos = original_pos;
        return false;
      }
    } else {
      if (m_blocks[index].pos < 0) {
        m_blocks[index].pos = original_pos;
        return false;
      }
    }
  } while (!is_block_valid(m_blocks[index]));
  return true;
}

bool BlockSequence::move_block_right(int index)
{
  int num_blocks = m_blocks.size();
  int original_pos = m_blocks[index].pos;
  do {
    ++m_blocks[index].pos;
    if (index < num_blocks - 1) {
      if (m_blocks[index].color == m_blocks[index + 1].color
          && m_blocks[index].pos + m_blocks[index].length + 1
          > m_blocks[index + 1].pos) {
        m_blocks[index].pos = original_pos;
        return false;
      } else if (m_blocks[index].pos + m_blocks[index].length
                 > m_blocks[index + 1].pos) {
        m_blocks[index].pos = original_pos;
        return false;
      }
    } else {
      if (m_blocks[index].pos + m_blocks[index].length > m_line.size()) {
        m_blocks[index].pos = original_pos;
        return false;
      }
    }
  } while (!is_block_valid(m_blocks[index]));
  return true;
}

bool BlockSequence::is_block_valid(const Block& block) const
{
  for (int pos = block.pos; pos < block.pos + block.length; ++pos) {
    if (m_line[pos].state == PuzzleCell::State::crossed_out)
      return false;
    if (m_line[pos].state == PuzzleCell::State::filled
        && m_line[pos].color != block.color)
      return false;
  }
  return true;
}

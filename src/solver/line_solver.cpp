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

#include "solver/line_solver.hpp"

#include <algorithm>
#include "puzzle/puzzle.hpp"
#include "puzzle/puzzle_line.hpp"
#include "solver/block_sequence.hpp"

void LineSolver::operator()()
{
  std::vector<PuzzleCell> line;
  solve_complete(line);

  for (unsigned i = 0; i < line.size(); ++i) {
    if (line[i].state == PuzzleCell::State::filled)
      m_line.mark_cell(i, line[i].color);
    else if (line[i].state == PuzzleCell::State::crossed_out)
      m_line.cross_out_cell(i);
    else
      m_line.clear_cell(i);
  }
}

void LineSolver::solve_complete(std::vector<PuzzleCell>& result)
{
  BlockSequence blocks(m_line.size(), m_line.clues());

  std::vector<BlockSequence> valid;

  do {
    if (is_seq_valid(blocks))
      valid.push_back(blocks);
  } while (blocks.slide_right());

  intersect_blocks(result, valid);
}

bool LineSolver::is_seq_valid(const BlockSequence& blocks)
{
  unsigned pos = 0;
  for (unsigned block = 0; block < blocks.size(); ++block) {
    while (pos < blocks[block].pos) {
      if (m_line[pos].state == PuzzleCell::State::filled)
        return false;
      ++pos;
    }

    while (pos < blocks[block].pos + blocks[block].length) {
      if (m_line[pos].state == PuzzleCell::State::crossed_out)
        return false;
      ++pos;
    }
  }

  while (pos < m_line.size()) {
    if (m_line[pos].state == PuzzleCell::State::filled)
      return false;
    ++pos;
  }

  return true;
}

void LineSolver::intersect_blocks(std::vector<PuzzleCell>& result,
                                  std::vector<BlockSequence>& seqs)
{
  result.clear();
  if (seqs.empty())
    return;

  result = std::vector<PuzzleCell>(m_line.size(), PuzzleCell());
  
  unsigned size = m_line.clues().size();
  unsigned pos = 0;
  for (unsigned block = 0; block < size; ++block) {
    //take minimum starting point
    unsigned start = seqs[0][block].pos;
    for (unsigned seq = 0; seq < seqs.size(); ++seq)
      start = std::min(start, seqs[seq][block].pos);

    while (pos < start) {
      result[pos].state = PuzzleCell::State::crossed_out;
      ++pos;
    }

    //take maximum starting point
    for (unsigned seq = 0; seq < seqs.size(); ++seq)
      start = std::max(start, seqs[seq][block].pos);
    pos = start;

    //take minimum ending point
    unsigned end = seqs[0][block].pos + seqs[0][block].length;
    for (unsigned seq = 0; seq < seqs.size(); ++seq)
      end = std::min(end, seqs[seq][block].pos + seqs[seq][block].length);

    while (pos < end) {
      result[pos].state = PuzzleCell::State::filled;
      result[pos].color = seqs[0][block].color;
      ++pos;
    }

    //take maximum ending point
    for (unsigned seq = 0; seq < seqs.size(); ++seq)
      end = std::max(end, seqs[seq][block].pos + seqs[seq][block].length);
    pos = end;
  }

  while (pos < result.size()) {
    result[pos].state = PuzzleCell::State::crossed_out;
    ++pos;
  }
}

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

bool LineSolver::operator()()
{
  std::vector<PuzzleCell> line;
  if (!solve_complete(line))
    return false;

  for (int i = 0; i < static_cast<int>(line.size()); ++i) {
    if (line[i].state == PuzzleCell::State::filled)
      m_line.mark_cell(i, line[i].color);
    else if (line[i].state == PuzzleCell::State::crossed_out)
      m_line.cross_out_cell(i);
    else
      m_line.clear_cell(i);
  }
  return true;
}

bool LineSolver::solve_fast(std::vector<PuzzleCell>& result)
{  
  std::vector<BlockSequence> seqs;
  seqs.emplace_back(m_line);
  seqs.emplace_back(m_line);

  auto& lblocks = seqs[0];
  auto& rblocks = seqs[1];

  if (!lblocks.arrange_left())
    return false;

  if (!rblocks.arrange_right())
    return false;

  intersect_blocks(result, seqs);
  return true;
}

bool LineSolver::solve_complete(std::vector<PuzzleCell>& result)
{
  BlockSequence blocks(m_line);
  if (!blocks.arrange_left())
    return false;
  
  std::vector<BlockSequence> valid;

  do
    valid.push_back(blocks);
  while (blocks.slide_right());

  if (valid.empty())
    return false;

  intersect_blocks(result, valid);
  return true;
}

void LineSolver::intersect_blocks(std::vector<PuzzleCell>& result,
                                  std::vector<BlockSequence>& seqs)
{
  result.clear();
  if (seqs.empty()) {
    PuzzleCell cross;
    cross.state = PuzzleCell::State::crossed_out;
    result = std::vector<PuzzleCell>(m_line.size(), cross);
    return;
  }

  result = std::vector<PuzzleCell>(m_line.size(), PuzzleCell());
  
  int size = m_line.clues().size();
  if (size == 1 && m_line.clues()[0].value == 0)
    size = 0;
  int pos = 0;
  for (int block = 0; block < size; ++block) {
    //take minimum starting point
    int start = seqs[0][block].pos;
    for (int seq = 0; seq < static_cast<int>(seqs.size()); ++seq)
      start = std::min(start, seqs[seq][block].pos);

    while (pos < start) {
      result[pos].state = PuzzleCell::State::crossed_out;
      ++pos;
    }

    //take maximum starting point
    for (int seq = 0; seq < static_cast<int>(seqs.size()); ++seq)
      start = std::max(start, seqs[seq][block].pos);
    pos = start;

    //take minimum ending point
    int end = seqs[0][block].pos + seqs[0][block].length;
    for (int seq = 0; seq < static_cast<int>(seqs.size()); ++seq)
      end = std::min(end, seqs[seq][block].pos + seqs[seq][block].length);

    while (pos < end) {
      result[pos].state = PuzzleCell::State::filled;
      result[pos].color = seqs[0][block].color;
      ++pos;
    }

    //take maximum ending point
    for (int seq = 0; seq < static_cast<int>(seqs.size()); ++seq)
      end = std::max(end, seqs[seq][block].pos + seqs[seq][block].length);
    pos = end;
  }

  while (pos < static_cast<int>(result.size())) {
    result[pos].state = PuzzleCell::State::crossed_out;
    ++pos;
  }
}

bool LineSolver::update_clues(std::vector<PuzzleClue>& clues)
{
  //if 0 is the only clue,
  //line is finished only when fully crossed out
  if (clues.size() == 1 && clues[0].value == 0) {
    clues[0].state = PuzzleClue::State::finished;

    for (int pos = 0; pos < static_cast<int>(m_line.size()); ++pos) {
      if (m_line[pos].state != PuzzleCell::State::crossed_out) {
        clues[0].state = PuzzleClue::State::normal;
        if (m_line[pos].state == PuzzleCell::State::filled)
          return false;
      }
    }
    return true;
  }

  //see if line is already solved
  if (m_line.is_solved()) {
    for (auto& clue : clues)
      clue.state = PuzzleClue::State::finished;
    return true;
  }

  //find leftmost and rightmost solutions that work
  std::vector<BlockSequence> list;
  list.emplace_back(m_line);
  list.emplace_back(m_line);
  BlockSequence& left = list[0];
  BlockSequence& right = list[1];

  bool contradiction = false;
  if (!left.arrange_left() || !right.arrange_right())
    contradiction = true;

  if (contradiction) { //line contains a contradiction
    for (auto& clue : clues)
      clue.state = PuzzleClue::State::error;
  } else { //no contradictions
    //iterate over blocks
    for (int i = 0; i < static_cast<int>(left.size()); ++i) {
      clues[i].state = PuzzleClue::State::normal;

      //if left and right blocks cover the same area,
      //check to make sure that area is filled with the correct color
      if (left[i].pos == right[i].pos) {
        bool finished = true;

        //make sure left is crossed out (unless multicolor)
        if (left[i].pos > 0
            && m_line[left[i].pos - 1].state
            != PuzzleCell::State::crossed_out
            && (i == 0 || left[i - 1].color == left[i].color))
          finished = false;
        //make sure right is crossed out (unless multicolor)
        else if (left[i].pos + left[i].length < m_line.size()
                 && m_line[left[i].pos + left[i].length].state
                 != PuzzleCell::State::crossed_out
                 && (i == static_cast<int>(left.size()) - 1
                     || left[i + 1].color == left[i].color))
          finished = false;
        else { //make sure block is completely filled in
          for (int pos = left[i].pos;
               pos < left[i].pos + left[i].length;
               ++pos) {
            if (m_line[pos].state != PuzzleCell::State::filled
                || m_line[pos].color != clues[i].color) {
              finished = false;
              break;
            }
          }
        }

        if (finished)
          clues[i].state = PuzzleClue::State::finished;
      }
    }
  }
  return false;
}

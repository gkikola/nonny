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

#ifndef NONNY_LINE_SOLVER_HPP
#define NONNY_LINE_SOLVER_HPP

#include <vector>

class BlockSequence;
class PuzzleCell;
class PuzzleClue;
class PuzzleLine;

/*
 * Takes a line from a puzzle and solves just that line as much as
 * possible based on the line's state and clues. There are two line
 * solvers: the fast line solver is faster but doesn't always find all
 * the information that can be deduced from the line. The complete
 * solver is slow but does not miss anything.
 */
class LineSolver {
public:
  LineSolver(PuzzleLine& line) : m_line(line) { }

  // Solve the line and modify the line itself with the solution
  void operator()();

  /*
   * Solve the line and store the result in the given vector. Does not
   * modify the original line.
   */
  void solve_fast(std::vector<PuzzleCell>& result);
  void solve_complete(std::vector<PuzzleCell>& result);

  /*
   * Update clue states based on line progress. Returns true if line
   * is solved.
   */
  bool update_clues(std::vector<PuzzleClue>& clues);

private:
  // Is the block sequence consistent with the current line state?
  bool is_seq_valid(const BlockSequence& blocks);

  /*
   * Find the intersection of all the block sequences and store the
   * result in the given vector.
   */
  void intersect_blocks(std::vector<PuzzleCell>& result,
                        std::vector<BlockSequence>& seqs);
  
  PuzzleLine& m_line;
};

#endif

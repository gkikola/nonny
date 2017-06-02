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

#include "solver/solver.hpp"

#include <algorithm>
#include <stdexcept>
#include "solver/line_solver.hpp"

Solver::Solver(Puzzle& puzzle)
  : m_puzzle(puzzle),
    m_row_priority(puzzle.height(), 1),
    m_col_priority(puzzle.width(), 1)
{
}

bool Solver::step()
{
  if (is_finished())
    return true;
  if (m_inconsistent)
    return false;
  
  const int num_iters = 16;
  
  int count = 0;
  while (count < num_iters && is_line_available()) {
    LineType type;
    int index = -1;

    //alternate between rows and columns
    if (count % 2 == 0) {
      int row = select_row();
      if (row >= 0) {
        type = LineType::row;
        index = row;
      }
    } else {
      int col = select_col();
      if (col >= 0) {
        type = LineType::column;
        index = col;
      }
    }

    if (index >= 0) { //found a line, send it to line solver
      PuzzleLine line(m_puzzle, index, type);
      if (!solve_line(line, m_use_complete)) {
        //found a contradiction
        backtrack();
        return false;
      }
    }

    ++count;
  }

  int num_rows_solved = m_rows_solved.size();
  int num_cols_solved = m_cols_solved.size();
  if (num_rows_solved == m_puzzle.height()
      && num_cols_solved == m_puzzle.width()) {
    record_solution();
    
    if (m_alternatives.empty()) {
      //no other possibilities, so we're done
      m_finished = true;
    } else {
      //backtrack to find alternative solutions
      backtrack();
    }
  }

  if (!m_finished) {
    //if the complete solver found new information,
    //switch back to the fast solver
    bool line_available = is_line_available();
    if (m_use_complete && !line_available) {
      //complete solver found nothing, need to make a guess
      guess();
      m_use_complete = false;
    }

    //if complete solver found new information, switch back to fast
    if (m_new_info_found && m_use_complete) {
      m_new_info_found = false;
      m_use_complete = false;
    }

    //switch to complete line solver if the fast one didn't produce any info
    if (!m_use_complete && !line_available) {
      m_use_complete = true;
      for (int i = 0; i < m_puzzle.width(); ++i) {
        if (m_cols_solved.find(i) == m_cols_solved.end())
          m_col_priority[i] = 1;
        else
          m_col_priority[i] = 0;
      }
      for (int j = 0; j < m_puzzle.height(); ++j) {
        if (m_rows_solved.find(j) == m_rows_solved.end())
          m_row_priority[j] = 1;
        else
          m_row_priority[j] = 0;
      }
    }
  }
  
  return m_finished;
}

void Solver::operator()()
{
  while (!step()) { }
}

int Solver::select_row()
{
  int max_row = -1;
  int row_priority = 0;
  for (unsigned j = 0; j < m_row_priority.size(); ++j) {
    if (m_row_priority[j] > row_priority) {
      max_row = j;
      row_priority = m_row_priority[j];
    }
  }

  if (max_row < 0) {
    return -1;
  } else {
    m_last_row_selected = max_row;
    m_row_priority[max_row] = 0;
    return max_row;
  }
}

int Solver::select_col()
{
  int max_col = -1;
  int col_priority = 0;
  for (unsigned i = 0; i < m_col_priority.size(); ++i) {
    if (m_col_priority[i] > col_priority) {
      max_col = i;
      col_priority = m_col_priority[i];
    }
  }

  if (max_col < 0) {
    return -1;
  } else {
    m_last_col_selected = max_col;
    m_col_priority[max_col] = 0;
    return max_col;
  }
}

bool Solver::is_line_available()
{
  for (int i : m_col_priority) {
    if (i > 0)
      return true;
  }
  for (int j : m_row_priority) {
    if (j > 0)
      return true;
  }
  return false;
}

void Solver::check_for_solved_lines()
{
  m_rows_solved.clear();
  m_cols_solved.clear();
  for (int i = 0; i < m_puzzle.width(); ++i) {
    PuzzleLine line = m_puzzle.get_col(i);
    if (line.is_solved())
      m_cols_solved.insert(i);
  }
  for (int j = 0; j < m_puzzle.height(); ++j) {
    PuzzleLine line = m_puzzle.get_row(j);
    if (line.is_solved())
      m_rows_solved.insert(j);
  }
}

void Solver::backtrack()
{
  if (m_alternatives.empty()) {
    //no alternatives
    //if no solutions were found yet, then the puzzle is inconsistent
    if (num_solutions() == 0)
      m_inconsistent = true;

    m_finished = true;
  } else {
    //backtrack to last alternative
    m_puzzle.load_state(m_alternatives.top().puzzle_state);

    //reset line priorities
    for (int& i : m_col_priority)
      i = 0;
    for (int& j : m_row_priority)
      j = 0;
    m_col_priority[m_alternatives.top().col] = 1;
    m_row_priority[m_alternatives.top().row] = 1;

    //regenerate solved list
    check_for_solved_lines();

    m_alternatives.pop();
  }
}

void Solver::guess()
{
  const int max_search_dist = 4;

  //increment guess counter
  ++m_num_guesses;

  //try to locate a good cell for a guess
  int best_x = -1, best_y = -1;
  int max_pot = 0;

  //calculate potential of nearby cells
  for (int y = m_last_row_selected - max_search_dist;
       y < m_last_row_selected + max_search_dist;
       ++y) {
    for (int x = m_last_col_selected - max_search_dist;
         x < m_last_col_selected + max_search_dist;
         ++x) {
      int potential = cell_potential(x, y);
      if (potential > max_pot) {
        best_x = x;
        best_y = y;
        max_pot = potential;
      }
    }
  }

  //if we couldn't find any candidates nearby, just search
  //the whole grid and pick the first undetermined cell
  if (max_pot == 0) {
    for (int y = 0; y < m_puzzle.height(); ++y) {
      for (int x = 0; x < m_puzzle.width(); ++x) {
        if (m_puzzle[x][y].state == PuzzleCell::State::blank) {
          best_x = x;
          best_y = y;
          y = m_puzzle.height(); //break outer loop
          break;
        }
      }
    }
  }

  //if there are no blank cells, why are we here?
  if (best_x < 0 || best_y < 0)
    throw std::runtime_error("Solver::guess: "
                             "guess called on finished puzzle");
  
  //we have our guess, now make the branches
  SolverState state;
  state.row = best_y;
  state.col = best_x;
  CompressedState grid_state;

  //cross out cell and push state onto the stack
  m_puzzle.cross_out_cell(best_x, best_y);
  m_puzzle.copy_state(grid_state);
  state.puzzle_state = std::move(grid_state);
  m_alternatives.push(state);

  //try each different color and push states onto the stack
  auto first = m_puzzle.palette().begin();
  if (first->name == "background") ++first;
  auto it = first;
  ++it;
  while (it != m_puzzle.palette().end()
         && it->name != "background") {
    grid_state = CompressedState();
    m_puzzle.mark_cell(best_x, best_y, it->color);
    m_puzzle.copy_state(grid_state);
    state.puzzle_state = std::move(grid_state);
    m_alternatives.push(state);
  }

  //try first color and continue on from there
  m_puzzle.mark_cell(best_x, best_y, first->color);
  m_col_priority[best_x] = 2;
  m_row_priority[best_y] = 2;
}

int Solver::cell_potential(int x, int y)
{
  if (x < 0 || x >= m_puzzle.width()
      || y < 0 || y >= m_puzzle.height())
    return 0;

  if (m_puzzle[x][y].state != PuzzleCell::State::blank)
    return 0;
  
  return num_filled_neighbors(x, y);
}

int Solver::num_filled_neighbors(int x, int y)
{
  int count = 0;
  if (x > 0 && m_puzzle[x - 1][y].state != PuzzleCell::State::blank)
    ++count;
  else if (x == 0) //edge cells are useful so count them too
    ++count;

  if (y > 0 && m_puzzle[x][y - 1].state != PuzzleCell::State::blank)
    ++count;
  else if (y == 0)
    ++count;

  if (x + 1 < m_puzzle.width()
      && m_puzzle[x + 1][y].state != PuzzleCell::State::blank)
    ++count;
  else if (x + 1 == m_puzzle.width())
    ++count;
  
  if (y + 1 < m_puzzle.height()
      && m_puzzle[x][y + 1].state != PuzzleCell::State::blank)
    ++count;
  else if (y + 1 == m_puzzle.height())
    ++count;

  return count;
}

bool Solver::solve_line(PuzzleLine& line, bool complete)
{
  LineSolver solver(line);
  m_solved_line.clear();
  if (complete) {
    if (!solver.solve_complete(m_solved_line))
      return false;
  } else {
    if (!solver.solve_fast(m_solved_line))
      return false;
  }

  bool is_solved = true;
  for (int i = 0; i < line.size(); ++i) {
    //if line is completely filled in, we're done with this line
    if (m_solved_line[i].state == PuzzleCell::State::blank)
      is_solved = false;
    
    //if line solver produced new information, update puzzle line
    //and boost priority of changed perpendicular lines
    if (line[i] != m_solved_line[i]
        && m_solved_line[i].state != PuzzleCell::State::blank) {
      m_new_info_found = true;

      if (line.type() == LineType::row)
        ++m_col_priority[i];
      else
        ++m_row_priority[i];

      switch (m_solved_line[i].state) {
      case PuzzleCell::State::filled:
        line.mark_cell(i, m_solved_line[i].color);
        break;
      case PuzzleCell::State::crossed_out:
        line.cross_out_cell(i);
        break;
      default:
        break;
      }
    }
  }

  if (is_solved) {
    if (line.type() == LineType::row)
      m_rows_solved.insert(line.index());
    else
      m_cols_solved.insert(line.index());
  }
  
  return true;
}

void Solver::record_solution()
{
  CompressedState sol;
  m_puzzle.copy_state(sol);

  //check to see if this solution was already found
  for (auto& e : m_solutions) {
    if (e == sol)
      return;
  }
  
  //otherwise, record the solution
  m_solutions.push_back(std::move(sol));
}

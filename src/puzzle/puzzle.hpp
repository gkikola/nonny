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

#ifndef NONNY_PUZZLE_HPP
#define NONNY_PUZZLE_HPP

#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "color/color.hpp"
#include "color/color_palette.hpp"
#include "puzzle/compressed_state.hpp"
#include "puzzle/puzzle_clue.hpp"
#include "puzzle/puzzle_grid.hpp"
#include "puzzle/puzzle_io.hpp"
#include "puzzle/puzzle_line.hpp"

/*
 * Class that represents a nonogram puzzle.
 */
class Puzzle {
  friend std::ostream& write_puzzle(std::ostream&, Puzzle,
                                    PuzzleFormat fmt);
  friend std::istream& read_puzzle(std::istream&, Puzzle&,
                                   PuzzleFormat fmt);

public:
  typedef std::map<std::string, std::string> Properties;
  typedef std::vector<std::vector<PuzzleClue>> ClueContainer;
  typedef ClueContainer::value_type ClueSequence;
  
  Puzzle() = default; // blank 0x0 puzzle
  Puzzle(const Puzzle&) = default;
  Puzzle(Puzzle&&) = default;

  // Creates a blank puzzle of the given size
  Puzzle(int width, int height);
  Puzzle(int width, int height, ColorPalette palette);

  int width() const { return m_grid.width(); }
  int height() const { return m_grid.height(); }

  PuzzleLine get_row(int index);
  ConstPuzzleLine get_row(int index) const;
  PuzzleLine get_col(int index);
  ConstPuzzleLine get_col(int index) const;

  ConstPuzzleLine operator[](int col) const;
  inline const PuzzleCell& at(int col, int row) const;
  
  void mark_cell(int col, int row, const Color& color = Color());
  void clear_cell(int col, int row);
  void cross_out_cell(int col, int row);

  void clear_all_cells();

  void shift_cells(int x, int y);
  
  void copy_state(CompressedState& state) const;
  void load_state(const CompressedState& state);

  bool is_solved() const;
  bool is_row_solved(int row) const;
  bool is_col_solved(int col) const;

  // Determines whether all of the cells are blank
  bool is_clear() const;

  // Determines whether the puzzle has multiple foreground colors
  bool is_multicolor() const { return m_palette.size() > 2; }

  /*
   * Update clue numbers based on changes made to puzzle grid and
   * update line solve status
   */
  void update(bool edit_mode = false);
  
  // Get color palette associated with this puzzle
  const ColorPalette& palette() const { return m_palette; }

  // Get rid of palette colors that aren't being used
  void purge_unused_colors();

  // Return pointer to p's property value or nullptr if p is not found
  inline const std::string* find_property(const std::string& p) const;

  void set_property(const std::string& property, const std::string& value);
  void resize(int width, int height);
  
  // Throw exception if p is not found in property map
  const std::string&
  property(const std::string& p) const { return m_properties.at(p); }

  const ClueContainer& row_clues() const { return m_row_clues; }
  const ClueContainer& col_clues() const { return m_col_clues; }
  inline const ClueSequence& row_clues(int row) const;
  inline const ClueSequence& col_clues(int col) const;
  
  Puzzle& operator=(const Puzzle&) & = default;
  Puzzle& operator=(Puzzle&&) & = default;

private:
  void refresh_all_cells();
  void handle_size_change();
  ClueSequence& line_clues(int index, LineType type);
  void update_line(int index, LineType type, bool edit_mode);

  PuzzleGrid m_grid;
  ClueContainer m_row_clues;
  ClueContainer m_col_clues;
  ColorPalette m_palette;
  Properties m_properties;
  std::set<int> m_rows_changed;
  std::set<int> m_cols_changed;
  std::set<int> m_rows_solved;
  std::set<int> m_cols_solved;
};

// Reads and writes puzzles in the .non format
inline std::ostream& operator<<(std::ostream& os, const Puzzle& puzzle);
inline std::istream& operator>>(std::istream& is, Puzzle& puzzle);


/* implementation */

inline const PuzzleCell& Puzzle::at(int col, int row) const
{
  return m_grid.at(col, row);
}

inline const std::string* Puzzle::find_property(const std::string& p) const
{
  auto it = m_properties.find(p);
  return (it == m_properties.end()) ? nullptr : &it->second;
}

inline const Puzzle::ClueSequence& Puzzle::row_clues(int row) const
{
  return m_row_clues.at(row);
}

inline const Puzzle::ClueSequence& Puzzle::col_clues(int col) const
{
  return m_col_clues.at(col);
}

inline std::ostream& operator<<(std::ostream& os, const Puzzle& puzzle)
{
  return write_puzzle(os, puzzle);
}

inline std::istream& operator>>(std::istream& is, Puzzle& puzzle)
{
  return read_puzzle(is, puzzle);
}

#endif

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

#include "puzzle/puzzle_grid.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include "color/color_palette.hpp"
#include "puzzle/puzzle_io.hpp"
#include "utility/utility.hpp"

PuzzleCell& PuzzleGrid::at(unsigned x, unsigned y)
{
  //call const version and convert back
  return const_cast<PuzzleCell&>
    (const_cast<const PuzzleGrid*>(this)->at(x, y));
}

const PuzzleCell& PuzzleGrid::at(unsigned x, unsigned y) const
{
  decltype(m_grid.size()) pos = y * m_width + x;
  
  if (pos > m_grid.size())
    throw std::out_of_range("PuzzleGrid::at: attempted to access "
                            "invalid puzzle cell");

  return m_grid[pos];
}

std::ostream& operator<<(std::ostream& os, const PuzzleGrid& grid)
{
  ColorPalette palette;
  char symbol = '0';
  for (unsigned y = 0; y != grid.height(); ++y) {
    for (unsigned x = 0; x != grid.width(); ++x) {
      Color color = grid.at(x, y).color;
      if (palette.find(color) == palette.end()) {
        palette.add(color, std::string(1, symbol), symbol);
        os << symbol << "=" << color << "\n";
        if (symbol == '9')
          symbol = 'a';
        else
          ++symbol;
      }
    }
  }
  os << "---\n";
  return write_grid(os, grid, palette);
}

std::istream& operator>>(std::istream& is, PuzzleGrid& grid)
{
  ColorPalette palette;
  std::string line;
  while (std::getline(is, line)) {
    if (!line.empty() && line[0] == '-')
      break;

    std::istringstream ss(line);
    char symbol;
    ss >> symbol;
    ss.get();
    if (ss.peek() == '#')
      ss.get();
    Color color;
    ss >> color;
    palette.add(color, std::string(1, symbol), symbol);
  }

  return read_grid(is, grid, palette);
}

std::ostream& write_grid(std::ostream& os, const PuzzleGrid& grid,
                         const ColorPalette& palette)
{
  char bkgd;
  bkgd = palette.symbol("background");
  
  for (unsigned y = 0; y != grid.height(); ++y) {
    for (unsigned x = 0; x != grid.width(); ++x) {
      const PuzzleCell& cell = grid.at(x, y);
      if (cell.state == PuzzleCell::State::filled) {
        auto it = palette.find(cell.color);
        if (it == palette.end())
          throw std::logic_error("::write_grid: color not found in palette");
        os << it->symbol;
      } else if (cell.state == PuzzleCell::State::crossed_out)
        os << ' ';
      else
        os << bkgd;
    }
    os << "\n";
  }
  return os;
}

std::istream& read_grid(std::istream& is, PuzzleGrid& grid,
                        const ColorPalette& palette)
{
  char bkgd = 0, black = 0;
  try {
    bkgd = palette.symbol("background");
    black = palette.find(Color())->symbol;
  } catch (std::out_of_range) { }
  grid.m_grid.clear();

  std::string line;
  while (std::getline(is, line)) {
    unsigned counter = 0;
    for (auto c : line) {
      try {
        PuzzleCell cell;
        cell.state = PuzzleCell::State::blank;
        Color color;
        if (c == ' ')
          cell.state = PuzzleCell::State::crossed_out;
        else if (c != black && c != bkgd)
          color = palette[c];
        if (c != ' ' && c != bkgd) {
          cell.state = PuzzleCell::State::filled;
          cell.color = color;
        }
        grid.m_grid.push_back(cell);
        ++counter;
      } catch (std::out_of_range) { }
    }
    if (counter > grid.m_width)
      grid.m_width = counter;
  }

  if (grid.m_width > 0 && grid.m_grid.size() % grid.m_width != 0)
    throw InvalidPuzzleFile("::read_grid: invalid puzzle state");

  return is;
}

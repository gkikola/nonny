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

#ifndef NONNY_COLOR_PALETTE_HPP
#define NONNY_COLOR_PALETTE_HPP

#include <map>
#include <string>
#include "color/color.hpp"

/*
 * Holds a collection of colors, indexed by name and by character symbol.
 * The symbol is used in puzzle files to represent the corresponding color.
 */
class ColorPalette {
public:
  ColorPalette();
  ColorPalette(const ColorPalette&) = default;
  ColorPalette(ColorPalette&&) = default;

  // Registers a new color
  void add(Color color, const std::string& name, char symbol = 0);

  ColorPalette& operator=(const ColorPalette&) & = default;
  ColorPalette& operator=(ColorPalette&&) & = default;

  // Looks up a color, throws std::out_of_range if not found
  const Color&
  operator[](const std::string& name) const { return m_colors.at(name); }

  const Color&
  operator[](char symbol) const { return m_colors_by_sym.at(symbol); }
private:
  std::map<std::string, Color> m_colors;
  std::map<char, Color> m_colors_by_sym;
};

#endif

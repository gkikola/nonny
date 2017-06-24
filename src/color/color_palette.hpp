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

#include <string>
#include <vector>
#include "color/color.hpp"

/*
 * Holds a collection of colors, indexed by name and by character symbol.
 * The symbol is used in puzzle files to represent the corresponding color.
 */
class ColorPalette {
  friend bool operator==(const ColorPalette& l, const ColorPalette& r);
public:
  struct Entry;
  typedef std::vector<Entry>::iterator iterator;
  typedef std::vector<Entry>::const_iterator const_iterator;

  ColorPalette();
  ColorPalette(const ColorPalette&) = default;
  ColorPalette(ColorPalette&&) = default;

  // Registers a new color
  void add(const Color& color, const std::string& name, char symbol = 0);

  // Remove an existing color, throws out_of_range if not found
  void remove(const std::string& name);

  // Lookup a symbol character from a color name
  inline char symbol(const std::string& name) const;

  ColorPalette& operator=(const ColorPalette&) & = default;
  ColorPalette& operator=(ColorPalette&&) & = default;

  // Looks up a color, throws std::out_of_range if not found
  const Color& operator[](const std::string& name) const;
  const Color& operator[](char symbol) const;

  static ColorPalette default_palette();

  const_iterator begin() const { return m_colors.begin(); }
  const_iterator end() const { return m_colors.end(); }

  const_iterator find(const Color& color) const;

  std::vector<Entry>::size_type
  size() const { return m_colors.size(); }
private:
  iterator at(const std::string& name);
  const_iterator at(const std::string& name) const;

  std::vector<Entry> m_colors;
};

struct ColorPalette::Entry {
  Color color;
  std::string name;
  char symbol = 0;
};

inline bool operator==(const ColorPalette& l, const ColorPalette& r);
inline bool operator!=(const ColorPalette& l, const ColorPalette& r);

inline bool
operator==(const ColorPalette::Entry& l, const ColorPalette::Entry& r);
inline bool
operator!=(const ColorPalette::Entry& l, const ColorPalette::Entry& r);
bool operator<(const ColorPalette::Entry& l, const ColorPalette::Entry& r);


/* implementation */

inline char ColorPalette::symbol(const std::string& name) const
{
  return at(name)->symbol;
}

inline bool operator==(const ColorPalette& l, const ColorPalette& r)
{
  return l.m_colors == r.m_colors;
}

inline bool operator!=(const ColorPalette& l, const ColorPalette& r)
{
  return !(l == r);
}

inline bool
operator==(const ColorPalette::Entry& l, const ColorPalette::Entry& r)
{
  return l.color == r.color && l.name == r.name && l.symbol == r.symbol;
}

inline bool
operator!=(const ColorPalette::Entry& l, const ColorPalette::Entry& r)
{
  return !(l == r);
}

#endif

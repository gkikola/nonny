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

#include "color/color_palette.hpp"

#include <algorithm>
#include <sstream>

ColorPalette::ColorPalette()
  : m_colors({{default_colors::black, "black", 'X'},
        {default_colors::white, "background", '.'}})
{
}

void ColorPalette::add(const Color& color, const std::string& name,
                       char symbol)
{
  auto it = std::find_if(m_colors.begin(), m_colors.end(),
                         [&color, &name](const Entry& e)
                         { return e.color == color || e.name == name; });
  if (it == m_colors.end()) {
    m_colors.push_back({color, name, symbol});
    std::inplace_merge(m_colors.begin(), m_colors.end() - 1, m_colors.end());
  } else {
    it->color = color;
    it->name = name;
    it->symbol = symbol;
    std::sort(m_colors.begin(), m_colors.end());
  }
}

void ColorPalette::remove(const std::string& name)
{
  m_colors.erase(at(name));
}

const Color& ColorPalette::operator[](const std::string& name) const
{
  return at(name)->color;
}

const Color& ColorPalette::operator[](char symbol) const
{
  auto it = std::find_if(m_colors.begin(), m_colors.end(),
                         [=](const Entry& e) { return e.symbol == symbol; });
  if (it == m_colors.end())
    throw std::out_of_range("ColorPalette::operator[]: color symbol "
                            + std::string(1, symbol) + " is not defined");
  return it->color;
}

ColorPalette ColorPalette::default_palette()
{
  ColorPalette palette;
  palette.add(default_colors::gray, "gray", '/');
  palette.add(default_colors::silver, "silver", '|');
  palette.add(default_colors::maroon, "maroon", '&');
  palette.add(default_colors::red, "red", '@');
  palette.add(default_colors::olive, "olive", '(');
  palette.add(default_colors::yellow, "yellow", '^');
  palette.add(default_colors::green, "green", '*');
  palette.add(default_colors::lime, "lime", '#');
  palette.add(default_colors::teal, "teal", ')');
  palette.add(default_colors::aqua, "aqua", '+');
  palette.add(default_colors::navy, "navy", '-');
  palette.add(default_colors::blue, "blue", '$');
  palette.add(default_colors::purple, "purple", '!');
  palette.add(default_colors::fuchsia, "fuchsia", '=');
  palette.add(default_colors::orange, "orange", '%');
  return palette;
}

ColorPalette::const_iterator ColorPalette::find(const Color& color) const
{
  return std::find_if(m_colors.begin(), m_colors.end(),
                      [&](const Entry& e) { return e.color == color; });
}

ColorPalette::iterator ColorPalette::at(const std::string& name)
{
  auto it = std::find_if(m_colors.begin(), m_colors.end(),
                         [&](const Entry& e) { return e.name == name; });
  if (it == m_colors.end())
    throw std::out_of_range("ColorPalette::at: " + name + " is not defined");

  return it;
}

ColorPalette::const_iterator ColorPalette::at(const std::string& name) const
{
  auto it = std::find_if(m_colors.cbegin(), m_colors.cend(),
                         [&](const Entry& e) { return e.name == name; });
  if (it == m_colors.cend())
    throw std::out_of_range("ColorPalette::at: " + name + " is not defined");

  return it;
}

bool operator<(const ColorPalette::Entry& l, const ColorPalette::Entry& r)
{
  if (l.color == r.color) {
    if (l.name == r.name)
      return l.symbol < r.symbol;
    else
      return l.name < r.name;
  } else
    return l.color < r.color;
}

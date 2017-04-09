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

#include "color/color.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include "main/utility.hpp"

namespace default_colors {
  extern const Color black(0, 0, 0);
  extern const Color white(255, 255, 255);
  extern const Color red(255, 0, 0);
  extern const Color green(0, 255, 0);
  extern const Color blue(0, 0, 255);
}

std::ostream& operator<<(std::ostream& os, const Color& color)
{
  os << std::hex << std::uppercase
     << std::setfill('0')
     << std::setw(2) << color.m_r
     << std::setw(2) << color.m_g
     << std::setw(2) << color.m_b;
  return os << std::dec << std::nouppercase << std::setfill(' ');
}

std::istream& read_component(std::istream& is, unsigned& val)
{
  //ignore whitespace
  while (is_space(is.peek())) is.get();

  //read exactly two characters
  char comp[3];
  comp[0] = is.get();
  comp[1] = is.get();
  comp[2] = '\0';

  //interpret
  std::istringstream ss(comp);
  ss >> std::hex >> val;

  return is;
}

std::istream& operator>>(std::istream& is, Color& color)
{
  read_component(is, color.m_r);
  read_component(is, color.m_g);
  read_component(is, color.m_b);
  return is;
}

bool operator<(const Color& l, const Color& r)
{
  auto lum1 = l.luminance();
  auto lum2 = r.luminance();

  if (lum1 == lum2) {
    if (l.red() == r.red()) {
      if (l.green() == r.green())
        return l.blue() < r.blue();
      else
        return l.green() < r.green();
    } else {
      return l.red() < r.red();
    }
  } else {
    return lum1 < lum2;
  }
}

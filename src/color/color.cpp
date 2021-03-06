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
#include "utility/utility.hpp"

namespace default_colors {
  extern const Color black(0, 0, 0);
  extern const Color gray(128, 128, 128);
  extern const Color silver(192, 192, 192);
  extern const Color white(255, 255, 255);
  extern const Color maroon(128, 0, 0);
  extern const Color red(255, 0, 0);
  extern const Color olive(128, 128, 0);
  extern const Color yellow(255, 255, 0);
  extern const Color green(0, 128, 0);
  extern const Color lime(0, 255, 0);
  extern const Color teal(0, 128, 128);
  extern const Color aqua(0, 255, 255);
  extern const Color navy(0, 0, 128);
  extern const Color blue(0, 0, 255);
  extern const Color purple(128, 0, 128);
  extern const Color fuchsia(255, 0, 255);
  extern const Color orange(255, 128, 0);
}

Color Color::fade(double opacity) const
{
  const int max = 256;
  int red = static_cast<int>((1 - opacity) * max + opacity * m_r);
  int green = static_cast<int>((1 - opacity) * max + opacity * m_g);
  int blue = static_cast<int>((1 - opacity) * max + opacity * m_b);

  if (red > max)
    red = max;
  if (green > max)
    green = max;
  if (blue > max)
    blue = max;

  return Color(red, green, blue);
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

std::istream& read_component(std::istream& is, int& val)
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

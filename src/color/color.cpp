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

#include <ios>
#include <iostream>

namespace default_colors {
  extern const Color black(0, 0, 0);
  extern const Color white(255, 255, 255);
  extern const Color red(255, 0, 0);
  extern const Color green(0, 255, 0);
  extern const Color blue(0, 0, 255);
}

std::ostream& operator<<(std::ostream& os, const Color& color)
{
  auto old_flags = os.setf(std::ios_base::hex | std::ios_base::uppercase);
  os << color.m_r << color.m_g << color.m_b;
  os.setf(old_flags);
  return os;
}

std::istream& operator>>(std::istream& is, Color& color)
{
  return is;
}

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

#include "video/rect.hpp"

#include <algorithm>

Rect intersection(const Rect& r1, const Rect& r2)
{
  int x = std::max(r1.x(), r2.x());
  int y = std::max(r1.y(), r2.y());
  int width = std::min(r1.x() + r1.width(),
                       r2.x() + r2.width()) - x;
  int height = std::min(r1.y() + r1.height(),
                        r2.y() + r2.height()) - y;

  if (width < 0 || height < 0)
    width = height = 0;

  return Rect(x, y, width, height);
}

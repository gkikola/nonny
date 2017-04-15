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

#ifndef NONNY_RECT_HPP
#define NONNY_RECT_HPP

#include "video/point.hpp"

/*
 * Represents an axis-aligned rectangle within the application window
 */
class Rect {
public:
  Rect() = default;
  Rect(int x, int y, unsigned width, unsigned height)
    : m_x(x), m_y(y), m_width(width), m_height(height) { }
  
  Rect(const Rect&) = default;
  Rect(Rect&&) = default;
  Rect& operator=(const Rect&) & = default;
  Rect& operator=(Rect&&) & = default;

  explicit operator bool() const { return m_width != 0 && m_height != 0; }
  
  int x() const { return m_x; }
  int y() const { return m_y; }
  unsigned width() const { return m_width; }
  unsigned height() const { return m_height; }

  void move(int x, int y) { m_x = x; m_y = y; }
  inline void resize(unsigned width, unsigned height);

  inline bool contains_point(int x, int y) const;
  inline bool contains_point(const Point& p) const;
  
private:
  int m_x = 0;
  int m_y = 0;
  unsigned m_width = 0;
  unsigned m_height = 0;
};

/*
 * Determines the intersection of two rectangles
 */
Rect intersection(const Rect& r1, const Rect& r2);


/* implementation */

inline void Rect::resize(unsigned width, unsigned height)
{
  m_width = width; m_height = height;
}

inline bool Rect::contains_point(int x, int y) const
{
  return x >= m_x && x <= m_x + static_cast<int>(m_width)
    && y >= m_y && y <= m_y + static_cast<int>(m_height);
}

inline bool Rect::contains_point(const Point& p) const
{
  return contains_point(p.x(), p.y());
}

#endif

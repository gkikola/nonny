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

#ifndef NONNY_POINT_HPP
#define NONNY_POINT_HPP

// Represents a point in the application window
class Point {
public:
  Point() = default;
  Point(int x, int y) : m_x(x), m_y(y) { }

  Point(const Point&) = default;
  Point(Point&&) = default;
  Point& operator=(const Point&) & = default;
  Point& operator=(Point&&) & = default;

  int& x() { return m_x; }
  int x() const { return m_x; }
  int& y() { return m_y; }
  int y() const { return m_y; }
private:
  int m_x = 0;
  int m_y = 0;
};

// Returns the distance in pixels between two points
double distance(const Point& p1, const Point& p2);

#endif

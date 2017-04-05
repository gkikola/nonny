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

#ifndef NONNY_COLOR_HPP
#define NONNY_COLOR_HPP

#include <iosfwd>
#include <stdexcept>

/*
 * Class representing a color. Colors are stored as red/green/blue
 * components each in the range 0-255.
 */
class Color {
  friend std::ostream& operator<<(std::ostream&, const Color&);
  friend std::istream& operator>>(std::istream&, Color&);
public:
  Color() : m_r(0), m_g(0), m_b(0) { }
  Color(const Color&) = default;
  Color(unsigned red, unsigned green, unsigned blue)
    : m_r(red), m_g(green), m_b(blue) { validate(); }

  // Gets color components
  unsigned red_component() const { return m_r; }
  unsigned green_component() const { return m_g; }
  unsigned blue_component() const { return m_b; }

  Color& operator=(const Color&) & = default;
private:
  inline void validate() const;
  
  unsigned m_r, m_g, m_b;
  static constexpr unsigned s_max = 255;   
};

/*
 * Exception class used to indicate that a color value is invalid.
 */
class BadColor : public std::logic_error {
public:
  BadColor() : logic_error("invalid color value") { }
};

// Read/write colors from/to a stream
std::ostream& operator<<(std::ostream& os, const Color& color);
std::istream& operator>>(std::istream& is, Color& color);

// Predefined default colors
namespace default_colors {
  extern const Color black;
  extern const Color white;
  extern const Color red;
  extern const Color green;
  extern const Color blue;
}


/* implementation */

/*
 * Ensures that a valid color is being stored and throws a BadColor
 * exception if not.
 */
inline void Color::validate() const
{
  if (m_r > s_max || m_g > s_max || m_b > s_max)
    throw BadColor();
}

#endif

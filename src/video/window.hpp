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

#ifndef NONNY_WINDOW_HPP
#define NONNY_WINDOW_HPP

#include <string>

// Settings used to initialize a window
struct WindowSettings {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  bool center = true;
  enum State { normal, maximized, full_screen } state = normal;
  std::string title;
  std::string icon;
};

/*
 * Represents an application window.
 */
class Window {
public:
  Window() { }
  virtual ~Window() { }

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  virtual int width() const = 0;
  virtual int height() const = 0;
};

#endif

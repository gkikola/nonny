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

#include "video/sdl/sdl_window.hpp"

#include "main/sdl/sdl_error.hpp"

SDLWindow::SDLWindow(const WindowSettings& ws)
{
  Uint32 flags = SDL_WINDOW_RESIZABLE;
  switch (ws.state) {
  default:
  case WindowSettings::normal:
    break;
  case WindowSettings::maximized:
    flags |= SDL_WINDOW_MAXIMIZED;
    break;
  case WindowSettings::full_screen:
    flags |= SDL_WINDOW_FULLSCREEN;
    break;
  }

  m_window = SDL_CreateWindow(ws.title.c_str(),
                              (ws.center ? SDL_WINDOWPOS_CENTERED : ws.x),
                              (ws.center ? SDL_WINDOWPOS_CENTERED : ws.y),
                              ws.width, ws.height,
                              flags);
  if (!m_window)
    throw SDLError("SDL_CreateWindow");
}

SDLWindow::~SDLWindow()
{
  SDL_DestroyWindow(m_window);
}

unsigned SDLWindow::width() const
{
  int w;
  SDL_GetWindowSize(m_window, &w, NULL);

  return static_cast<unsigned>(w);
}

unsigned SDLWindow::height() const
{
  int h;
  SDL_GetWindowSize(m_window, NULL, &h);

  return static_cast<unsigned>(h);
}

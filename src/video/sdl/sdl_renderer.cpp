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

#include "video/sdl/sdl_renderer.hpp"

#include "utility/sdl/sdl_error.hpp"
#include "utility/utility.hpp"
#include "video/sdl/sdl_window.hpp"
#include "video/rect.hpp"

SDL_Rect rect_to_sdl_rect(const Rect& r)
{
  return SDL_Rect { r.x(), r.y(),
      static_cast<int>(r.width()), static_cast<int>(r.height()) };
}

SDLRenderer::SDLRenderer(Window& window)
{
  SDLWindow& swin = dynamic_cast<SDLWindow&>(window);
  m_renderer = SDL_CreateRenderer(swin.get_sdl_handle(), -1,
                                  SDL_RENDERER_ACCELERATED
                                  | SDL_RENDERER_PRESENTVSYNC);
  if (!m_renderer)
    throw SDLError("SDL_CreateRenderer");
}

SDLRenderer::~SDLRenderer()
{
  SDL_DestroyRenderer(m_renderer);
}

void SDLRenderer::draw_point(const Point& point)
{
  SDL_RenderDrawPoint(m_renderer, point.x, point.y);
}

void SDLRenderer::draw_line(const Point& point1, const Point& point2)
{
  SDL_RenderDrawLine(m_renderer, point1.x, point1.y, point2.x, point2.y);
}

void SDLRenderer::draw_rect(const Rect& rect)
{
  SDL_Rect srect = rect_to_sdl_rect(rect);
  SDL_RenderDrawRect(m_renderer, &srect);
}

void SDLRenderer::draw_dotted_rect(const Rect& rect)
{
}

void SDLRenderer::fill_rect(const Rect& rect)
{
  SDL_Rect srect = rect_to_sdl_rect(rect);
  SDL_RenderFillRect(m_renderer, &srect);
}

void SDLRenderer::set_draw_color(const Color& color)
{
  SDL_SetRenderDrawColor(m_renderer,
                         color.red(), color.green(), color.blue(),
                         255);
}

void SDLRenderer::set_viewport()
{
  SDL_RenderSetViewport(m_renderer, NULL);
}

void SDLRenderer::set_viewport(const Rect& rect)
{
  SDL_Rect srect = rect_to_sdl_rect(rect);
  SDL_RenderSetViewport(m_renderer, &srect);
}

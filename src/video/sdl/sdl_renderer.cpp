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

#include <vector>
#include "utility/sdl/sdl_error.hpp"
#include "utility/utility.hpp"
#include "video/sdl/sdl_font.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/sdl/sdl_window.hpp"
#include "video/point.hpp"
#include "video/rect.hpp"

SDL_Rect rect_to_sdl_rect(const Rect& r)
{
  return SDL_Rect { r.x(), r.y(), r.width(), r.height() };
}

SDLRenderer::SDLRenderer(Window& window)
  : m_draw_color{0, 0, 0, 255}
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
  SDL_RenderDrawPoint(m_renderer, point.x(), point.y());
}

void SDLRenderer::draw_line(const Point& point1, const Point& point2)
{
  SDL_RenderDrawLine(m_renderer, point1.x(), point1.y(),
                     point2.x(), point2.y());
}

void SDLRenderer::draw_dotted_line(const Point& start,
                                   int length, bool vertical)
{
  std::vector<SDL_Point> points;
  for (int n = 0; n <= length; n += 3) {
    if (vertical)
      points.push_back(SDL_Point { start.x(), start.y() + n });
    else
      points.push_back(SDL_Point { start.x() + n, start.y() });
  }

  SDL_RenderDrawPoints(m_renderer, points.data(), points.size());
}

void SDLRenderer::draw_rect(const Rect& rect)
{
  SDL_Rect srect = rect_to_sdl_rect(rect);
  SDL_RenderDrawRect(m_renderer, &srect);
}

void SDLRenderer::fill_rect(const Rect& rect)
{
  SDL_Rect srect = rect_to_sdl_rect(rect);
  SDL_RenderFillRect(m_renderer, &srect);
}

void SDLRenderer::set_draw_color(const Color& color)
{
  m_draw_color.r = color.red();
  m_draw_color.g = color.green();
  m_draw_color.b = color.blue();
  
  SDL_SetRenderDrawColor(m_renderer,
                         color.red(), color.green(), color.blue(),
                         255);
}

Rect SDLRenderer::draw_text(const Point& point, const Font& font,
                            const std::string& text)
{
  const SDLFont* sdl_font = dynamic_cast<const SDLFont*>(&font);
  if (!sdl_font)
    throw std::runtime_error("SDLRenderer::draw_text: "
                             "given Font is not an SDLFont");
  SDL_Surface* surface
    = TTF_RenderUTF8_Blended(sdl_font->get_sdl_handle(),
                             text.c_str(),
                             m_draw_color);
  SDLTexture texture(m_renderer, surface);
  SDL_FreeSurface(surface);

  Rect dest_rect(point.x(), point.y(), texture.width(), texture.height());
  copy_texture(texture, Rect(), dest_rect);
  return dest_rect;
}

void SDLRenderer::copy_texture(const Texture& src,
                               const Rect& src_rect, const Rect& dest_rect)
{
  const SDLTexture* texture = dynamic_cast<const SDLTexture*>(&src);
  if (!texture)
    throw std::runtime_error("SDLRenderer::copy: "
                             "given Texture is not an SDLTexture");

  SDL_Rect sr = rect_to_sdl_rect(src_rect);
  SDL_Rect dr = rect_to_sdl_rect(dest_rect);
  SDL_Rect* p_sr = src_rect ? &sr : NULL;
  SDL_Rect* p_dr = dest_rect ? &dr : NULL;
  SDL_RenderCopy(m_renderer, texture->get_sdl_handle(), p_sr, p_dr);
}

void SDLRenderer::set_clip_rect()
{
  SDL_RenderSetClipRect(m_renderer, NULL);
}

void SDLRenderer::set_clip_rect(const Rect& rect)
{
  SDL_Rect srect = rect_to_sdl_rect(rect);
  SDL_RenderSetClipRect(m_renderer, &srect);
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

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

#include "ui/static_image.hpp"

#include "video/renderer.hpp"

void StaticImage::draw(Renderer& renderer, const Rect& region) const
{
  if (m_texture) {
    Rect location(m_boundary.x()
                  + m_boundary.width() / 2
                  - m_texture->width() / 2,
                  m_boundary.y()
                  + m_boundary.height() / 2
                  - m_texture->height() / 2,
                  m_texture->width(), m_texture->height());
    renderer.copy_texture(*m_texture, Rect(), location);
  }
}

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

#include "ui/image_button.hpp"

#include "video/font.hpp"
#include "video/rect.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"

constexpr int num_cells = 4;

void ImageButton::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(region);

  draw_background(renderer);
  if (has_focus())
    draw_sel_rect(renderer);
  draw_image(renderer);

  renderer.set_clip_rect();
}

void ImageButton::draw_image(Renderer& renderer) const
{
  if (m_texture) {
    int cell_size = m_texture->height() / num_cells;
    int state = 0;
    if (m_depressed)
      state = 3;
    else if (m_mouse_hover)
      state = 1;
    else if (has_focus())
      state = 2;

    Rect src(m_cell * cell_size, state * cell_size,
             cell_size, cell_size);
    renderer.copy_texture(*m_texture, src, m_boundary);
  }
}

void ImageButton::calc_size()
{
  int width = 0, height = 0;
  if (m_texture)
    width = height = m_texture->height() / num_cells;

  resize(width, height);
}

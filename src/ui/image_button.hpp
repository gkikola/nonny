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

#ifndef NONNY_IMAGE_BUTTON_HPP
#define NONNY_IMAGE_BUTTON_HPP

#include "ui/button.hpp"

class Texture;

class ImageButton : public Button {
public:
  ImageButton() = default;
  explicit ImageButton(const Texture& texture, unsigned cell = 0)
    : m_texture(&texture), m_cell(cell) { calc_size(); }

  ImageButton(const ImageButton&) = default;
  ImageButton(ImageButton&&) = default;
  ImageButton& operator=(const ImageButton&) = default;
  ImageButton& operator=(ImageButton&&) = default;

  using UIPanel::draw; //make draw overloads visible
  void draw(Renderer& renderer, const Rect& region) const override;

protected:
  void draw_image(Renderer& renderer) const;
  
private:
  void calc_size();
  
  const Texture* m_texture;
  unsigned m_cell = 0;
};

#endif

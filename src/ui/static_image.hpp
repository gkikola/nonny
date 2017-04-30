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

#ifndef NONNY_STATIC_IMAGE_HPP

#include "ui/control.hpp"
#include "video/texture.hpp"

class Texture;

class StaticImage : public Control {
public:
  StaticImage() = default;
  StaticImage(const Texture& texture)
    : m_texture(&texture) { resize(texture.width(), texture.height()); }

  using UIPanel::update;
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override { }
  void draw(Renderer& renderer, const Rect& region) const override;

  bool can_focus() const override { return false; }

private:
  const Texture* m_texture = nullptr;
};

#endif

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

#ifndef NONNY_SPACER_HPP
#define NONNY_SPACER_HPP

#include "ui/control.hpp"

/*
 * A sizeless control intended as a placeholder. Can be used in menus
 * to add space between other controls.
 */
class Spacer : public Control {
public:
  Spacer() { resize(0, 0); }

  using UIPanel::update;
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override { }
  void draw(Renderer& renderer, const Rect& region) const override { }

  bool can_focus() const override { return false; }
};

#endif

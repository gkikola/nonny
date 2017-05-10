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

#ifndef NONNY_VICTORY_VIEW_HPP
#define NONNY_VICTORY_VIEW_HPP

#include "view/view.hpp"

class InputHandler;
class Puzzle;
class Renderer;
class ViewManager;

class VictoryView : public View {
public:
  VictoryView(ViewManager& vm, Puzzle& puzzle)
    : View(vm), m_puzzle(puzzle) { }
  VictoryView(ViewManager& vm, Puzzle& puzzle,
              unsigned width, unsigned height)
    : View(vm, width, height), m_puzzle(puzzle) { }

  VictoryView(const VictoryView&) = default;
  VictoryView(VictoryView&&) = default;

  VictoryView& operator=(const VictoryView&) & = default;
  VictoryView& operator=(VictoryView&&) = default;

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(unsigned width, unsigned height) override;

private:
  Puzzle& m_puzzle;
};

#endif

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

#include <memory>
#include "ui/button.hpp"
#include "ui/puzzle_preview.hpp"
#include "video/font.hpp"
#include "view/view.hpp"

class InputHandler;
class Puzzle;
class Renderer;
class ViewManager;

class VictoryView : public View {
public:
  VictoryView(ViewManager& vm, Puzzle& puzzle)
    : View(vm), m_puzzle(puzzle) { load_resources(); }
  VictoryView(ViewManager& vm, Puzzle& puzzle,
              unsigned width, unsigned height)
    : View(vm, width, height), m_puzzle(puzzle) { load_resources(); }

  VictoryView(const VictoryView&) = default;
  VictoryView(VictoryView&&) = default;

  VictoryView& operator=(const VictoryView&) & = default;
  VictoryView& operator=(VictoryView&&) = default;

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(unsigned width, unsigned height) override;

private:
  void load_resources();
  
  Puzzle& m_puzzle;
  std::string m_puzzle_title;
  std::string m_puzzle_author;
  std::unique_ptr<Font> m_title_font;
  std::unique_ptr<Font> m_info_font;

  std::shared_ptr<Button> m_back_button;

  PuzzlePreview m_preview;
};

#endif

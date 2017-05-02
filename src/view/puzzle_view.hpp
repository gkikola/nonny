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

#ifndef NONNY_PUZZLE_VIEW_HPP
#define NONNY_PUZZLE_VIEW_HPP

#include <string>
#include "puzzle/puzzle.hpp"
#include "ui/scrolling_panel.hpp"
#include "video/font.hpp"
#include "video/texture.hpp"
#include "view/view.hpp"

class PuzzleView : public View {
public:
  PuzzleView(ViewManager& vm, const std::string& filename);
  PuzzleView(ViewManager& vm, const std::string& filename,
             unsigned width, unsigned height);

  PuzzleView(const PuzzleView& pv);
  PuzzleView(PuzzleView&&);
  PuzzleView& operator=(const PuzzleView& pv) &;
  PuzzleView& operator=(PuzzleView&& pv) &;

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(unsigned width, unsigned height) override;

private:
  void load(const std::string& filename);
  void setup_panels();
  
  Puzzle m_puzzle;
  ScrollingPanel m_main_panel;
  ScrollingPanel m_info_pane;

  std::unique_ptr<Font> m_clue_font;
  std::unique_ptr<Texture> m_cell_texture;

  std::unique_ptr<Font> m_title_font;
  std::unique_ptr<Font> m_info_font;
  std::unique_ptr<Font> m_button_font;
};

#endif

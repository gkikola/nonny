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

#ifndef NONNY_PUZZLE_PANEL_HPP
#define NONNY_PUZZLE_PANEL_HPP

#include "ui/ui_panel.hpp"

class Font;
class InputHandler;
class Puzzle;
class Renderer;
class Texture;

class PuzzlePanel : public UIPanel {
public:
  PuzzlePanel(Font& rule_font, const Texture& cell_texture)
    : m_rule_font(rule_font), m_cell_texture(cell_texture) { }
  PuzzlePanel(Font& rule_font, const Texture& cell_texture, Puzzle& puzzle);

  void attach_puzzle(Puzzle& puzzle) { m_puzzle = &puzzle; }

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;
private:
  void draw_grid_lines(Renderer& renderer) const;

  Font& m_rule_font;
  const Texture& m_cell_texture;
  
  Puzzle* m_puzzle = nullptr;
  unsigned m_cell_size = 32;
};

#endif

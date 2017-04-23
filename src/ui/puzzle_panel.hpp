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

#include <vector>
#include "color/color_palette.hpp"
#include "puzzle/puzzle_cell.hpp"
#include "ui/ui_panel.hpp"
#include "video/point.hpp"

class Font;
class InputHandler;
class Puzzle;
class Renderer;
class Texture;

class PuzzlePanel : public UIPanel {
public:
  PuzzlePanel(Font& clue_font, const Texture& cell_texture)
    : m_clue_font(clue_font), m_cell_texture(cell_texture) { }
  PuzzlePanel(Font& clue_font, const Texture& cell_texture, Puzzle& puzzle);

  void attach_puzzle(Puzzle& puzzle);

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

  void move(int x, int y);
private:
  void calc_grid_pos();
  unsigned row_clue_width(unsigned row) const;
  unsigned col_clue_height(unsigned col) const;
  int clue_spacing() const { return m_cell_size / 3; }
  void draw_grid_lines(Renderer& renderer) const;
  void draw_clues(Renderer& renderer) const;
  void draw_cells(Renderer& renderer) const;

  void update_cells(unsigned ticks);
  void set_cell(unsigned x, unsigned y, PuzzleCell::State state);

  void next_color();

  Font& m_clue_font;
  const Texture& m_cell_texture;
  
  Puzzle* m_puzzle = nullptr;
  ColorPalette::const_iterator m_color;
  std::vector<unsigned> m_cell_time;
  std::vector<PuzzleCell::State> m_prev_cell_state;
  unsigned m_cell_size = 32;
  Point m_grid_pos;
};

#endif

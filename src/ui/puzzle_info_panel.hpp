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

#ifndef NONNY_PUZZLE_INFO_PANEL_HPP
#define NONNY_PUZZLE_INFO_PANEL_HPP

#include "ui/ui_panel.hpp"

class Font;
class Puzzle;

class PuzzleInfoPanel : public UIPanel {
public:
  PuzzleInfoPanel(Font& title_font, Font& info_font, Font& button_font);
  PuzzleInfoPanel(Font& title_font, Font& info_font, Font& button_font,
                  Puzzle& puzzle);
  void attach_puzzle(Puzzle& puzzle);

  //start/stop sliding animation
  void start_slide() { m_sliding = true; }
  void stop_slide() { m_sliding = false; }

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;
private:
  Font& m_title_font;
  Font& m_info_font;
  Font& m_button_font;

  Puzzle* m_puzzle = nullptr;

  bool m_sliding = false;
};

#endif

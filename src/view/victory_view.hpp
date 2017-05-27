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

/*
 * Victory screen displayed when the user completes a puzzle. Shows a
 * picture of the puzzle and completion times.
 */
class VictoryView : public View {
public:
  VictoryView(ViewManager& vm, Puzzle& puzzle,
              unsigned clear_time, unsigned best_time);
  VictoryView(ViewManager& vm, Puzzle& puzzle,
              unsigned clear_time, unsigned best_time,
              int width, int height);

  VictoryView(const VictoryView&) = default;
  VictoryView(VictoryView&&) = default;

  VictoryView& operator=(const VictoryView&) & = default;
  VictoryView& operator=(VictoryView&&) = default;

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(int width, int height) override;

private:
  void load_resources();
  
  Puzzle& m_puzzle;
  unsigned m_clear_time;
  unsigned m_best_time;
  std::string m_puzzle_title;
  std::string m_puzzle_author;
  std::string m_times;
  std::unique_ptr<Font> m_title_font;
  std::unique_ptr<Font> m_info_font;

  std::shared_ptr<Button> m_back_button;

  PuzzlePreview m_preview;
};

#endif

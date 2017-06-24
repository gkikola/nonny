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

#ifndef NONNY_ANALYZE_VIEW_HPP
#define NONNY_ANALYZE_VIEW_HPP

#include <memory>
#include "ui/analysis_panel.hpp"
#include "video/font.hpp"
#include "view/view.hpp"

class Puzzle;
class ViewManager;

/*
 * Game screen that displays solution analysis such as number of
 * solutions, solvability, and difficulty. Also includes a solution
 * preview.
 */
class AnalyzeView : public View {
public:
  AnalyzeView(ViewManager& vm, Puzzle& puzzle);

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(int width, int height) override;

  bool is_transparent() const override { return true; }

private:
  void load_resources();

  Puzzle& m_puzzle;

  std::shared_ptr<AnalysisPanel> m_panel;
  std::unique_ptr<Font> m_font;
};

#endif

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

#ifndef NONNY_DRAW_TOOL_PANEL
#define NONNY_DRAW_TOOL_PANEL

#include <functional>
#include "ui/puzzle_panel.hpp"
#include "ui/ui_panel.hpp"

class Texture;

class DrawToolPanel : public UIPanel {
public:
  DrawToolPanel(Texture& texture) : m_texture(texture) { calc_size(); }

  // Register callback functions
  typedef std::function<void()> Callback;
  void on_tool_change(Callback fn) { m_tool_callback = fn; }

  PuzzlePanel::DrawTool selected_tool() const { return m_cur_tool; }

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

private:
  void calc_size();

  Texture& m_texture;
  Callback m_tool_callback;
  PuzzlePanel::DrawTool m_cur_tool = PuzzlePanel::DrawTool::paint;
};

#endif

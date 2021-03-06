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

/*
 * Puzzle screen. This View is used when the user is playing or
 * editing a puzzle.
 */
class PuzzleView : public View {
public:
  // Load a blank puzzle in edit mode
  PuzzleView(ViewManager& vm);
  PuzzleView(ViewManager& vm, int width, int height);

  // Load the designated puzzle file
  PuzzleView(ViewManager& vm, const std::string& filename);
  PuzzleView(ViewManager& vm, const std::string& filename,
             int width, int height);

  PuzzleView(const PuzzleView& pv);
  PuzzleView(PuzzleView&&);
  PuzzleView& operator=(const PuzzleView& pv) &;
  PuzzleView& operator=(PuzzleView&& pv) &;

  void update(unsigned ticks, InputHandler& input) override;
  void draw(Renderer& renderer) override;
  void resize(int width, int height) override;

  void save_progress();
  void restart();
  void save_puzzle(std::string filename = "");
  void update_properties();
  void set_edit_mode();
  void solve_and_edit();

  bool is_editing_mode_active() const { return m_edit_mode; }
  bool is_save_needed() const;

  Puzzle& puzzle() { return m_puzzle; }
  const Puzzle& puzzle() const { return m_puzzle; }

  unsigned time() const;
  unsigned best_time() const { return m_best_time; }

private:
  void load(const std::string& filename);
  PuzzleFormat file_type(const std::string& filename) const;
  void new_puzzle();
  void setup_default_palette();
  void setup_panels();
  void handle_color_change();
  void handle_tool_change();
  void save();
  void enable_editing();
  std::string puzzle_id() const;
  std::string puzzle_collection() const;

  Puzzle m_puzzle;
  std::string m_puzzle_filename;
  bool m_edit_mode = false;
  unsigned m_best_time = 0;
  bool m_ask_before_save = false;
  bool m_draw_tooltips = true;

  ScrollingPanel m_main_panel;
  ScrollingPanel m_info_pane;

  std::unique_ptr<Font> m_clue_font;
  std::unique_ptr<Texture> m_cell_texture;

  std::unique_ptr<Font> m_title_font;
  std::unique_ptr<Font> m_info_font;
  std::unique_ptr<Font> m_size_font;
  std::unique_ptr<Texture> m_ctrl_texture;
  std::unique_ptr<Texture> m_arrow_texture;
  std::unique_ptr<Texture> m_draw_texture;
};

void read_puzzle_png(const std::string& filename, Puzzle& puzzle);
void write_puzzle_png(const std::string& filename, Puzzle& puzzle);

#endif

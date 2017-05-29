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

#include <functional>
#include <memory>
#include <vector>
#include "ui/draw_tool_panel.hpp"
#include "ui/image_button.hpp"
#include "ui/palette_panel.hpp"
#include "ui/puzzle_panel.hpp"
#include "ui/puzzle_preview.hpp"
#include "ui/ui_panel.hpp"

class Font;
class Puzzle;
class Texture;

/*
 * Displays information about a puzzle that the user is solving and
 * holds some buttons to allow the user to access the menu or change
 * options.
 */
class PuzzleInfoPanel : public UIPanel {
public:
  PuzzleInfoPanel(Font& title_font, Font& info_font, Font& size_font,
                  Texture& ctrl_texture, Texture& arrow_texture,
                  Texture& draw_texture, int max_width,
                  bool edit_mode = false);
  void attach_puzzle(Puzzle& puzzle);

  // Register callback functions
  typedef std::function<void()> Callback;
  void on_menu_open(Callback fn) { m_buttons[menu]->register_callback(fn); }
  void on_zoom_in(Callback fn) { m_buttons[zoom_in]->register_callback(fn); }
  void on_zoom_out(Callback fn) { m_buttons[zoom_out]->register_callback(fn); }
  void on_hint_toggle(Callback fn);
  void on_clear_puzzle(Callback fn);
  void on_save(Callback fn) { m_buttons[save]->register_callback(fn); }
  void on_undo(Callback fn) { m_buttons[undo]->register_callback(fn); }
  void on_redo(Callback fn) { m_buttons[redo]->register_callback(fn); }
  void on_analyze(Callback fn) { m_buttons[analyze]->register_callback(fn); }
  void on_left(Callback fn);
  void on_right(Callback fn);
  void on_up(Callback fn);
  void on_down(Callback fn);
  void on_data_edit_request(Callback fn);
  void on_tool_change(Callback fn) { m_tool_selector.on_tool_change(fn); }
  void on_color_change(Callback fn) { m_color_selector.on_color_change(fn); }
  
  // Start/stop sliding animation
  void start_slide() { m_sliding = true; }
  void stop_slide() { m_sliding = false; }

  // Set or clear puzzle editing mode
  void set_edit_mode(bool edit_mode = true);
  
  // Get currently selected color
  Color active_color() const;

  // Get currently selected draw tool
  PuzzlePanel::DrawTool active_draw_tool() const;
  
  // Get/set time puzzle has been open
  unsigned time() const { return m_time; }
  void time(unsigned time) { m_time = time; }

  // Reload puzzle metadata and size
  void refresh_puzzle_properties();

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;

  void draw(Renderer& renderer, const Rect& region) const override;

  void move(int x, int y) override;

private:
  void setup_buttons();
  void retrieve_puzzle_info();
  void calculate_bounds();

  Font& m_title_font;
  Font& m_info_font;
  Font& m_size_font;
  Texture& m_ctrl_texture;
  Texture& m_arrow_texture;
  Texture& m_draw_texture;
  Puzzle* m_puzzle = nullptr;
  std::string m_puzzle_title;
  std::string m_puzzle_author;
  std::string m_puzzle_size;

  PuzzlePreview m_preview;
  PalettePanel m_color_selector;
  DrawToolPanel m_tool_selector;

  enum { menu = 0, zoom_in, zoom_out, hint,
         save, undo, redo, analyze, left, right, up, down };
  const int num_buttons = down + 1;
  std::vector<std::shared_ptr<ImageButton>> m_buttons;
  Callback m_left_callback;
  Callback m_right_callback;
  Callback m_up_callback;
  Callback m_down_callback;
  Callback m_hint_callback;
  Callback m_clear_callback;
  Callback m_data_edit_callback;

  bool m_sliding = false;
  unsigned m_time = 0;
  int m_max_width = 0;
  bool m_edit_mode = false;
};

#endif

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

#ifndef NONNY_PALETTE_PANEL_HPP
#define NONNY_PALETTE_PANEL_HPP

#include <functional>
#include "color/color.hpp"
#include "ui/ui_panel.hpp"
#include "video/point.hpp"
#include "video/rect.hpp"

class ColorPalette;

/*
 * Displays the colors in a palette and allows the user to
 * select a color.
 */
class PalettePanel : public UIPanel {
public:
  PalettePanel() { }
  PalettePanel(const ColorPalette& palette);

  void set_palette(const ColorPalette& palette);

  // Register callback function
  typedef std::function<void()> Callback;
  void on_color_change(Callback fn) { m_color_callback = fn; }

  // Gets the selected color
  Color selected_color() const { return m_colors[m_selection]; }

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

  /*
   * Set how wide the panel should be. Determines wrapping of color
   * buttons.
   */
  void set_width(unsigned width);

private:
  void prev_color();
  void next_color();
  void set_selection(unsigned selection);

  unsigned colors_per_line() const; //how many colors fit on a line
  unsigned entry_at_point(Point pt) const;
  Rect entry_rect(unsigned index) const;
  void update_offset(); //calculate position of color button placement

  std::vector<Color> m_colors;
  Color m_bkgd_color;
  unsigned m_selection = 0;
  Callback m_color_callback;
  int m_offset = 0;
};

#endif

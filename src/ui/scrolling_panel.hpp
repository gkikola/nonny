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

#ifndef NONNY_SCROLLING_PANEL_HPP
#define NONNY_SCROLLING_PANEL_HPP

#include <memory>
#include "ui/scrollbar.hpp"
#include "ui/ui_panel.hpp"

/*
 * An interface element containing scrollbars that allow the user to
 * scroll a particular panel.
 */
class ScrollingPanel : public UIPanel {
public:
  ScrollingPanel() { }
  explicit ScrollingPanel(const Rect& boundary) : UIPanel(boundary) { }
  ScrollingPanel(const Rect& boundary, UIPanelPtr child)
    : UIPanel(boundary) { attach_panel(child); }

  void attach_panel(UIPanelPtr child);

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;
  void resize(int width, int height) override;

  UIPanel& main_panel() { return *m_main_panel; }
  const UIPanel& main_panel() const { return *m_main_panel; }
  void center_main_panel() { center_panel_vert(); center_panel_horiz(); }
  void move_panel_in_bounds();
  int scrollbar_width() const { return s_scrollbar_width; }

  void smooth_scroll_up();
  void smooth_scroll_down();

private:
  void center_panel_vert();
  void center_panel_horiz();

  UIPanelPtr m_main_panel = nullptr;
  int m_main_panel_width = 0;
  int m_main_panel_height = 0;
  Scrollbar m_hscroll;
  Scrollbar m_vscroll;
  bool m_hscroll_active = false;
  bool m_vscroll_active = false;
  int m_smooth_scroll_amount = 0;

  static const int s_scrollbar_width = 16;
};

#endif

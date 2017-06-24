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

#ifndef NONNY_DIALOG_HPP
#define NONNY_DIALOG_HPP

#include <vector>
#include "ui/control.hpp"
#include "ui/ui_panel.hpp"

/*
 * Dialog base class. A Dialog is a collection of controls presented
 * to the user as a menu or message box.
 */
class Dialog : public UIPanel {
public:
  Dialog() : m_focused(m_controls.end()) { }
  virtual ~Dialog() { }

  virtual void add_control(ControlPtr control);
  virtual void position_controls() = 0;

  void focus_prev();
  void focus_next();

  using UIPanel::update; //make update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

  void move(int x, int y) override;

protected:
  void find_focus();
  void give_focus();
  void remove_focus();

  virtual bool allow_arrow_focus() const { return false; }

  std::vector<ControlPtr> m_controls;
  std::vector<ControlPtr>::iterator m_focused;
  bool m_need_reposition = false;
};

#endif

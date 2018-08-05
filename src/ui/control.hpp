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

#ifndef NONNY_CONTROL_HPP
#define NONNY_CONTROL_HPP

#include "ui/ui_panel.hpp"

/*
 * Control base class. A Control is an interface element, like a
 * button or a text box, that the user can interact with.
 */
class Control : public UIPanel {
public:
  Control() = default;
  explicit Control(const Rect& boundary) : UIPanel(boundary) { }
  Control(const Control&) = default;
  Control(Control&&) = default;
  virtual ~Control() { }

  Control& operator=(const Control&) & = default;
  Control& operator=(Control&&) & = default;

  virtual void give_focus() { m_focused = true; }
  virtual void remove_focus() { m_focused = false; }
  virtual bool can_focus() const { return true; }
  virtual bool has_focus() const { return m_focused; }

  virtual void enable() { m_enabled = true; }
  virtual void disable() { m_enabled = false; }
  virtual bool is_enabled() const { return m_enabled; }

private:
  bool m_focused = false;
  bool m_enabled = false;
};

typedef std::shared_ptr<Control> ControlPtr;

template <typename ControlType, typename... Args>
ControlPtr make_control(Args& ... args);


/* implementation */

template <typename ControlType, typename... Args>
ControlPtr make_control(Args& ... args)
{
  return std::make_shared<ControlType>(args...);
}

#endif

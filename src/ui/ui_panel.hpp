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

#ifndef NONNY_UI_PANEL_HPP
#define NONNY_UI_PANEL_HPP

#include <memory>
#include <vector>
#include "video/rect.hpp"

class InputHandler;
class Renderer;

class UIPanel {
public:
  UIPanel() { }
  explicit UIPanel(const Rect& boundary)
    : m_boundary(boundary) { }
  UIPanel(const UIPanel&) = default;
  UIPanel(UIPanel&&) = default;
  virtual ~UIPanel() { }

  UIPanel& operator=(const UIPanel&) & = default;
  UIPanel& operator=(UIPanel&&) & = default;

  virtual void update(unsigned ticks, InputHandler& input) = 0;
  virtual void draw(Renderer& renderer) { draw(renderer, m_boundary); }
  virtual void draw(Renderer& renderer, const Rect& region) const = 0;
  
  virtual void move(int x, int y) { m_boundary.move(x, y); }
  virtual void scroll(int x, int y);
  virtual void resize(unsigned width, unsigned height);
  
  Rect boundary() const { return m_boundary; }

protected:
  Rect m_boundary;
};

typedef std::shared_ptr<UIPanel> UIPanelPtr;

template <typename PanelType, typename... Args>
UIPanelPtr make_ui_panel(Args& ... args);


/* implementation */

template <typename PanelType, typename... Args>
UIPanelPtr make_ui_panel(Args& ... args)
{
  return std::make_shared<PanelType>(args...);
}

#endif

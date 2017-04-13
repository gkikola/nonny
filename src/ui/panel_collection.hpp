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

#ifndef NONNY_PANEL_COLLECTION_HPP
#define NONNY_PANEL_COLLECTION_HPP

#include "ui/ui_panel.hpp"

class PanelCollection : public UIPanel {
public:
  typedef std::vector<std::shared_ptr<UIPanel>>::iterator iterator;
  typedef std::vector<std::shared_ptr<UIPanel>>::const_iterator const_iterator;
  
  PanelCollection() { }
  explicit PanelCollection(const Rect& boundary) : UIPanel(boundary) { }
  PanelCollection(const PanelCollection&) = default;
  PanelCollection(PanelCollection&&) = default;

  PanelCollection& operator=(const PanelCollection&) & = default;
  PanelCollection& operator=(PanelCollection&&) & = default;

  void insert(std::shared_ptr<UIPanel> child);
  void erase(iterator pos);

  iterator begin() { return m_children.begin(); }
  const_iterator begin() const { return m_children.begin(); }
  iterator end() { return m_children.end(); }
  const_iterator end() const { return m_children.end(); }
};

#endif

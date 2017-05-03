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

#ifndef NONNY_PUZZLE_PANEL_HPP
#define NONNY_PUZZLE_PANEL_HPP

#include <vector>
#include "color/color_palette.hpp"
#include "puzzle/puzzle.hpp"
#include "puzzle/puzzle_cell.hpp"
#include "ui/ui_panel.hpp"
#include "video/point.hpp"

class Font;
class InputHandler;
class Renderer;
class Texture;

class PuzzlePanel : public UIPanel {
public:
  PuzzlePanel(Font& clue_font, const Texture& cell_texture)
    : m_clue_font(clue_font), m_cell_texture(cell_texture) { }
  PuzzlePanel(Font& clue_font, const Texture& cell_texture, Puzzle& puzzle);

  void attach_puzzle(Puzzle& puzzle);

  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

  void move(int x, int y);

private:
  void calc_grid_pos();
  unsigned row_clue_width(unsigned row) const;
  unsigned col_clue_height(unsigned col) const;
  int clue_spacing() const { return m_cell_size / 3; }
  void draw_grid_lines(Renderer& renderer) const;
  void draw_clues(Renderer& renderer) const;
  void draw_cells(Renderer& renderer) const;
  void draw_selection(Renderer& renderer) const;

  void update_cells(unsigned ticks);
  void set_cell(unsigned x, unsigned y, PuzzleCell::State state);
  void drag_over_cell(unsigned x, unsigned y);
  void handle_mouse_selection(unsigned ticks, InputHandler& input,
                              const Rect& region);
  void handle_kb_selection(unsigned ticks, InputHandler& input);

  enum class Direction { up, down, left, right };
  void move_selection(Direction dir);
  void next_color();
  inline bool is_point_in_grid(const Point& p) const;
  inline void cell_at_point(const Point& p, unsigned* x, unsigned* y) const;

  Font& m_clue_font;
  const Texture& m_cell_texture;

  enum class DragType { fill, cross, blanking_fill, blanking_cross };

  //Puzzle information
  Puzzle* m_puzzle = nullptr;
  ColorPalette::const_iterator m_color;
  std::vector<unsigned> m_cell_time;
  std::vector<PuzzleCell::State> m_prev_cell_state;
  unsigned m_cell_size = 32;
  Point m_grid_pos;

  //Dragging states
  DragType m_mouse_drag_type = DragType::fill;
  DragType m_kb_drag_type = DragType::fill;
  bool m_mouse_dragging = false;
  bool m_kb_dragging = false;

  //Mouse lock state
  bool m_mouse_locked = false;
  enum class MouseLockType { to_row, to_col } m_mouse_lock_type;
  unsigned m_mouse_lock_pos = 0;
  unsigned m_mouse_lock_time = 0;
  unsigned m_drag_start_x = 0;
  unsigned m_drag_start_y = 0;

  //Arrow key states
  Direction m_kb_dir;
  bool m_kb_dir_held = false;
  unsigned m_ticks_until_sel_change = 0;

  //Cell selection
  bool m_selected = false;
  unsigned m_selection_x = 0;
  unsigned m_selection_y = 0;
};


/* implementation */

inline bool PuzzlePanel::is_point_in_grid(const Point& p) const
{
  const int cell_size = static_cast<int>(m_cell_size);
  const int width = static_cast<int>(m_puzzle->width());
  const int height = static_cast<int>(m_puzzle->height());
  return p.x() > m_grid_pos.x() && p.y() > m_grid_pos.y()
    && p.x() <= m_grid_pos.x() + width * (cell_size + 1)
    && p.y() <= m_grid_pos.y() + height * (cell_size + 1);
}

inline void PuzzlePanel::cell_at_point(const Point& p,
                                       unsigned* x, unsigned* y) const
{
  if (x) {
    if (p.x() < m_grid_pos.x() + 1)
      *x = 0;
    else
      *x = (p.x() - m_grid_pos.x() - 1) / (m_cell_size + 1);

    if (*x >= m_puzzle->width())
      *x = m_puzzle->width() - 1;
  }
  if (y) {
    if (p.y() < m_grid_pos.y() + 1)
      *y = 0;
    else
      *y = (p.y() - m_grid_pos.y() - 1) / (m_cell_size + 1);

    if (*y >= m_puzzle->height())
      *y = m_puzzle->height() - 1;
  }
}

#endif

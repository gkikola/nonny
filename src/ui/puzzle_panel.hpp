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

#include <functional>
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

/*
 * Represents the user interface for solving (or editing) a puzzle.
 */
class PuzzlePanel : public UIPanel {
public:
  PuzzlePanel(Font& clue_font, const Texture& cell_texture)
    : m_clue_font(clue_font), m_cell_texture(cell_texture) { }
  PuzzlePanel(Font& clue_font, const Texture& cell_texture, Puzzle& puzzle);

  void attach_puzzle(Puzzle& puzzle);

  void set_active_color(Color color) { m_color = color; }

  enum class DrawTool { paint = 0, line, rect, ellipse, fill };
  void set_edit_mode(bool edit_mode = true) { m_edit_mode = edit_mode; }
  void set_draw_tool(DrawTool tool) { m_draw_tool = tool; }
  
  using UIPanel::update; //make all update and draw overloads visible
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

  void move(int x, int y);

private:
  void calc_grid_pos();
  int row_clue_width(int row) const;
  int col_clue_height(int col) const;
  int clue_spacing() const { return m_cell_size / 3; }
  void draw_grid_lines(Renderer& renderer) const;
  void draw_clues(Renderer& renderer) const;
  void draw_cells(Renderer& renderer) const;
  void draw_cell(Renderer& renderer, int x, int y,
                 PuzzleCell::State state, PuzzleCell::State prev_state,
                 const Color& color, unsigned animation_time) const;
  void draw_selection(Renderer& renderer) const;

  void update_cells(unsigned ticks);
  void set_cell(int x, int y, PuzzleCell::State state);
  void drag_over_cell(int x, int y);
  void handle_mouse_selection(unsigned ticks, InputHandler& input,
                              const Rect& region);
  void handle_kb_selection(unsigned ticks, InputHandler& input);
  void do_draw_action(bool mark = true);

  typedef std::function<void(int, int)> CellFunction;
  void for_each_point_on_selection(CellFunction fn) const;
  void call_on_point(CellFunction fn, int x, int y) const;

  enum class Direction { up, down, left, right };
  void move_selection(Direction dir, int count = 1);
  inline bool is_point_in_grid(const Point& p) const;
  inline void cell_at_point(const Point& p, int* x, int* y) const;

  //find coordinates of cell without making sure they're in bounds
  inline void generalized_cell_at_point(const Point&p, int* x, int* y) const;

  Font& m_clue_font;
  const Texture& m_cell_texture;

  enum class DragType { fill, cross, blanking_fill, blanking_cross };

  //Puzzle information
  Puzzle* m_puzzle = nullptr;
  Color m_color;
  std::vector<unsigned> m_cell_time;
  std::vector<PuzzleCell::State> m_prev_cell_state;
  int m_cell_size = 32;
  Point m_grid_pos;
  bool m_edit_mode = false;
  DrawTool m_draw_tool = DrawTool::paint;

  //Dragging states
  DragType m_mouse_drag_type = DragType::fill;
  DragType m_kb_drag_type = DragType::fill;
  bool m_mouse_dragging = false;
  bool m_kb_dragging = false;

  //Mouse lock state
  bool m_mouse_locked = false;
  enum class MouseLockType { to_row, to_col } m_mouse_lock_type;
  int m_mouse_lock_pos = 0;
  int m_drag_start_x = 0;
  int m_drag_start_y = 0;
  int m_drag_end_x = 0;
  int m_drag_end_y = 0;
  bool m_drag_marks = false; //are we marking or clearing the cells?
  unsigned m_ticks_on_cur_cell = 0;

  //Cell selection
  bool m_selected = false;
  int m_selection_x = 0;
  int m_selection_y = 0;
};


/* implementation */

inline bool PuzzlePanel::is_point_in_grid(const Point& p) const
{
  const int cell_size = m_cell_size;
  const int width = m_puzzle->width();
  const int height = m_puzzle->height();
  return p.x() > m_grid_pos.x() && p.y() > m_grid_pos.y()
    && p.x() <= m_grid_pos.x() + width * (cell_size + 1)
    && p.y() <= m_grid_pos.y() + height * (cell_size + 1);
}

inline void PuzzlePanel::cell_at_point(const Point& p,
                                       int* x, int* y) const
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

inline void PuzzlePanel::generalized_cell_at_point(const Point&p,
                                                   int* x, int* y) const
{
  if (x)
    *x = (p.x() - m_grid_pos.x() - 1) / (m_cell_size + 1);
  if (y)
    *y = (p.y() - m_grid_pos.y() - 1) / (m_cell_size + 1);
}

#endif

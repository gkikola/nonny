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

#include "ui/puzzle_panel.hpp"

#include <string>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "puzzle/puzzle.hpp"
#include "utility/utility.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"

const Color cell_border_color(128, 128, 128);
const Color blank_cell_color(255, 255, 255);
const Color shaded_cell_color(230, 230, 255);
const Color lightly_shaded_cell_color(240, 240, 255);

PuzzlePanel::PuzzlePanel(Font& clue_font, const Texture& cell_texture,
                         Puzzle& puzzle)
  : m_clue_font(clue_font), m_cell_texture(cell_texture), m_puzzle(&puzzle)
{
  calc_grid_pos();
}

void PuzzlePanel::update(unsigned ticks, InputHandler& input,
                         const Rect& active_region)
{
}

void PuzzlePanel::draw(Renderer& renderer, const Rect& region) const
{
  if (m_puzzle) {
    renderer.set_viewport(region);
    draw_grid_lines(renderer);
    draw_clues(renderer);
    shade_cells(renderer);
  
    renderer.set_viewport();
  }
}

void PuzzlePanel::calc_grid_pos()
{
  if (m_puzzle) {
    m_clue_font.resize(m_cell_size * 3 / 5);
    m_grid_pos.x() = m_boundary.x();
    m_grid_pos.y() = m_boundary.y();

    for (unsigned i = 0; i < m_puzzle->width(); ++i) {
      int bottom = m_boundary.y() + col_clue_height(i);
      if (bottom > m_grid_pos.y())
        m_grid_pos.y() = bottom;
    }

    for (unsigned j = 0; j < m_puzzle->height(); ++j) {
      int side = m_boundary.x() + row_clue_width(j);
      if (side > m_grid_pos.x())
        m_grid_pos.x() = side;
    }

    m_boundary.width()
      = m_grid_pos.x() + m_puzzle->width() * (m_cell_size + 1) + 1;
    m_boundary.height()
      = m_grid_pos.y() + m_puzzle->height() * (m_cell_size + 1) + 1;
  }
}

unsigned PuzzlePanel::row_clue_width(unsigned row) const
{
  unsigned width = 0;
  for (auto clue : m_puzzle->row_clues(row)) {
    unsigned text_wd, text_ht;
    m_clue_font.text_size(std::to_string(clue.value), &text_wd, &text_ht);
    width += text_wd + clue_spacing();
  }
  return width;
}

unsigned PuzzlePanel::col_clue_height(unsigned col) const
{
  unsigned height = 0;
  for (auto clue : m_puzzle->col_clues(col)) {
    unsigned text_wd, text_ht;
    m_clue_font.text_size(std::to_string(clue.value), &text_wd, &text_ht);
    height += text_ht + clue_spacing();
  }
  return height;
}

void PuzzlePanel::draw_grid_lines(Renderer& renderer) const
{
  renderer.set_draw_color(cell_border_color);
  unsigned width = m_puzzle->width();
  unsigned height = m_puzzle->height();
  for (unsigned x = 0; x <= width; ++x) {
    Point start(m_grid_pos.x() + x * (m_cell_size + 1), m_grid_pos.y());
    Point end(start.x(), m_grid_pos.y() + height * (m_cell_size + 1));
    renderer.draw_line(start, end);
  }
  for (unsigned y = 0; y <= height; ++y) {
    Point start(m_grid_pos.x(), m_grid_pos.y() + y * (m_cell_size + 1));
    Point end(m_grid_pos.x() + width * (m_cell_size + 1), start.y());
    renderer.draw_line(start, end);
  }
}

void PuzzlePanel::draw_clues(Renderer& renderer) const
{
  int x, y;
  for (unsigned i = 0; i < m_puzzle->width(); ++i) {
    x = m_grid_pos.x() + i * (m_cell_size + 1);
    y = m_grid_pos.y() - col_clue_height(i);
    for (auto clue : m_puzzle->col_clues(i)) {
      renderer.set_draw_color(clue.color);
      std::string value = std::to_string(clue.value);
      unsigned wd, ht;
      m_clue_font.text_size(value, &wd, &ht);
      Point pos(x + (m_cell_size + 1) / 2 - wd / 2, y);
      renderer.draw_text(pos, m_clue_font, value);
      y += ht + clue_spacing();
    }
  }

  for (unsigned j = 0; j < m_puzzle->height(); ++j) {
    x = m_grid_pos.x() - row_clue_width(j);
    y = m_grid_pos.y() + j * (m_cell_size + 1);
    for (auto clue : m_puzzle->row_clues(j)) {
      renderer.set_draw_color(clue.color);
      std::string value = std::to_string(clue.value);
      unsigned wd, ht;
      m_clue_font.text_size(value, &wd, &ht);
      Point pos(x, y + (m_cell_size + 1) / 2 - ht / 2);
      renderer.draw_text(pos, m_clue_font, value);
      x += wd + clue_spacing();
    }
  }
}

void PuzzlePanel::shade_cells(Renderer& renderer) const
{
  for (unsigned y = 0; y < m_puzzle->height(); ++y) {
    for (unsigned x = 0; x < m_puzzle->width(); ++x) {
      Rect dest(m_grid_pos.x() + x * (m_cell_size + 1) + 1,
                m_grid_pos.y() + y * (m_cell_size + 1) + 1,
                m_cell_size, m_cell_size);

      if (x % 2 != y % 2)
        renderer.set_draw_color(lightly_shaded_cell_color);
      else if (x % 2 == 0)
        renderer.set_draw_color(shaded_cell_color);
      else
        renderer.set_draw_color(blank_cell_color);
      renderer.fill_rect(dest);
    }
  }
}

void PuzzlePanel::move(int x, int y)
{
  int delta_x = x - m_boundary.x();
  int delta_y = y - m_boundary.y();
  UIPanel::move(x, y);
  m_grid_pos.x() += delta_x;
  m_grid_pos.y() += delta_y;
}

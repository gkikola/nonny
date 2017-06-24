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

#include "ui/palette_panel.hpp"

#include <algorithm>
#include "color/color.hpp"
#include "color/color_palette.hpp"
#include "input/input_handler.hpp"
#include "video/renderer.hpp"

constexpr int color_size = 32;
constexpr int spacing = 4;

PalettePanel::PalettePanel(const ColorPalette& palette)
{
  set_palette(palette);
}

void PalettePanel::set_palette(const ColorPalette& palette)
{
  m_colors.clear();
  for (const auto& e : palette) {
    if (e.name != "background")
      m_colors.emplace_back(e.color);
    else
      m_bkgd_color = e.color;
  }
  update_offset();
}

void PalettePanel::update(unsigned ticks, InputHandler& input,
                          const Rect& active_region)
{
  if (input.was_mouse_button_pressed(Mouse::Button::left)) {
    if (intersection(active_region, m_boundary)
        .contains_point(input.mouse_position())) {
      set_selection(entry_at_point(input.mouse_position()));
    }
  }

  if (input.was_key_pressed(Keyboard::Key::lalt)
      || input.was_key_pressed(Keyboard::Key::ralt)
      || input.was_key_pressed(Keyboard::Key::end)
      || input.was_key_pressed(Keyboard::Key::kp_end))
    next_color();
  else if (input.was_key_pressed(Keyboard::Key::home)
           || input.was_key_pressed(Keyboard::Key::kp_home))
    prev_color();
}

void PalettePanel::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(region);

  for (int i = 0; i < static_cast<int>(m_colors.size()); ++i) {
    Rect r = entry_rect(i);
    renderer.set_draw_color(m_colors[i]);
    renderer.fill_rect(r);

    if (i == m_selection)
      renderer.set_draw_color(m_bkgd_color);
    else
      renderer.set_draw_color(default_colors::black);
    renderer.draw_rect(r);
  }

  renderer.set_clip_rect();
}

void PalettePanel::set_width(int width)
{
  resize(width, 0);
  if (colors_per_line() > 0 && !m_colors.empty()) {
    int num_lines = m_colors.size() / colors_per_line();
    if (m_colors.size() % colors_per_line() != 0)
      ++num_lines;
    resize(width, color_size * num_lines + spacing * (num_lines - 1));
  }
  update_offset();
}

void PalettePanel::prev_color()
{
  if (m_selection == 0)
    m_selection = m_colors.size() - 1;
  else
    --m_selection;
  if (m_color_callback)
    m_color_callback();
}

void PalettePanel::next_color()
{
  ++m_selection;
  if (m_selection >= static_cast<int>(m_colors.size()))
    m_selection = 0;
  if (m_color_callback)
    m_color_callback();
}

void PalettePanel::set_selection(int selection)
{
  m_selection = selection;
  if (m_color_callback)
    m_color_callback();
}

int PalettePanel::colors_per_line() const
{
  return (m_boundary.width() + spacing) / (color_size + spacing);
}

int PalettePanel::entry_at_point(Point pt) const
{
  int x = (pt.x() - m_boundary.x() - m_offset)
    / (color_size + spacing);
  int y = (pt.y() - m_boundary.y()) / (color_size + spacing);
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;

  int index = x + y * colors_per_line();
  if (index >= static_cast<int>(m_colors.size()))
    index = m_colors.size() - 1;

  return index;
}

Rect PalettePanel::entry_rect(int index) const
{
  int cpl = colors_per_line();
  int x = cpl ? index % cpl : 0;
  int y = cpl ? index / cpl : 0;

  return Rect(m_boundary.x() + m_offset + x * (color_size + spacing),
              m_boundary.y() + y * (color_size + spacing),
              color_size,
              color_size);
}

void PalettePanel::update_offset()
{
  if (m_colors.empty() || colors_per_line() == 0) {
    m_offset = 0;
  } else {
    int num_colors = colors_per_line();
    if (num_colors > static_cast<int>(m_colors.size()))
      num_colors = m_colors.size();
    int color_wd = color_size * num_colors + spacing * (num_colors - 1);
    if (m_boundary.width() >= color_wd)
      m_offset = m_boundary.width() / 2 - color_wd / 2;
    else
      m_offset = color_wd / 2 - m_boundary.width() / 2;
  }
}

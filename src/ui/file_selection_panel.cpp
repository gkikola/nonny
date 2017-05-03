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

#include "ui/file_selection_panel.hpp"

#include <algorithm>
#include <experimental/filesystem>
#include "color/color.hpp"
#include "input/input_handler.hpp"
#include "video/font.hpp"
#include "video/renderer.hpp"
#include "video/texture.hpp"

namespace stdfs = std::experimental::filesystem;

const Color background_color = default_colors::white;
const Color foreground_color = default_colors::black;
const Color selection_color = default_colors::blue;

FileSelectionPanel::FileSelectionPanel(Font& filename_font, Font& info_font,
                                       Texture& icons,
                                       const std::string& path)
  : m_filename_font(filename_font),
    m_info_font(info_font),
    m_path(path)
{
  load_file_list();
}

void FileSelectionPanel::open_path(const std::string& path)
{
  if (m_path != path) {
    m_path = path;
    load_file_list();
  }
}

std::string FileSelectionPanel::selected_file() const
{
  if (m_is_selected)
    return m_files[m_selection].filename;
  else
    return "";
}

void FileSelectionPanel::update(unsigned ticks, InputHandler& input,
                                const Rect& active_region)
{
}

void FileSelectionPanel::draw(Renderer& renderer, const Rect& region) const
{
  renderer.set_clip_rect(region);

  const int entry_size = 96;

  int min_y = region.y() - m_boundary.y();
  if (min_y < 0) min_y = 0;
  int max_y = min_y + region.height();
  unsigned min_index, max_index;

  if (m_files.empty()) {
    min_index = max_index = 0;
  } else {
    min_index = min_y / entry_size;
    if (min_index > 0)
      --min_index;
    if (min_index > m_files.size())
      min_index = m_files.size() - 1;

    max_index = max_y / entry_size + 1;
    if (max_index > m_files.size())
      max_index = m_files.size();
  }
  
  for (unsigned i = min_index; i < max_index; ++i) {
    if (m_is_selected && m_selection == i)
      renderer.set_draw_color(selection_color);
    else
      renderer.set_draw_color(background_color);
    renderer.fill_rect(Rect(m_boundary.x(),
                            m_boundary.y() + i * entry_size,
                            m_boundary.width(),
                            entry_size));
    renderer.set_draw_color(foreground_color);
    int x = m_boundary.x(), y = m_boundary.y() + i * entry_size;
    renderer.draw_text(Point(x, y),
                       m_filename_font, m_files[i].filename);
  }
  
  renderer.set_clip_rect();
}

void FileSelectionPanel::load_file_list()
{
  m_files.clear();

  if (!m_path.empty()) {
    stdfs::path p(m_path);

    for (const auto& file : stdfs::directory_iterator(p)) {
      FileInfo info;
      info.full_path = file.path();
      info.filename = file.path().filename();
      if (stdfs::is_directory(file))
        info.type = FileInfo::Type::directory;
      else { //check for puzzle file
        if (file.path().extension() == ".non")
          info.type = FileInfo::Type::puzzle_file;
        else
          info.type = FileInfo::Type::file;
      }
      m_files.push_back(info);
    }
  }

  resize(400, 96 * m_files.size());
}

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

#ifndef NONNY_FILE_SELECTION_PANEL
#define NONNY_FILE_SELECTION_PANEL

#include <functional>
#include <string>
#include <vector>
#include "ui/ui_panel.hpp"

class Font;
class Texture;

class FileSelectionPanel : public UIPanel {
public:
  typedef std::function<void(const std::string&)> Callback;
  
  FileSelectionPanel(Font& filename_font, Font& info_font,
                     Texture& icons, const std::string& path = "");

  void open_path(const std::string& path);
  void open_file(const std::string& file);
  std::string selected_file() const;
  std::string path() const { return m_path; }

  void on_file_select(Callback fn);
  void on_dir_change(Callback fn);

  using UIPanel::update;
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

private:
  unsigned entry_height() const;
  void load_file_list();
  void sort_files();

  struct FileInfo;
  static bool file_info_less_than(const FileInfo& l, const FileInfo& r);

  struct FileInfo {
    std::string filename;
    std::string full_path;
    enum class Type { directory, file, puzzle_file } type;
    std::string puzzle_name;
    unsigned puzzle_width = 0;
    unsigned puzzle_height = 0;
  };
  
  const Font& m_filename_font;
  const Font& m_info_font;
  const Texture& m_icon_texture;
  std::string m_path;
  std::vector<FileInfo> m_files;
  unsigned m_selection = 0;
  bool m_is_selected = false;

  Callback m_file_callback;
  Callback m_dir_callback;
};

#endif

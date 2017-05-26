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
#include <memory>
#include <string>
#include <vector>
#include "puzzle/puzzle_progress.hpp"
#include "puzzle/puzzle_summary.hpp"
#include "ui/ui_panel.hpp"

class Font;
class SaveManager;
class Texture;

/*
 * A file viewer/selector that shows information about puzzle files
 * and other files on the user's storage device. The user can select a
 * file or open a directory with the mouse or keyboard.
 */
class FileSelectionPanel : public UIPanel {
public:
  FileSelectionPanel(SaveManager& save_mgr,
                     Font& filename_font, Font& info_font,
                     Texture& icons, const std::string& path = "");

  void open_path(const std::string& path);
  void open_file(const std::string& file);
  std::string selected_file() const;
  std::string path() const { return m_path; }
  void open_selection();

  /*
   * Register callback functions. A callback function receives a
   * string with the name of the file or directory that was selected
   * or opened.
   */
  typedef std::function<void(const std::string&)> Callback;
  void on_file_select(Callback fn);
  void on_file_open(Callback fn);
  void on_dir_change(Callback fn);

  using UIPanel::update;
  using UIPanel::draw;
  void update(unsigned ticks, InputHandler& input,
              const Rect& active_region) override;
  void draw(Renderer& renderer, const Rect& region) const override;

private:
  void draw_progress(Renderer& renderer, const PuzzleGrid& grid,
                     const Rect& area) const;
  void select(unsigned index);
  void make_selection_visible(const Rect& visible_region);
  unsigned entry_height() const;
  void load_file_list();
  void sort_files();
  void load_puzzle_info();

  struct FileInfo;
  static bool file_info_less_than(const FileInfo& l, const FileInfo& r);

  struct FileInfo {
    std::string filename;
    std::string full_path;
    enum class Type { directory, file, puzzle_file } type = Type::file;
    std::shared_ptr<PuzzleSummary> puzzle_info;
    std::shared_ptr<PuzzleProgress> puzzle_progress;
  };

  SaveManager& m_save_mgr;
  const Font& m_filename_font;
  const Font& m_info_font;
  const Texture& m_icon_texture;
  std::string m_path;
  std::vector<FileInfo> m_files;
  unsigned m_selection = 0;
  bool m_is_selected = false;
  unsigned m_num_puzzles_loaded = 0;

  Callback m_file_open_callback;
  Callback m_file_sel_callback;
  Callback m_dir_callback;
};

#endif

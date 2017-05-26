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

#ifndef NONNY_SAVE_MANAGER_HPP
#define NONNY_SAVE_MANAGER_HPP

#include <map>
#include <string>

class GameSettings;
class PuzzleProgress;

/*
 * Handles saving and loading of puzzle progress. Progress is saved
 * and loaded from a file whose filename and directory is based on the
 * name of the puzzle and the collection it belongs to, if any.
 */
class SaveManager {
public:
  explicit SaveManager(GameSettings& settings);
  SaveManager(const SaveManager&) = delete;
  SaveManager(SaveManager&&) = delete;

  SaveManager& operator=(const SaveManager&) = delete;
  SaveManager& operator=(SaveManager&&) = delete;

  void load_progress(PuzzleProgress& prog,
                     const std::string& path,
                     const std::string& collection,
                     const std::string& id) const;
  void save_progress(const PuzzleProgress& prog,
                     const std::string& path,
                     const std::string& collection,
                     const std::string& id) const;

private:
  std::string find_save_file(const std::string& path,
                             std::string collection,
                             std::string id) const;

  GameSettings& m_settings;
};

#endif

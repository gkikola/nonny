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

#include "save_manager.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <experimental/filesystem>
#include "puzzle/puzzle_progress.hpp"
#include "settings/game_settings.hpp"
#include "utility/utility.hpp"

namespace stdfs = std::experimental::filesystem;

std::istream& scan_for_path(std::istream& is, std::string& path);
std::string standardize(const std::string& name);

constexpr int max_id_size = 32;
constexpr int max_file_counter = 9999;

SaveManager::SaveManager(GameSettings& settings)
  : m_settings(settings)
{
}

void SaveManager::load_progress(PuzzleProgress& prog,
                                const std::string& path,
                                const std::string& collection,
                                const std::string& id) const
{
  std::string filename = find_save_file(path, collection, id);
  std::ifstream file(filename);

  if (file.is_open()) {
    file >> prog;
  } else {
    prog = PuzzleProgress(path);
  }
}

void SaveManager::save_progress(const PuzzleProgress& prog,
                                const std::string& path,
                                const std::string& collection,
                                const std::string& id) const
{
  std::string filename = find_save_file(path, collection, id);

  //make sure directory exists, create it if not
  stdfs::path p = stdfs::path(filename).remove_filename();
  if (!p.empty() && !stdfs::exists(p))
    stdfs::create_directories(p);
  
  std::ofstream file(filename);

  if (!file.is_open())
    throw std::runtime_error("SaveManager::save_progress: "
                             "could not save to file "
                             + filename);

  file << prog;
}

std::string SaveManager::find_save_file(const std::string& path,
                                        std::string collection,
                                        std::string id) const
{
  std::string std_dir = standardize(collection);
  std::string std_id = standardize(id);

  if (std_dir.empty())
    std_dir = "default";
  if (std_id.empty())
    std_id = "untitled";
  
  std::string expected_dir = m_settings.save_dir();
  expected_dir += std::string("saves") + m_settings.filesystem_separator();
  expected_dir += std_dir + m_settings.filesystem_separator();

  int counter = 0;
  while (counter < max_file_counter) {
    std::string full_name = expected_dir;
    if (counter == 0)
      full_name += std_id + ".nsv";
    else
      full_name += std_id + std::to_string(counter) + ".nsv";
    
    std::ifstream file(full_name);
    if (file.is_open()) {
      std::string name;
      scan_for_path(file, name);
      if (name == path)
        return full_name;
    } else
      return full_name;

    ++counter;
  }

  //too many files with the same name, reuse the first one
  return expected_dir + std_id + ".nsv";
}

std::istream& scan_for_path(std::istream& is, std::string& path)
{
  path = "";
  std::string line;
  while (std::getline(is, line)) {
    auto prop = parse_property(line);

    if (prop.first == "file") {
      path = prop.second;
      break;
    }
  }
  return is;
}

std::string standardize(const std::string& name)
{
  std::string result;
  for (auto c : name) {
    if (is_space(c))
      result += '_';
    else if (is_alpha(c) || is_digit(c))
      result += to_lower(c);
    else if (c == '&')
      result += "and";
    else if (c == '/' || c == '\\' || c == ':')
      result += '-';
  }

  if (result.size() > max_id_size)
    result = result.substr(0, max_id_size);

  return result;
}

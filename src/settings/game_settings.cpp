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

#include "settings/game_settings.hpp"

#include <fstream>
#include <experimental/filesystem>
#include "config.h"
#include "utility/utility.hpp"

namespace stdfs = std::experimental::filesystem;

GameSettings::GameSettings()
{
  find_directories();
}

void GameSettings::find_directories()
{
  m_separator = static_cast<char>(stdfs::path::preferred_separator);
  m_save_dir = save_path();

  stdfs::path base(base_path());

  //find data directory
  //look in current directory first
  m_data_dir = base.string();
  if (is_data_dir(m_data_dir))
    return;

  //could we be inside a build directory?
  m_data_dir = (base / stdfs::path("../data")).string();
  if (is_data_dir(m_data_dir))
    return;

  //one last check
  m_data_dir = (base / stdfs::path("../../data")).string();
  if (is_data_dir(m_data_dir))
    return;

  //none of the above, use installed directory
  m_data_dir = NONNY_DATADIR;
}

bool GameSettings::is_data_dir(const std::string& path)
{
  stdfs::path img_path = stdfs::path(path) / stdfs::path("images/nonny.png");
  std::ifstream file(img_path);
  return file.is_open();
}

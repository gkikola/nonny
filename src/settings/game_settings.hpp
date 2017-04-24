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

#ifndef NONNY_GAME_SETTINGS_HPP
#define NONNY_GAME_SETTINGS_HPP

#include <string>

class GameSettings {
public:
  GameSettings();
  
  const std::string& data_dir() const { return m_data_dir; }
  const std::string& save_dir() const { return m_save_dir; }
  char filesystem_separator() const { return m_separator; }

  inline std::string font_dir() const;
  inline std::string image_dir() const;
  inline std::string puzzle_dir() const;
private:
  void find_directories();
  bool has_config(const std::string& path);
  std::string m_data_dir;
  std::string m_save_dir;
  char m_separator = '/';
};


/* implementation */
inline std::string GameSettings::font_dir() const
{
  return data_dir() + "fonts" + filesystem_separator();
}

inline std::string GameSettings::image_dir() const
{
  return data_dir() + "images" + filesystem_separator();
}

inline std::string GameSettings::puzzle_dir() const
{
  return data_dir() + "puzzles" + filesystem_separator();
}

#endif

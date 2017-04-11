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

#ifndef NONNY_GAME_HPP
#define NONNY_GAME_HPP

#include <memory>
#include "video/renderer.hpp"
#include "video/video_system.hpp"
#include "video/window.hpp"

class Game {
public:
  Game(int argc, char* argv[]);

  void run();
private:
  std::unique_ptr<VideoSystem> m_video;
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Renderer> m_renderer;
};

#endif

cmake_minimum_required (VERSION 3.1)
project (Nonny)

set (NONNY_VERSION_MAJOR 0)
set (NONNY_VERSION_MINOR 01)
set (
  NONNY_VERSION
  "${NONNY_VERSION_MAJOR}.${NONNY_VERSION_MINOR}"
  )

if (WIN32)
  set (NONNY_DATADIR_SUFFIX "/nonny/")
else ()
  set (NONNY_DATADIR_SUFFIX "/share/nonny/")
endif ()

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

include (FindPkgConfig)
pkg_search_module (SDL2 REQUIRED sdl2>=2.0.1)
pkg_search_module (SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module (SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

include_directories (
  ${SDL2_INCLUDE_DIRS}
  ${SDL2IMAGE_INCLUDE_DIRS}
  ${SDL2TTF_INCLUDE_DIRS}
  )
include_directories ("src")
include_directories ("${PROJECT_BINARY_DIR}")

add_executable (
  nonny
  src/color/color.cpp
  src/color/color_palette.cpp
  src/event/sdl/sdl_event_handler.cpp
  src/event/event_handler.cpp
  src/input/sdl/sdl_input_handler.cpp
  src/input/input_handler.cpp
  src/input/key.cpp
  src/main/game.cpp
  src/main/main.cpp
  src/puzzle/puzzle.cpp
  src/puzzle/puzzle_cell.cpp
  src/puzzle/puzzle_clue.cpp
  src/puzzle/puzzle_grid.cpp
  src/puzzle/puzzle_io.cpp
  src/settings/game_settings.cpp
  src/ui/panel_collection.cpp
  src/ui/puzzle_info_panel.cpp
  src/ui/puzzle_panel.cpp
  src/ui/puzzle_preview.cpp
  src/ui/scrollbar.cpp
  src/ui/scrolling_panel.cpp
  src/ui/ui_panel.cpp
  src/utility/sdl/sdl_error.cpp
  src/utility/sdl/sdl_paths.cpp
  src/utility/utility.cpp
  src/video/sdl/sdl_font.cpp
  src/video/sdl/sdl_renderer.cpp
  src/video/sdl/sdl_texture.cpp
  src/video/sdl/sdl_video_system.cpp
  src/video/sdl/sdl_window.cpp
  src/video/font.cpp
  src/video/point.cpp
  src/video/rect.cpp
  src/video/renderer.cpp
  src/video/texture.cpp
  src/video/video_system.cpp
  src/video/window.cpp
  src/view/view.cpp
  src/view/view_manager.cpp
  src/view/puzzle_view.cpp
  )

target_link_libraries (
  nonny
  ${SDL2_LIBRARIES}
  ${SDL2IMAGE_LIBRARIES}
  ${SDL2TTF_LIBRARIES}
  )

if (WIN32)
  install (TARGETS nonny DESTINATION nonny)
  install (DIRECTORY data/ DESTINATION nonny)
else ()
  install (TARGETS nonny DESTINATION bin)
  install (DIRECTORY data/ DESTINATION share/nonny)
endif ()

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif ()

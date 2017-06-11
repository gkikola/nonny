cmake_minimum_required (VERSION 3.1)
project (Nonny)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

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

find_package (SDL2 REQUIRED)
find_package (SDL2_image REQUIRED)
find_package (SDL2_ttf REQUIRED)

include_directories (
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
  ${SDL2_TTF_INCLUDE_DIR}
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
  src/puzzle/compressed_state.cpp
  src/puzzle/puzzle.cpp
  src/puzzle/puzzle_cell.cpp
  src/puzzle/puzzle_clue.cpp
  src/puzzle/puzzle_grid.cpp
  src/puzzle/puzzle_io.cpp
  src/puzzle/puzzle_line.cpp
  src/puzzle/puzzle_progress.cpp
  src/puzzle/puzzle_summary.cpp
  src/save/save_manager.cpp
  src/settings/game_settings.cpp
  src/solver/block_sequence.cpp
  src/solver/line_solver.cpp
  src/solver/solver.cpp
  src/ui/analysis_panel.cpp
  src/ui/button.cpp
  src/ui/control.cpp
  src/ui/dialog.cpp
  src/ui/draw_tool_panel.cpp
  src/ui/file_selection_panel.cpp
  src/ui/image_button.cpp
  src/ui/menu.cpp
  src/ui/message_box.cpp
  src/ui/option_dialog.cpp
  src/ui/palette_panel.cpp
  src/ui/puzzle_info_panel.cpp
  src/ui/puzzle_panel.cpp
  src/ui/puzzle_preview.cpp
  src/ui/scrollbar.cpp
  src/ui/scrolling_panel.cpp
  src/ui/static_image.cpp
  src/ui/static_text.cpp
  src/ui/text_box.cpp
  src/ui/tooltip.cpp
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
  src/view/analyze_view.cpp
  src/view/data_edit_view.cpp
  src/view/file_view.cpp
  src/view/menu_view.cpp
  src/view/message_box_view.cpp
  src/view/puzzle_view.cpp
  src/view/victory_view.cpp
  src/view/view.cpp
  src/view/view_manager.cpp
  )

target_link_libraries (
  nonny
  stdc++fs
  ${SDL2_LIBRARY}
  ${SDL2_IMAGE_LIBRARIES}
  ${SDL2_TTF_LIBRARIES}
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

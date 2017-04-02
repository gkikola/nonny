cmake_minimum_required (VERSION 2.8)
project (Nonny)

set (NONNY_VERSION_MAJOR 0)
set (NONNY_VERSION_MINOR 01)
set (
  NONNY_VERSION
  "${NONNY_VERSION_MAJOR}.${NONNY_VERSION_MINOR}"
  )

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

include (FindPkgConfig)
pkg_search_module (SDL2 REQUIRED sdl2>=2.0.1)
pkg_search_module (SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module (SDL2TTF REQUIRED SDL2_ttf>=2.0.0)

include_directories (
  ${SDL2_INCLUDE_DIRS}
  ${SDL2IMAGE_INCLUDE_DIRS}
  ${SDL2TTF_INCLUDE_DIRS}
  )
include_directories ("${PROJECT_BINARY_DIR}")

add_executable (
  nonny
  src/application.cpp
  src/button.cpp
  src/collection_index.cpp
  src/control_container.cpp
  src/control.cpp
  src/game.cpp
  src/info_pane.cpp
  src/input_handler.cpp
  src/main.cpp
  src/menu.cpp
  src/oldpuzzle.cpp
  src/preview.cpp
  src/puzzle_collection.cpp
  src/renderer.cpp
  src/scrollbar.cpp
  src/static_text.cpp
  )

target_link_libraries (
  nonny
  ${SDL2_LIBRARIES}
  ${SDL2IMAGE_LIBRARIES}
  ${SDL2TTF_LIBRARIES}
  )

install (TARGETS nonny DESTINATION bin)
install (DIRECTORY data/ DESTINATION share/nonny)

#include "puzzle.h"

Puzzle::Puzzle(std::string filename) {
  grid_width = 10;
  grid_height = 10;

  grid.resize(grid_width * grid_height, CellState::blank);
}

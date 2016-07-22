#ifndef NONNY_PUZZLE_H
#define NONNY_PUZZLE_H

#include <string>
#include <vector>

enum class CellState { blank, marked, xedout };

class Puzzle {
public:
  Puzzle(std::string filename);

  int width() const { return grid_width; }
  int height() const { return grid_height; }

  CellState cell(int x, int y) { return grid[y * grid_width + x]; }
private:
  std::vector<CellState> grid;
  int grid_width;
  int grid_height;
};

#endif

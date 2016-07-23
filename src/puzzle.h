#ifndef NONNY_PUZZLE_H
#define NONNY_PUZZLE_H

#include <string>
#include <vector>

enum class CellState { blank, marked, xedout };

class Puzzle {
public:
  explicit Puzzle(const std::string& filename);

  int width() const { return grid_width; }
  int height() const { return grid_height; }

  CellState cell(int x, int y) const { return grid[y * grid_width + x]; }

  const std::vector<int>& get_row_rule(int row) const;
  const std::vector<int>& get_col_rule(int col) const;
private:
  void load_file(const std::string& filename);
  
  std::vector<CellState> grid;
  std::vector<std::vector<int>> row_rules;
  std::vector<std::vector<int>> col_rules;
  int grid_width;
  int grid_height;
};

#endif

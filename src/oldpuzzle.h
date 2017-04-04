#ifndef NONNY_OLDPUZZLE_H
#define NONNY_OLDPUZZLE_H

#include <string>
#include <vector>

enum class CellState { blank, marked, exedout };

struct Cell {
  CellState state;
  CellState prev_state;
  int age;
};
  
struct RuleEntry {
  int value;
  bool completed;
  bool hint;
  bool error;
};

class OldPuzzle {
public:
  explicit OldPuzzle(const std::string& filename);

  int width() const { return m_width; }
  int height() const { return m_height; }

  CellState cell(int x, int y) const { return m_grid[y * m_width + x].state; }
  CellState prev_cell_state(int x, int y) const;
  int cell_age(int x, int y) const { return m_grid[y * m_width + x].age; }
  void set_cell(int x, int y, CellState state);
  void age_cell(int x, int y, int max);

  const std::string& title() const { return m_title; }
  const std::string& author() const { return m_author; }
  
  const std::vector<RuleEntry>& get_row_rule(int row) const;
  const std::vector<RuleEntry>& get_col_rule(int col) const;
private:
  void load_file(const std::string& filename);
  void unload_puzzle();
  
  std::vector<Cell> m_grid;
  std::vector<std::vector<RuleEntry>> m_row_rules;
  std::vector<std::vector<RuleEntry>> m_col_rules;
  int m_width;
  int m_height;

  std::string m_title;
  std::string m_author;
};

#endif
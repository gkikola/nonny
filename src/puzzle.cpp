#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

#include "puzzle.h"

Puzzle::Puzzle(const std::string& filename) : m_width{0}, m_height{0} {
  load_file(filename);
}

const std::vector<RuleEntry>& Puzzle::get_row_rule(int row) const {
  if (row >= m_row_rules.size() || row < 0)
    throw std::runtime_error("Puzzle::get_row_rule: invalid row");

  return m_row_rules[row];
}

const std::vector<RuleEntry>& Puzzle::get_col_rule(int col) const {
  if (col >= m_col_rules.size() || col < 0)
    throw std::runtime_error("Puzzle::get_col_rule: invalid column");

  return m_col_rules[col];
}

CellState Puzzle::prev_cell_state(int x, int y) const {
  return m_grid[y * width() + x].prev_state;
}

void Puzzle::set_cell(int x, int y, CellState state) {
  if (x >= 0 && y >= 0 && x < width() && y < height()) {
    int index = y * width() + x;

    m_grid[index].prev_state = m_grid[index].state;
    m_grid[index].state = state;
    m_grid[index].age = 0;
  }
}

void Puzzle::age_cell(int x, int y, int max) {
  if (cell_age(x, y) < max)
    m_grid[y * width() + x].age++;
}

void Puzzle::load_file(const std::string& filename) {
  std::string line;
  std::ifstream file(filename);

  if (!file.is_open()) {
    throw std::runtime_error("Puzzle::load_file: could not open file");
  }

  bool reading_rows = true;

  while (getline(file, line)) {
    std::istringstream ss(line);
    std::string property;

    ss >> property;
    transform(property.begin(), property.end(), property.begin(), tolower);

    if (property.length() > 0) {
      if (isdigit(property[0])) { //current line is rule data
        ss.seekg(0);

        if (reading_rows) {
          std::vector<RuleEntry> rule;
          m_row_rules.push_back(std::move(rule));
        }
        else {
          std::vector<RuleEntry> rule;
          m_col_rules.push_back(std::move(rule));
        }

        char ch;
        while ((ch = ss.peek()) != EOF) {
          if (ch == ' ' || ch == ',' || ch == '\t')
            ss.get(); //eat the character
          else { //read rule data
            RuleEntry entry;
            ss >> entry.value;
            entry.completed = false;
            entry.hint = false;
            entry.error = false;

            if (reading_rows)
              m_row_rules[m_row_rules.size() - 1].push_back(entry);
            else
              m_col_rules[m_col_rules.size() - 1].push_back(entry);
          }
        }
      }
      else if (property == "width")
        ss >> m_width;
      else if (property == "height")
        ss >> m_height;
      else if (property == "rows") {
        reading_rows = true;
      } else if (property == "columns") {
        reading_rows = false;
      }
    }
  }

  Cell blank;
  blank.state = CellState::blank;
  blank.prev_state = CellState::blank;
  blank.age = 0;
  
  m_grid.resize(m_width * m_height, blank);

  file.close();
}

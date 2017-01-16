#ifndef NONNY_PUZZLE_COLLECTION_H
#define NONNY_PUZZLE_COLLECTION_H

#include <string>
#include <vector>

class Puzzle;

class PuzzleCollection {
 public:
  PuzzleCollection(const std::string& path);
  ~PuzzleCollection();
 private:
  std::string m_title;
  std::string m_path;
  std::vector<Puzzle*> m_puzzles;
};

#endif

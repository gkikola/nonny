#ifndef NONNY_PUZZLE_COLLECTION_H
#define NONNY_PUZZLE_COLLECTION_H

#include <string>
#include <vector>

struct PuzzleEntry {
  std::string title;
  std::string path;
  int width;
  int height;
};

class Puzzle;

class PuzzleCollection {
 public:
  PuzzleCollection(const std::string& path);
  ~PuzzleCollection();

  typedef std::vector<PuzzleEntry*>::iterator iterator;
  typedef std::vector<PuzzleEntry*>::const_iterator const_iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
 private:
  void read_puzzle(const std::string& path);
  std::string m_title;
  std::string m_path;
  std::vector<PuzzleEntry*> m_puzzles;
};

const std::string info_file = "collection.nco";

#endif

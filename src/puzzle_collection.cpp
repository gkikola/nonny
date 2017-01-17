#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "puzzle.h"

#include "puzzle_collection.h"

PuzzleCollection::PuzzleCollection(const std::string& path)
  : m_path{path} {
  char sep = boost::filesystem::path::preferred_separator;

  //see if a collection file exists
  std::string filename = path + sep + info_file;
  std::ifstream file(filename);

  if (file.is_open()) {
    std::string line;
    getline(file, line); //collection title, ignore
    while (getline(file, line))
      read_puzzle(path + sep + line);
  } else {
    //no collection file, just read all files in the directory
    for (auto& puzzle_file : boost::filesystem::directory_iterator(path)) {
      auto f = puzzle_file.path();

      if (boost::filesystem::is_regular_file(f)
          && f.extension().string() == ".non")
        read_puzzle(f.string());
    }
  }
}

PuzzleCollection::~PuzzleCollection() {
  for (PuzzleEntry* entry : m_puzzles) {
    delete entry;
  }
}

PuzzleCollection::iterator PuzzleCollection::begin() {
  return m_puzzles.begin();
}

PuzzleCollection::iterator PuzzleCollection::end() {
  return m_puzzles.end();
}

PuzzleCollection::const_iterator PuzzleCollection::begin() const {
  return m_puzzles.begin();
}

PuzzleCollection::const_iterator PuzzleCollection::end() const {
  return m_puzzles.end();
}

void PuzzleCollection::read_puzzle(const std::string& path) {
  Puzzle puzzle(path);
  PuzzleEntry *entry = new PuzzleEntry;

  if (puzzle.title().length() > 0)
    entry->title = puzzle.title();
  else
    entry->title = "Untitled";

  entry->width = puzzle.width();
  entry->height = puzzle.height();
  entry->path = path;

  m_puzzles.push_back(entry);
}

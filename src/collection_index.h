#ifndef NONNY_COLLECTION_INDEX_H
#define NONNY_COLLECTION_INDEX_H

#include <string>
#include <vector>
#include "puzzle_collection.h"

struct CollectionEntry {
  std::string path;
  std::string title;
};

class CollectionIndex {
 public:
  CollectionIndex(const std::string& path, const std::string& index_file);
  ~CollectionIndex();

  typedef std::vector<CollectionEntry*>::iterator iterator;
  typedef std::vector<CollectionEntry*>::const_iterator const_iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  
  PuzzleCollection* open_collection(const CollectionEntry* collection) const;
 private:
  void read_collection(const std::string& path);
  
  std::string m_path;
  std::vector<CollectionEntry*> m_collections;
};

#endif

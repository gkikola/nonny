#ifndef NONNY_COLLECTION_INDEX_H
#define NONNY_COLLECTION_INDEX_H

#include <string>

class CollectionIndex {
 public:
  CollectionIndex(const std::string& path);
  ~CollectionIndex();
 private:
  std::string m_path;
};

#endif

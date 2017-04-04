#include <fstream>
#include <string>
#include <vector>
#include "puzzle_collection.h"

#include "collection_index.h"

CollectionIndex::CollectionIndex(const std::string& path,
                                 const std::string& index_file)
  : m_path{path} {
  char sep = '/';//TODO REPLACE: boost::filesystem::path::preferred_separator;
  
  //see if an index file exists
  std::string filename = path + sep + index_file;
  std::ifstream file(filename);

  if (file.is_open()) {
    std::string line;
    while (getline(file, line))
      read_collection(path + sep + line);
  } else {
    /*
    //no index file, just enumerate each directory
    for (auto& dir : boost::filesystem::directory_iterator(path)) {
      auto p = dir.path();

      if (boost::filesystem::is_directory(p))
        read_collection(p.string());
        }*/
    throw std::runtime_error("unimplemented");//TODO REPLACE
  }
}

CollectionIndex::~CollectionIndex() {
  for (CollectionEntry* entry : m_collections) {
    delete entry;
  }
}

CollectionIndex::iterator CollectionIndex::begin() {
  return m_collections.begin();
}

CollectionIndex::iterator CollectionIndex::end() {
  return m_collections.end();
}

CollectionIndex::const_iterator CollectionIndex::begin() const {
  return m_collections.begin();
}

CollectionIndex::const_iterator CollectionIndex::end() const {
  return m_collections.end();
}

void CollectionIndex::read_collection(const std::string& path) {
  //  std::string filename = path
  //  + boost::filesystem::path::preferred_separator + info_file;
  std::string filename = path + '/' + info_file; //TODO REPLACE

  std::ifstream file(filename);

  if (file.is_open()) {
    //try to pull title information
    std::string line;
    CollectionEntry* entry = new CollectionEntry;
    
    getline(file, line);
    if (line.length() > 0)
      entry->title = line;
    else {
      //boost::filesystem::path p(path);
      //entry->title = p.filename().string();
      entry->title = "Unnamed"; //TODO REPLACE
    }

    entry->path = path;
    m_collections.push_back(entry);
  } else { //no collection information
    //boost::filesystem::path p(path);
    CollectionEntry* entry = new CollectionEntry;
    entry->path = path;
    entry->title = "Unnamed"; //TODO REPLACE: p.filename().string();
    m_collections.push_back(entry);
  }
}

PuzzleCollection*
CollectionIndex::open_collection(const std::string& collection) const {
  return new PuzzleCollection(collection);
}
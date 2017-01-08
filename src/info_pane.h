#ifndef NONNY_INFO_PANE_H
#define NONNY_INFO_PANE_H

#include <vector>
#include "renderer.h"

#include "control.h"
#include "preview.h"

class Game;

class InfoPane {
 public:
  InfoPane(Game* game);
  ~InfoPane();

  void set_width(int width) { m_width = width; }
  int width() const { return m_width; }

  void update();

  void position_controls();

  typedef std::vector<Control*>::const_iterator const_iterator;
  const_iterator begin() const { return m_controls.begin(); }
  const_iterator end() const { return m_controls.end(); }
 private:
  Game* m_game;

  std::vector<Control*> m_controls;
  Preview* m_preview;

  int m_width;
};

#endif

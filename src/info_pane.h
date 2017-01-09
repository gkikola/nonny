#ifndef NONNY_INFO_PANE_H
#define NONNY_INFO_PANE_H

#include <vector>
#include "control.h"
#include "control_container.h"
#include "preview.h"
#include "renderer.h"

class Game;

class InfoPane : public ControlContainer {
 public:
  explicit InfoPane(Game* game);

  void set_width(int width) { m_width = width; }
  int width() const { return m_width; }

  void get_preview_position(int* x, int* y) const;
  void get_preview_size(int* width, int* height) const;
  
  void setup_controls();
 private:
  Game* m_game;

  Preview* m_preview;
  StaticText* m_title;
  StaticText* m_size;
  StaticText* m_author;

  Button* m_menu;
  Button* m_hints;
  Button* m_errors;

  int m_width;
};

#endif

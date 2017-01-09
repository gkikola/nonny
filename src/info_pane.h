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

  void get_preview_position(int* x, int* y) const;
  void get_preview_size(int* width, int* height) const;
  
  void update();
  void draw(Renderer* renderer) const;

  void setup_controls();
 private:
  Game* m_game;

  Preview* m_preview;
  StaticText* m_title;
  StaticText* m_size;
  StaticText* m_author;

  Button* m_menu;

  int m_width;
};

#endif

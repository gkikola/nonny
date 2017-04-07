#ifndef NONNY_MENU_H
#define NONNY_MENU_H

#include "control_container.h"

class Game;

class Menu : public ControlContainer {
 public:
  explicit Menu(Game* game);

  void add_control(Control* control);
  void arrange_controls(int screen_width, int screen_height, int columns = 1);
 private:
  Game* m_game;
};

#endif

#ifndef NONNY_MENU_H
#define NONNY_MENU_H

#include "control_container.h"

class Game;

class Menu : public ControlContainer {
 public:
  explicit Menu(Game* game);

  void add_control(Control* control);
 private:
  Game* m_game;
};

#endif

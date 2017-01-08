#ifndef NONNY_INFO_PANE_H
#define NONNY_INFO_PANE_H

class Game;

class InfoPane {
 public:
  InfoPane(Game* game) : m_game{game}, m_width{0} {}

  void set_width(int width) { m_width = width; }
  int width() const { return m_width; }
 private:
  Game* m_game;

  int m_width;
};

#endif

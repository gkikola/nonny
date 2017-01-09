#ifndef NONNY_CONTROL_H
#define NONNY_CONTROL_H

class Game;
class Renderer;

class Control {
 public:
  explicit Control(Game* game);

  void get_position(int* x, int* y) const;
  void get_size(int* width, int* height) const;

  virtual void move(int x, int y);
  virtual void resize(int width, int height);

  virtual void draw(Renderer* renderer) const = 0;
 protected:
  Game* m_game;

 private:
  int m_x, m_y;
  int m_width, m_height;
};

#endif

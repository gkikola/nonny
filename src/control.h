#ifndef NONNY_CONTROL_H
#define NONNY_CONTROL_H

class Control {
 public:
  Control();

  void get_position(int* x, int* y) const;
  void get_size(int* width, int* height) const;

  void move(int x, int y);
  void resize(int width, int height);
 private:
  int m_x, m_y;
  int m_width, m_height;
};

#endif

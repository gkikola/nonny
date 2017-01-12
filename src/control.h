#ifndef NONNY_CONTROL_H
#define NONNY_CONTROL_H

class Game;
class Renderer;

class Control {
 public:
  explicit Control(Game* game);

  void get_position(int* x, int* y) const;
  void get_size(int* width, int* height) const;

  void hover_mouse(bool hovering = true);
  bool is_mouse_hovering() const { return m_hover; }

  void select();
  void deselect();
  bool is_selected() const { return m_selected; }

  void depress();
  void unpress();
  bool is_depressed() const { return m_depressed; }

  virtual void move(int x, int y);
  virtual void resize(int width, int height);

  virtual void activate() = 0;

  virtual void draw(Renderer* renderer) const = 0;
  virtual bool is_selectable() const { return true; }

  enum class MouseAction { left, middle, right };
  enum class KeyAction { activate, depress, next_ctrl };

  virtual void key_press(KeyAction action, bool down);
  virtual void mouse_press(MouseAction action, int x, int y, bool down);
  virtual void mouse_move(int x, int y);
 protected:
  Game* m_game;

 private:
  int m_x, m_y;
  int m_width, m_height;
  bool m_hover;
  bool m_selected;
  bool m_depressed;
};

#endif

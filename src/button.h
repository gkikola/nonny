#ifndef NONNY_BUTTON_H
#define NONNY_BUTTON_H

#include "control.h"

class Game;

class Button : public Control {
 public:
  Button(Game* game);

  void hover_mouse(bool hovering = true);
  bool is_mouse_hovering() const { return m_hover; }

  void select();
  void deselect();
  bool is_selected() const { return m_selected; }

  const std::string& label() const { return m_label; }
  void set_label(const std::string& label);

  typedef void (*callback)(Game*);
  void register_callback(callback function);
  void activate();

  virtual void update() override;
  virtual void draw(Renderer* renderer) const override;
 private:
  std::string m_label;
  bool m_hover;
  bool m_selected;
  callback m_callback_func;
};

#endif

#ifndef NONNY_BUTTON_H
#define NONNY_BUTTON_H

#include "control.h"

class Game;

class Button : public Control {
 public:
  Button(Game* game);

  const std::string& label() const { return m_label; }
  void set_label(const std::string& label);

  typedef void (*callback)(Game*);
  void register_callback(callback function);

  virtual void activate() override;
  virtual void draw(Renderer* renderer) const override;
 private:
  std::string m_label;
  callback m_callback_func;
};

#endif

#ifndef NONNY_BUTTON_H
#define NONNY_BUTTON_H

#include <string>
#include "control.h"

class Game;

class Button : public Control {
 public:
  Button(Game* game);

  const std::string& label() const { return m_label; }
  void set_label(const std::string& label);

  const std::string& data() const { return m_data; }
  void set_data(const std::string& data);

  typedef void (*callback)(Game*, const std::string&);
  void register_callback(callback function);

  virtual void activate() override;
  virtual void draw(Renderer* renderer) const override;
 private:
  std::string m_label;
  std::string m_data;
  callback m_callback_func;
};

#endif

#ifndef NONNY_STATIC_TEXT_H
#define NONNY_STATIC_TEXT_H

#include <string>

#include "control.h"

class Renderer;

class StaticText : public Control {
 public:
  explicit StaticText(Game* game);

  virtual void draw(Renderer* renderer) const override;
  virtual void activate() override {}

  virtual bool is_selectable() const override { return false; }
  
  void set_string(const std::string& str);
  void get_string(std::string* str) const;

  enum class Type { standard, heading, small, huge };

  void set_type(Type type) { m_type = type; }
  Type type() const { return m_type; }
 private:
  std::string m_string;
  Type m_type;
};

#endif

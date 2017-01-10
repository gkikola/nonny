#ifndef NONNY_CONTROL_CONTAINER_H
#define NONNY_CONTROL_CONTAINER_H

#include <list>

class Control;
class Renderer;

class ControlContainer {
 public:
  virtual ~ControlContainer();
  
  typedef std::list<Control*>::iterator iterator;

  iterator begin();
  iterator end();

  int size() const { return m_controls.size(); }

  void select_next(bool backwards = false);
  Control* selection();

  virtual void draw(Renderer* renderer) const;
  virtual void clear();
 protected:
  void add_control(Control* control);
 private:
  typedef std::list<Control*> ControlList;

  ControlList m_controls;
};

#endif

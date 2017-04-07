#ifndef NONNY_CONTROL_CONTAINER_H
#define NONNY_CONTROL_CONTAINER_H

#include <list>

class Control;
class Renderer;

class ControlContainer {
 public:
  virtual ~ControlContainer();
  
  typedef std::list<Control*>::iterator iterator;
  typedef std::list<Control*>::const_iterator const_iterator;

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  int size() const { return m_controls.size(); }

  void select_next(bool backwards = false);
  Control* selection();
  void clear_selection();

  virtual void draw(Renderer* renderer) const;
  virtual void clear();
 protected:
  void add_control(Control* control);
 private:
  typedef std::list<Control*> ControlList;

  ControlList m_controls;
};

#endif

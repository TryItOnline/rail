// NilVar.h

#ifndef NIL_VAR_H_RAIL_1
#define NIL_VAR_H_RAIL_1

#include "Var.h"

class NilVar : public Var
{
public:
  NilVar() {}
  virtual ~NilVar() {}

  virtual StringVar * getString(void)
  {
    return NULL;
  }

  virtual ListVar * getList(void)
  {
    return NULL;
  }

  virtual NilVar * getNil(void)
  {
    return this;
  }

  virtual Closure * getClosure(void)
  {
    return NULL;
  }

  virtual std::string toString(void) const
  {
    return "<NIL>";
  }
private:
  NilVar(NilVar const &) {}
  NilVar & operator=(NilVar const &) { return *this; }
};

#endif

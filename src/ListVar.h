// ListVar.h

#ifndef LIST_VAR_H_RAIL_1
#define LIST_VAR_H_RAIL_1

#include "Var.h"
#include "NilVar.h"

struct ConsCell
{
  ConsCell(Binding newCar = NIL, Binding newCdr = NIL)
    : car(newCar)
    , cdr(newCdr)
  {
  }
  Binding car;
  Binding cdr;
};

class ListVar : public Var
{
public:
  ListVar()
    : car(NIL)
    , cdr(NIL) {}
  ListVar(Binding newCar, Binding newCdr)
    : car(newCar), cdr(newCdr) {}
  virtual ~ListVar() {}

  virtual StringVar * getString(void)
  {
    return NULL;
  }

  virtual ListVar * getList(void)
  {
    return this;
  }

  virtual NilVar * getNil(void)
  {
    return NULL;
  }

  virtual Closure * getClosure(void)
  {
    return NULL;
  }

  virtual std::string toString(void) const
  {
    return "(cons " + car->toString() + " " + cdr->toString() + ")";
  }

  ConsCell getCell(void) const
  {
    return ConsCell(car, cdr);
  }

  Binding getCar(void) const
  {
    return car;
  }

  Binding getCdr(void) const
  {
    return cdr;
  }
private:
  ListVar(ListVar const &) {}
  ListVar & operator=(ListVar const &) { return *this; }
private:
  Binding car;
  Binding cdr;
};

#endif

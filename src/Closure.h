// Closure.h

#ifndef CLOSURE_H_RAIL_1
#define CLOSURE_H_RAIL_1

#include "Binding.h"
#include "Vec.h"
#include "Dir.h"
#include "Var.h"
#include "Board.h"

class Closure : public Var
{
public:
  Closure(std::map<std::string, Binding> const & newEnv,
          Board const & newFunc,
          Vec newPos,
          Dir::t newDir)
    : environment(newEnv)
    , function(newFunc)
    , position(newPos)
    , direction(newDir) {}
  virtual ~Closure() {}

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
    return NULL;
  }

  virtual Closure * getClosure(void)
  {
    return this;
  }

  virtual std::string toString(void) const
  {
    return "(closure '" + function.getName() + "')";
  }

  std::map<std::string, Binding> const & getEnvironment(void) const
  {
    return environment;
  }

  Board const & getFunction(void) const
  {
    return function;
  }

  Vec getPosition(void) const
  {
    return position;
  }

  Dir::t getDirection(void) const
  {
    return direction;
  }
private:
  Closure();
  Closure(Closure const &);
  Closure & operator=(Closure const &) { return *this; }
private:
  std::map<std::string, Binding> environment;
  Board const & function;
  Vec position;
  Dir::t direction;
};

#endif

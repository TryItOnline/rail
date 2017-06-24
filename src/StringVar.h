// StringVar.h

#ifndef STRING_VAR_H_RAIL_1
#define STRING_VAR_H_RAIL_1

#include "Var.h"

class StringVar : public Var
{
public:
  StringVar() {}
  StringVar(std::string const & newData) : data(newData) {}
  virtual ~StringVar() {}

  virtual StringVar * getString(void)
  {
    return this;
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
    return NULL;
  }

  virtual std::string toString(void) const
  {
    return "\"" + data + "\"";
  }

  std::string const & get(void) const
  {
    return data;
  }
private:
  StringVar(StringVar const &) {}
  StringVar & operator=(StringVar const &) { return *this; }
private:
  std::string data;
};

#endif

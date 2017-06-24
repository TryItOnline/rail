// Var.h

// This is the base class for all variables in rail. The
// acquire/release is for reference counting, which we will use for
// our GC implementation. We can use reference counting here because
// the only primitives are cons (':'), lambda ('&'), and stack
// operations. No modifications of cons cells or closures are allowed.

// The simple tag-based polymorphism here is used because all of the
// actions work only on specific well-defined types. Thus we want
// to bypass the complex multiple-dispatch pattern.

#ifndef VAR_H_RAIL_1
#define VAR_H_RAIL_1

class StringVar;
class ListVar;
class NilVar;
class Closure;

class Var
{
public:
  Var() : refCount(0) {}
  virtual ~Var() {}

  void acquire(void)
  {
    ++refCount;
  }

  void release(void)
  {
    --refCount;
    if (refCount <= 0)
    {
      delete this;
    }
  }

  virtual StringVar * getString(void)=0;
  virtual ListVar * getList(void)=0;
  virtual NilVar * getNil(void)=0;
  virtual Closure * getClosure(void)=0;

  virtual std::string toString(void) const=0;
private:
  Var(Var const &) {}
  Var & operator=(Var const &) { return *this; }
private:
  int refCount;
};

#endif

// Binding.h

#ifndef BINDING_H_RAIL_1
#define BINDING_H_RAIL_1

class Var;

class Binding
{
public:
  Binding();
  explicit Binding(Var * newData);
  ~Binding();

  Binding(Binding const & right);
  Binding & operator=(Binding const & right);

  Var * operator->(void);
  Var const * operator->(void) const;
  Var & operator*(void);
  Var const & operator*(void) const;

  bool null(void);
private:
  Var * data;
};

#endif

// Binding.cpp

#include "lib.h"
#include "Var.h"
#include "Binding.h"

using namespace std;

Binding::Binding()
  : data(NULL)
{
}

Binding::Binding(Var * newData)
{
  data = newData;
  if (!null())
  {
    data->acquire();
  }
}

Binding::~Binding()
{
  if (!null())
  {
    data->release();
  }
}

Binding::Binding(Binding const & right)
{
  data = right.data;
  if (!null())
  {
    data->acquire();
  }
}

Binding & Binding::operator=(Binding const & right)
{
  Binding temp(right);
  swap(data, temp.data);
  return *this;
}

Var * Binding::operator->(void)
{
  return data;
}

Var const * Binding::operator->(void) const
{
	return data;
}

Var & Binding::operator*(void)
{
  return *data;
}

Var const & Binding::operator*(void) const
{
	return*data;
}

bool Binding::null(void)
{
  return data == NULL;
}

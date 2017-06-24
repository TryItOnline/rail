// lib.cpp

#include "lib.h"
#include "Binding.h"
#include "NilVar.h"

using namespace std;

Binding NIL(new NilVar());

string intToString(int num)
{
  ostringstream stream;
  stream << num;
  return stream.str();
}

namespace args
{
  bool isBatch = true;
  ostream * output = NULL;
  istream * input = NULL;
  list<string> sourceFiles;
}

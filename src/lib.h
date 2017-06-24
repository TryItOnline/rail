// lib.h

#ifndef LIB_H_RAIL_1
#define LIB_H_RAIL_1

#include <vector>
#include <list>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>

class InternalException : public std::exception
{
public:
  explicit InternalException(std::string const & newLabel)
    : label(newLabel)
  {
  }

  virtual ~InternalException() throw() {}

  virtual char const * what() const throw()
  {
    return label.c_str();
  }
private:
  std::string label;
};

class ArgumentException : public std::exception
{
public:
  explicit ArgumentException(std::string const & newLabel)
    : label(newLabel)
  {
  }

  virtual ~ArgumentException() throw() {}

  virtual char const * what() const throw()
  {
    return label.c_str();
  }
private:
  std::string label;
};

std::string intToString(int num);

class Binding;

extern Binding NIL;

namespace args
{
  extern bool isBatch;
  extern std::ostream * output;
  extern std::istream * input;
  extern std::list<std::string> sourceFiles;
}

#endif

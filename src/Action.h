// Action.h

#ifndef ACTION_H_RAIL_1
#define ACTION_H_RAIL_1

#include "Binding.h"
#include "Dir.h"
#include "NilVar.h"
#include "StringVar.h"
#include "ListVar.h"
#include "Closure.h"
#include "Board.h"
#include "ActivationRecord.h"
#include "Error.h"

class Action
{
public:
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> & globals)=0;
public:
  // Helper functions. These are called from individual actions to do
  // a common task.
  static Action * charToAction(char current);
  static void checkVarCount(std::list<Binding> const & theStack, int count);
  static bool checkBool(Binding var);
  static int checkNumeric(Binding var);
  static ConsCell checkList(Binding var);
  static std::string checkString(Binding var);

  static bool areEqual(Binding left, Binding right);

  static void runY(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   Dir::t first, Dir::t second, Dir::t third);
  static void callFunctionByName(std::string const & name,
                                 std::list<ActivationRecord> & programStack,
                                 std::map<std::string, Board> const & globals);
};

class Nop : public Action
{
public:
  static Nop v;
  virtual void run(std::list<Binding> &, std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
  }
};

class True : public Action
{
public:
  static True v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    dataStack.push_front(Binding(new StringVar("1")));
  }
};

class False : public Action
{
public:
  static False v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    dataStack.push_front(Binding(new StringVar("0")));
  }
};

// The direction before the y junction is the direction which the
// 'arrow' points to. So '>' is an EastY.

// This is the '>' junction.
class EastY : public Action
{
public:
  static EastY v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> &)
  {
    runY(dataStack, programStack, Dir::west, Dir::southeast, Dir::northeast);
  }
};

// This is the '<' junction.
class WestY : public Action
{
public:
  static WestY v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> &)
  {
    runY(dataStack, programStack, Dir::east, Dir::northwest, Dir::southwest);
  }
};

// This is the '^' junction.
class NorthY : public Action
{
public:
  static NorthY v;
  virtual void run(std::list<Binding> & dataStack,
                    std::list<ActivationRecord> & programStack,
                    std::map<std::string, Board> &)
  {
    runY(dataStack, programStack, Dir::south, Dir::northeast, Dir::northwest);
  }
};

// This is the 'v' junction.
class SouthY : public Action
{
public:
  static SouthY v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> &)
  {
    runY(dataStack, programStack, Dir::north, Dir::southwest, Dir::southeast);
  }
};

class Boom : public Action
{
public:
  static Boom v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 1);
    std::string custom = checkString(dataStack.front());
    throw CrashException(Error::custom, custom);
  }
};

class EofCheck : public Action
{
public:
  static EofCheck v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> & globals)
  {
    if (args::input->peek() == EOF)
    {
      True::v.run(dataStack, programStack, globals);
    }
    else
    {
      False::v.run(dataStack, programStack, globals);
    }
  }
};

class Input : public Action
{
public:
  static Input v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    int val = args::input->get();
    if (val == EOF)
    {
      throw CrashException(Error::noMoreInput);
    }
    else
    {
      dataStack.push_front(Binding(new StringVar(static_cast<char>(val)
                                                        + std::string())));
    }
  }
};

class Output : public Action
{
public:
  static Output v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 1);
    std::string val = checkString(dataStack.front());
    dataStack.pop_front();
    *(args::output) << val;
  }
};

class Underflow : public Action
{
public:
  static Underflow v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    std::string count = intToString(static_cast<int>(dataStack.size()));
    dataStack.push_front(Binding(new StringVar(count)));
  }
};

class TypeP : public Action
{
public:
  static TypeP v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 1);
    Binding top = dataStack.front();
    dataStack.pop_front();
    std::string type;
    if (top->getString() != NULL)
    {
      type = "string";
    }
    else if (top->getList() != NULL)
    {
      type = "list";
    }
    else if (top->getNil() != NULL)
    {
      type = "nil";
    }
    else if (top->getClosure() != NULL)
    {
      type = "lambda";
    }
    else
    {
      throw InternalException("TypeP::run(): Unknown type");
    }
    dataStack.push_front(Binding(new StringVar(type)));
  }
};

class PushConstant : public Action
{
public:
  static PushConstant v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    // Our job is done. The system pushed the string in delimiters
    // onto the stack. That is just what we want to do.
    checkVarCount(dataStack, 1);
    checkString(dataStack.front());
  }
};

class UseVariable : public Action
{
public:
  static UseVariable v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 1);
    std::string varName = checkString(dataStack.front());
    dataStack.pop_front();
    std::map<std::string, Binding>::const_iterator pos
                           = programStack.front().getEnvironment().find(varName);
    if (pos != programStack.front().getEnvironment().end())
    {
      dataStack.push_front(pos->second);
    }
    else
    {
      throw CrashException(Error::localBindingNotFound);
    }
  }
};

class BindVariable : public Action
{
public:
  static BindVariable v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    std::string varName = checkString(dataStack.front());
    dataStack.pop_front();
    programStack.front().bindVariable(varName, dataStack.front());
    dataStack.pop_front();
  }
};

class CallFunction : public Action
{
public:
  static CallFunction v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> & globals)
  {
    checkVarCount(dataStack, 1);
    std::string functionName = checkString(dataStack.front());
    dataStack.pop_front();
    if (functionName != "")
    {
      callFunctionByName(functionName, programStack, globals);
    }
    else
    {
      Binding function = dataStack.front();
      if (function->getString() != NULL)
      {
        callFunctionByName(function->getString()->get(), programStack,
                           globals);
      }
      else if (function->getClosure() != NULL)
      {
        programStack.push_front(ActivationRecord(*(function->getClosure())));
      }
      else
      {
        throw CrashException(Error::typeMismatch);
      }
      dataStack.pop_front();
    }
  }
};

class Add : public Action
{
public:
  static Add v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    int right = checkNumeric(dataStack.front());
    dataStack.pop_front();
    int left = checkNumeric(dataStack.front());
    dataStack.pop_front();
    dataStack.push_front(Binding(new StringVar(intToString(left + right))));
  }
};

class Divide : public Action
{
public:
  static Divide v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    int right = checkNumeric(dataStack.front());
    dataStack.pop_front();
    int left = checkNumeric(dataStack.front());
    dataStack.pop_front();
    dataStack.push_front(Binding(new StringVar(intToString(left / right))));
  }
};

class Multiply : public Action
{
public:
  static Multiply v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    int right = checkNumeric(dataStack.front());
    dataStack.pop_front();
    int left = checkNumeric(dataStack.front());
    dataStack.pop_front();
    dataStack.push_front(Binding(new StringVar(intToString(left * right))));
  }
};

class Remainder : public Action
{
public:
  static Remainder v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    int right = checkNumeric(dataStack.front());
    dataStack.pop_front();
    int left = checkNumeric(dataStack.front());
    dataStack.pop_front();
    dataStack.push_front(Binding(new StringVar(intToString(left % right))));
  }
};

class Subtract : public Action
{
public:
  static Subtract v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    int right = checkNumeric(dataStack.front());
    dataStack.pop_front();
    int left = checkNumeric(dataStack.front());
    dataStack.pop_front();
    dataStack.push_front(Binding(new StringVar(intToString(left - right))));
  }
};

class Digit : public Action
{
public:
  static Digit v0;
  static Digit v1;
  static Digit v2;
  static Digit v3;
  static Digit v4;
  static Digit v5;
  static Digit v6;
  static Digit v7;
  static Digit v8;
  static Digit v9;
  Digit(int newNum = 0) : num(newNum) {}
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    dataStack.push_front(Binding(new StringVar(intToString(num))));
  }
private:
  int num;
};

class Cut : public Action
{
public:
  static Cut v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    int index = checkNumeric(dataStack.front());
    dataStack.pop_front();
    std::string str = checkString(dataStack.front());
    dataStack.pop_front();
    if (index < 0 || index > static_cast<int>(str.size()))
    {
      throw CrashException(Error::indexOutOfBounds);
    }
    std::string leftResult = str.substr(0, index);
    std::string rightResult = str.substr(index);
    dataStack.push_front(Binding(new StringVar(leftResult)));
    dataStack.push_front(Binding(new StringVar(rightResult)));
  }
};

class Append : public Action
{
public:
  static Append v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    std::string right = checkString(dataStack.front());
    dataStack.pop_front();
    std::string left = checkString(dataStack.front());
    dataStack.pop_front();
    dataStack.push_front(Binding(new StringVar(left + right)));
  }
};

class Size : public Action
{
public:
  static Size v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 1);
    std::string str = checkString(dataStack.front());
    dataStack.pop_front();
    int size = static_cast<int>(str.size());
    dataStack.push_front(Binding(new StringVar(intToString(size))));
  }
};

class PushNIL : public Action
{
public:
  static PushNIL v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    dataStack.push_front(NIL);
  }
};

class Cons : public Action
{
public:
  static Cons v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 2);
    Binding car = dataStack.front();
    dataStack.pop_front();
    Binding cdr = dataStack.front();
    dataStack.pop_front();
    dataStack.push_front(Binding(new ListVar(car, cdr)));
  }
};

class ListBreakup : public Action
{
public:
  static ListBreakup v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> &,
                   std::map<std::string, Board> &)
  {
    checkVarCount(dataStack, 1);
    ConsCell cell = checkList(dataStack.front());
    dataStack.pop_front();
    dataStack.push_front(cell.cdr);
    dataStack.push_front(cell.car);
  }
};

class GreaterThan : public Action
{
public:
  static GreaterThan v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> & globals)
  {
    checkVarCount(dataStack, 2);
    int right = checkNumeric(dataStack.front());
    dataStack.pop_front();
    int left = checkNumeric(dataStack.front());
    dataStack.pop_front();
    if (left > right)
    {
      True::v.run(dataStack, programStack, globals);
    }
    else
    {
      False::v.run(dataStack, programStack, globals);
    }
  }
};

class IsEqualTo : public Action
{
public:
  static IsEqualTo v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> & globals)
  {
    checkVarCount(dataStack, 2);
    Binding right = dataStack.front();
    dataStack.pop_front();
    Binding left = dataStack.front();
    dataStack.pop_front();
    if (areEqual(left, right))
    {
      True::v.run(dataStack, programStack, globals);
    }
    else
    {
      False::v.run(dataStack, programStack, globals);
    }
  }
};

class Reflector : public Action
{
public:
  static Reflector v;
  virtual void run(std::list<Binding> &,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> &)
  {
    ActivationRecord & record = programStack.front();
    record.setDirection(Dir::back(record.getDirection()));
  }
};

class Lambda : public Action
{
public:
  static Lambda v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> & globals)
  {
    ActivationRecord & record = programStack.front();
    if (record.getFunction() == NULL)
    {
      throw InternalException("Lambda: No function in the given record");
    }
    dataStack.push_front(Binding(new Closure(record.getEnvironment(),
                                             *(record.getFunction()),
                                             record.getPosition(),
                                             record.getDirection())));
    record.setDirection(Dir::back(record.getDirection()));
  }
};

class End : public Action
{
public:
  static End v;
  virtual void run(std::list<Binding> & dataStack,
                   std::list<ActivationRecord> & programStack,
                   std::map<std::string, Board> &)
  {
    programStack.pop_front();
  }
};

#endif

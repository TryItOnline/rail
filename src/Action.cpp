// Action.cpp

#include "lib.h"
#include "Action.h"
#include "Binding.h"
#include "Dir.h"
#include "Error.h"
#include "ActivationRecord.h"

using namespace std;

namespace
{
  struct CharTable
  {
    CharTable()
    {
      for (int i = 0; i < 256; ++i)
      {
        data[i] = &(Nop::v);
      }

      data['t'] = &(True::v);
      data['f'] = &(False::v);

      data['>'] = &(EastY::v);
      data['<'] = &(WestY::v);
      data['^'] = &(NorthY::v);
      data['v'] = &(SouthY::v);

      data['b'] = &(Boom::v);
      data['e'] = &(EofCheck::v);
      data['i'] = &(Input::v);
      data['o'] = &(Output::v);
      data['u'] = &(Underflow::v);
      data['?'] = &(TypeP::v);

      data['a'] = &(Add::v);
      data['d'] = &(Divide::v);
      data['m'] = &(Multiply::v);
      data['r'] = &(Remainder::v);
      data['s'] = &(Subtract::v);
      data['0'] = &(Digit::v0);
      data['1'] = &(Digit::v1);
      data['2'] = &(Digit::v2);
      data['3'] = &(Digit::v3);
      data['4'] = &(Digit::v4);
      data['5'] = &(Digit::v5);
      data['6'] = &(Digit::v6);
      data['7'] = &(Digit::v7);
      data['8'] = &(Digit::v8);
      data['9'] = &(Digit::v9);

      data['c'] = &(Cut::v);
      data['p'] = &(Append::v);
      data['z'] = &(Size::v);

      data['n'] = &(PushNIL::v);
      data[':'] = &(Cons::v);
      data['~'] = &(ListBreakup::v);

      data['g'] = &(GreaterThan::v);
      data['q'] = &(IsEqualTo::v);

      data['@'] = &(Reflector::v);
      data['&'] = &(Lambda::v);
      data['#'] = &(End::v);
    }

    Action * data[256];
  };
}

Action * Action::charToAction(char current)
{
  static CharTable result;
  return result.data[current];
}

void Action::checkVarCount(list<Binding> const & theStack, int count)
{
  if (theStack.size() < static_cast<size_t>(count))
  {
    throw CrashException(Error::stackUnderflow);
  }
}

bool Action::checkBool(Binding var)
{
  bool result = true;
  string str = checkString(var);
  if (str == "0")
  {
    result = false;
  }
  else if (str == "1")
  {
    result = true;
  }
  else
  {
    throw CrashException(Error::typeMismatch);
  }
  return result;
}

int Action::checkNumeric(Binding var)
{
  int result = 0;
  string str = checkString(var);
  bool allDigits = true;
  for (size_t i = 0; i < str.size() && allDigits; ++i)
  {
    allDigits = allDigits && isdigit(str[i]);
  }
  if (allDigits)
  {
    istringstream stream(str);
    stream >> result;
  }
  else
  {
    throw CrashException(Error::typeMismatch);
  }
  return result;
}

ConsCell Action::checkList(Binding var)
{
  ConsCell result;
  ListVar * lvar = var->getList();
  if (lvar == NULL)
  {
    throw CrashException(Error::typeMismatch);
  }
  result = lvar->getCell();
  return result;
}

string Action::checkString(Binding var)
{
  string result;
  StringVar * svar = var->getString();
  if (svar == NULL)
  {
    throw CrashException(Error::typeMismatch);
  }
  result = svar->get();
  return result;
}

bool Action::areEqual(Binding left, Binding right)
{
  bool result = false;
  if (left->getString() != NULL && right->getString() != NULL)
  {
    result = (left->getString()->get() == right->getString()->get());
  }
  else if (left->getList() != NULL && right->getList() != NULL)
  {
    result = (areEqual(left->getList()->getCar(),
                       right->getList()->getCar())
              && areEqual(left->getList()->getCdr(),
                          right->getList()->getCdr()));
  }
  else if (left->getNil() != NULL && right->getNil() != NULL)
  {
    result = true;
  }
  else if (left->getClosure() != NULL && right->getClosure() != NULL)
  {
    Closure & leftClosure = *(left->getClosure());
    Closure & rightClosure = *(right->getClosure());
    result = &(leftClosure.getFunction()) == &(rightClosure.getFunction())
      && leftClosure.getPosition() == rightClosure.getPosition()
      && leftClosure.getDirection() == rightClosure.getDirection();
    std::map<std::string, Binding>::const_iterator leftPos
      = leftClosure.getEnvironment().begin();
    std::map<std::string, Binding>::const_iterator leftLimit
      = leftClosure.getEnvironment().end();
    std::map<std::string, Binding>::const_iterator rightPos
      = rightClosure.getEnvironment().begin();
    std::map<std::string, Binding>::const_iterator rightLimit
      = rightClosure.getEnvironment().end();
    for (; leftPos != leftLimit && rightPos != rightLimit && result;
         ++leftPos, ++rightPos)
    {
      result = result && areEqual(leftPos->second, rightPos->second);
    }
  }
  else
  {
    result = false;
  }
  return result;
}

// first second and third are the incoming directions of the
// y-junction in counterclockwise order.
void Action::runY(list<Binding> & dataStack,
                  list<ActivationRecord> & programStack,
                  Dir::t first, Dir::t second, Dir::t third)
{
  checkVarCount(dataStack, 1);
  ActivationRecord & current = programStack.front();
  bool test = checkBool(dataStack.front());
  Dir::t direction = current.getDirection();
  if ((direction == second && !test)
      || (direction == third && test))
  {
    current.setDirection(Dir::back(first));
  }
  else if ((direction == third && !test)
           || (direction == first && test))
  {
    current.setDirection(Dir::back(second));
  }
  else if ((direction == first && !test)
           || (direction == second && test))
  {
    current.setDirection(Dir::back(third));
  }
  else
  {
    throw CrashException(Error::wrongDirectionIntoJunction);
  }
  dataStack.pop_front();
}

void Action::callFunctionByName(string const & name,
                                list<ActivationRecord> & programStack,
                                map<string, Board> const & globals)
{
  std::map<std::string, Board>::const_iterator pos
                                                  = globals.find(name);
  if (pos != globals.end())
  {
    programStack.push_front(ActivationRecord(pos->second));
  }
  else
  {
    throw CrashException(Error::globalBindingNotFound);
  }
}

Nop Nop::v;
True True::v;
False False::v;

EastY EastY::v;
WestY WestY::v;
NorthY NorthY::v;
SouthY SouthY::v;

Boom Boom::v;
EofCheck EofCheck::v;
Input Input::v;
Output Output::v;
Underflow Underflow::v;
TypeP TypeP::v;

PushConstant PushConstant::v;
UseVariable UseVariable::v;
BindVariable BindVariable::v;
CallFunction CallFunction::v;

Add Add::v;
Divide Divide::v;
Multiply Multiply::v;
Remainder Remainder::v;
Subtract Subtract::v;
Digit Digit::v0(0);
Digit Digit::v1(1);
Digit Digit::v2(2);
Digit Digit::v3(3);
Digit Digit::v4(4);
Digit Digit::v5(5);
Digit Digit::v6(6);
Digit Digit::v7(7);
Digit Digit::v8(8);
Digit Digit::v9(9);

Cut Cut::v;
Append Append::v;
Size Size::v;

PushNIL PushNIL::v;
Cons Cons::v;
ListBreakup ListBreakup::v;

GreaterThan GreaterThan::v;
IsEqualTo IsEqualTo::v;

Reflector Reflector::v;
Lambda Lambda::v;
End End::v;

// ActivationRecord.cpp

#include "lib.h"
#include "ActivationRecord.h"
#include "Rail.h"
#include "Error.h"
#include "MultiLexer.h"
#include "StringVar.h"
#include "Action.h"

using namespace std;

ActivationRecord::ActivationRecord()
  : function(NULL)
  , direction(Dir::north)
{
}

ActivationRecord::ActivationRecord(Board const & source)
  : function(&source)
  , position(0,0)
  , direction(Dir::southeast)
{
}

ActivationRecord::ActivationRecord(Closure const & source)
  : environment(source.getEnvironment())
  , function(&(source.getFunction()))
  , position(source.getPosition())
  , direction(source.getDirection())
{
}

ActivationRecord::~ActivationRecord()
{
}

ActivationRecord::ActivationRecord(ActivationRecord const & right)
{
  // This is exception-safe because the environment copy is the only
  // thing that can fail.
  environment = right.environment;
  function = right.function;
  position = right.position;
  direction = right.direction;
}

ActivationRecord & ActivationRecord::operator=(ActivationRecord const & right)
{
  environment = right.environment;
  function = right.function;
  position = right.position;
  direction = right.direction;
  return *this;
}

void ActivationRecord::move(MultiLexer & multi)
{
  if (function == NULL)
  {
    throw InternalException("ActivationRecord::move(): No board");
  }
  if (multi.isDone())
  {
    throw InternalException("ActivationRecord::move(): Move requested when multi is already done");
  }
  else if (multi.isStarted())
  {
    position = position + Dir::dirToVec(direction);
    if (position.x < function->getMinX() || position.x > function->getMaxX()
      || position.y < function->getMinY() || position.y > function->getMaxY())
    {
      throw CrashException(Error::multiOutOfBounds);
    }
  }
  else
  {
    normalMove();
  }
}

void ActivationRecord::normalMove(void)
{
  Vec primPos = position + Dir::dirToVec(direction);
  Vec leftPos = position + Dir::dirToVec(Dir::left(direction));
  Vec rightPos = position + Dir::dirToVec(Dir::right(direction));

  Rail::t sourceRail = Rail::charToRail(function->at(position));
  Rail::t primRail = Rail::charToRail(function->at(primPos));
  Rail::t leftRail = Rail::charToRail(function->at(leftPos));
  Rail::t rightRail = Rail::charToRail(function->at(rightPos));

  Rail::t resultRail = Rail::rubble;

  if (Rail::isPrimary(sourceRail, primRail, direction))
  {
    position = primPos;
    resultRail = primRail;
  }
  else
  {
    bool isLeft = Rail::isLeft(sourceRail, leftRail);
    bool isRight = Rail::isRight(sourceRail, rightRail);
    if (isLeft && !isRight)
    {
      position = leftPos;
      resultRail = leftRail;
    }
    else if (!isLeft && isRight)
    {
      position = rightPos;
      resultRail = rightRail;
    }
    else if (isLeft && isRight)
    {
      throw CrashException(Error::ambiguousMove);
    }
    else
    {
      // !isLeft && !isRight
//      cerr << "orig:  '" << function->at(position) << "'" << endl;
//      cerr << "left:  '" << function->at(leftPos) << "'" << endl;
//      cerr << "prim:  '" << function->at(primPos) << "'" << endl;
//      cerr << "right: '" << function->at(rightPos) << "'" << endl;
      throw CrashException(Error::noValidMove);
    }
  }
  direction = Rail::correctDirection(resultRail, direction);
}

Action & ActivationRecord::getAction(MultiLexer & multi,
                                     list<Binding> & dataStack)
{
  if (function == NULL)
  {
    throw InternalException("ActivationRecord::act(): No board");
  }
  if (multi.isDone())
  {
    throw InternalException("ActivationRecord::getAction(): action requested when multi is already done");
  }
  else if (multi.isStarted())
  {
    multi.add(function->at(position));
    if (multi.isFunction())
    {
      dataStack.push_front(Binding(new StringVar(multi.getToken())));
      multi.reset();
      return CallFunction::v;
    }
    else if (multi.isConstant())
    {
      dataStack.push_front(Binding(new StringVar(multi.getToken())));
      multi.reset();
      return PushConstant::v;
    }
    else if (multi.isUse())
    {
      dataStack.push_front(Binding(new StringVar(multi.getToken())));
      multi.reset();
      return UseVariable::v;
    }
    else if (multi.isBind())
    {
      dataStack.push_front(Binding(new StringVar(multi.getToken())));
      multi.reset();
      return BindVariable::v;
    }
    else
    {
      return Nop::v;
    }
  }
  else
  {
    switch(function->at(position))
    {
    case '[':
    case ']':
    case '{':
    case '}':
    case '(':
    case ')':
      multi.add(function->at(position));
      return Nop::v;
    default:
      Action * act = Action::charToAction(function->at(position));
      if (act == NULL)
      {
        throw InternalException("ActivationRecord::act(): NULL action");
      }
      return *act;
    }
  }
}

std::map<std::string, Binding> const & ActivationRecord::getEnvironment(void)
  const
{
  return environment;
}

void ActivationRecord::bindVariable(string name, Binding var)
{
  environment[name] = var;
}

Board const * ActivationRecord::getFunction(void) const
{
  return function;
}

Vec ActivationRecord::getPosition(void) const
{
  return position;
}

void ActivationRecord::setPosition(Vec newPosition)
{
  position = newPosition;
}

Dir::t ActivationRecord::getDirection(void) const
{
  return direction;
}

void ActivationRecord::setDirection(Dir::t newDirection)
{
  direction = newDirection;
}

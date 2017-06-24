// ActivationRecord.h

#ifndef ACTIVATION_RECORD_H_RAIL_1
#define ACTIVATION_RECORD_H_RAIL_1

#include "Vec.h"
#include "Board.h"
#include "Dir.h"
#include "Closure.h"
#include "Binding.h"

class Action;
class MultiLexer;

class ActivationRecord
{
public:
  ActivationRecord();
  ActivationRecord(Board const & source);
  ActivationRecord(Closure const & source);
  ~ActivationRecord();

  ActivationRecord(ActivationRecord const & right);
  ActivationRecord & operator=(ActivationRecord const & right);

  void move(MultiLexer & multi);
  Action & getAction(MultiLexer & multi, std::list<Binding> & dataStack);

  std::map<std::string, Binding> const & getEnvironment(void) const;
  void bindVariable(std::string name, Binding var);
  Board const * getFunction(void) const;
  Vec getPosition(void) const;
  void setPosition(Vec newPosition);
  Dir::t getDirection(void) const;
  void setDirection(Dir::t newDirection);
private:
  void normalMove(void);
private:
  std::map<std::string, Binding> environment;
  Board const * function;
  Vec position;
  Dir::t direction;
};

#endif

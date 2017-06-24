// Thread.h

#ifndef THREAD_H_RAIL_1
#define THREAD_H_RAIL_1

#include "Board.h"
#include "ActivationRecord.h"
#include "Binding.h"
#include "Error.h"
#include "MultiLexer.h"

class Thread
{
public:
  Thread();
  ~Thread();
  void reset(Board & start, std::map<std::string, Board> * newGlobals);
  void go(void);
  void step(void);
  std::list<Binding> const & getDataStack(void);
  std::list<ActivationRecord> const & getProgramStack(void);
  Error::t getStatus(void);
  std::string getErrorString(void);
private:
  std::list<Binding> dataStack;
  std::list<ActivationRecord> programStack;
  std::map<std::string, Board> * globals;
  MultiLexer multi;

  Error::t status;
  std::string custom;
};

#endif

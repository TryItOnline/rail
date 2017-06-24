// Thread.cpp

#include "lib.h"
#include "Thread.h"
#include "Error.h"
#include "Action.h"

using namespace std;

Thread::Thread()
  : status(Error::finished)
{
}

Thread::~Thread()
{
}

void Thread::reset(Board & start, map<string, Board> * newGlobals)
{
  dataStack.clear();
  programStack.clear();
  programStack.push_back(ActivationRecord(start));
  globals = newGlobals;
  multi.reset();
  if (globals != NULL)
  {
    status = Error::runnable;
  }
  else
  {
    status = Error::finished;
  }
  custom.clear();
}

void Thread::go(void)
{
  bool done = false;

  if (status != Error::runnable)
  {
    done = true;
  }
  while (!done)
  {
    step();
    if (status != Error::runnable)
    {
      done = true;
    }
  }
  status = Error::finished;
}

void Thread::step(void)
{
  if (status == Error::runnable)
  {
    try
    {
      programStack.front().move(multi);
      Action & act = programStack.front().getAction(multi, dataStack);
      act.run(dataStack, programStack, *globals);
    }
    catch(CrashException & error)
    {
      status = error.get();
      if (status == Error::custom)
      {
        custom = error.getCustom();
      }
      else
      {
        custom.clear();
      }
      cerr << "Crash: " << Error::errorToString(status);
      if (status == Error::custom)
      {
        cerr << " " << custom << endl;
      }
    }
  }

  if (programStack.empty())
  {
    status = Error::finished;
    custom.clear();
  }
}

list<Binding> const & Thread::getDataStack(void)
{
  return dataStack;
}

list<ActivationRecord> const & Thread::getProgramStack(void)
{
  return programStack;
}

Error::t Thread::getStatus(void)
{
  return status;
}

string Thread::getErrorString(void)
{
  if (status == Error::custom)
  {
    return Error::errorToString(status) + ": " + custom;
  }
  else
  {
    return Error::errorToString(status);
  }
}

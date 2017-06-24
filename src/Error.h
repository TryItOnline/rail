// Error.h

#ifndef ERROR_H_RAIL_1
#define ERROR_H_RAIL_1

namespace Error
{
  enum t
  {
    min = 0,
    runnable = 0,
    finished,
    custom,
    noValidMove,
    ambiguousMove,
    wrongDirectionIntoJunction,
    stackUnderflow,
    typeMismatch,
    invalidCharInFunctionName,
    invalidCharInConstant,
    invalidEscapeSequence,
    invalidCharInVariableName,
    multiOutOfBounds,
    localBindingNotFound,
    globalBindingNotFound,
    noMoreInput,
    indexOutOfBounds,
    max = indexOutOfBounds,
    count = max + 1
  };

  std::string errorToString(t current);
};

class CrashException
{
public:
  explicit CrashException(Error::t newType, std::string newCustom="")
    : type(newType)
    , custom(newCustom)
  {
  }

  Error::t get(void)
  {
    return type;
  }

  std::string getCustom(void)
  {
    return custom;
  }
private:
  Error::t type;
  std::string custom;
};

#endif

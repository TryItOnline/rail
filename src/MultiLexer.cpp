// MultiLexer.cpp

#include "lib.h"
#include "MultiLexer.h"
#include "Error.h"

using namespace std;

MultiLexer::MultiLexer()
  : state(EMPTY)
  , openChar('\0')
  , closeChar('\0')
{
}

void MultiLexer::reset(void)
{
  token.clear();
  state = EMPTY;
  openChar = '\0';
  closeChar = '\0';
}

void MultiLexer::add(char letter)
{
  switch (state)
  {
  case EMPTY:
    if (letter == '{')
    {
      state = FUNCTION_BEGIN;
      openChar = '{';
      closeChar = '}';
    }
    else if (letter == '}')
    {
      state = FUNCTION_BEGIN;
      openChar = '}';
      closeChar = '{';
    }
    else if (letter == '[')
    {
      state = CONSTANT_BEGIN;
      openChar = '[';
      closeChar = ']';
    }
    else if (letter == ']')
    {
      state = CONSTANT_BEGIN;
      openChar = ']';
      closeChar = '[';
    }
    else if (letter == '(')
    {
      state = VARIABLE_BEGIN;
      openChar = '(';
      closeChar = ')';
    }
    else if (letter == ')')
    {
      state = VARIABLE_BEGIN;
      openChar = ')';
      closeChar = '(';
    }
    else
    {
      throw InternalException(string("Invalid opening character: '") + letter + "'");
    }
    break;
  case FUNCTION_BEGIN:
  case FUNCTION_END:
    inFunction(letter);
    break;
  case CONSTANT_BEGIN:
  case CONSTANT_END:
  case ESCAPE_BEGIN:
  case ESCAPE_END:
    inConstant(letter);
    break;
  case VARIABLE_BEGIN:
    if (letter == openChar || letter == '{' || letter == '}' || letter == '\'')
    {
      throw CrashException(Error::invalidCharInVariableName);
    }
    else if (letter == closeChar)
    {
      state = USE_END;
    }
    else if (letter == '!')
    {
      state = BIND_BEGIN;
    }
    else
    {
      state = USE_BEGIN;
      token.push_back(letter);
    }
    break;
  case USE_BEGIN:
  case USE_END:
    inUse(letter);
    break;
  case BIND_BEGIN:
  case BIND_MIDDLE:
  case BIND_END:
    inBind(letter);
    break;
  default:
    throw InternalException("Unknown state in MultiLexer: "
                            + intToString(state));
    break;
  }
}

bool MultiLexer::isStarted(void) const
{
  return state != EMPTY;
}

bool MultiLexer::isDone(void) const
{
  return isFunction() || isConstant() || isUse() || isBind();
}

bool MultiLexer::isFunction(void) const
{
  return state == FUNCTION_END;
}

bool MultiLexer::isConstant(void) const
{
  return state == CONSTANT_END;
}

bool MultiLexer::isUse(void) const
{
  return state == USE_END;
}

bool MultiLexer::isBind(void) const
{
  return state == BIND_END;
}

string const & MultiLexer::getToken(void) const
{
  return token;
}

void MultiLexer::inFunction(char letter)
{
  switch (state)
  {
  case FUNCTION_BEGIN:
    if (letter == '\'' || letter == openChar || letter == '!' || letter == '('
      || letter == ')')
    {
      throw CrashException(Error::invalidCharInFunctionName);
    }
    else if (letter == closeChar)
    {
      state = FUNCTION_END;
    }
    else
    {
      state = FUNCTION_BEGIN;
      token.push_back(letter);
    }
    break;
  case FUNCTION_END:
    tooManyLetters(letter);
    break;
  }
}

void MultiLexer::inConstant(char letter)
{
  switch (state)
  {
  case CONSTANT_BEGIN:
    if (letter == '\\')
    {
      state = ESCAPE_BEGIN;
    }
    else if (letter == openChar)
    {
      throw CrashException(Error::invalidCharInConstant);
    }
    else if (letter == closeChar)
    {
      state = CONSTANT_END;
    }
    else
    {
      state = CONSTANT_BEGIN;
      token.push_back(letter);
    }
    break;
  case CONSTANT_END:
    tooManyLetters(letter);
    break;
  case ESCAPE_BEGIN:
    if (letter == '\\')
    {
      state = CONSTANT_BEGIN;
      token.push_back('\\');
    }
    else if (letter == 'n')
    {
      state = ESCAPE_END;
      token.push_back('\n');
    }
    else if (letter == 't')
    {
      state = ESCAPE_END;
      token.push_back('\t');
    }
    else if (letter == '[')
    {
      state = ESCAPE_END;
      token.push_back('[');
    }
    else if (letter == ']')
    {
      state = ESCAPE_END;
      token.push_back(']');
    }
    else
    {
      throw CrashException(Error::invalidEscapeSequence);
    }
    break;
  case ESCAPE_END:
    if (letter == '\\')
    {
      state = CONSTANT_BEGIN;
    }
    else
    {
      throw CrashException(Error::invalidEscapeSequence);
    }
    break;
  }
}

void MultiLexer::inUse(char letter)
{
  switch (state)
  {
  case USE_BEGIN:
    if (letter == openChar || letter == '!' || letter == '{' || letter == '}'
      || letter == '\'')
    {
      throw CrashException(Error::invalidCharInVariableName);
    }
    else if (letter == closeChar)
    {
      state = USE_END;
    }
    else
    {
      state = USE_BEGIN;
      token.push_back(letter);
    }
    break;
  case USE_END:
    tooManyLetters(letter);
    break;
  }
}

void MultiLexer::inBind(char letter)
{
  switch (state)
  {
  case BIND_BEGIN:
    if (letter == openChar || letter == closeChar || letter == '{'
        || letter == '}' || letter == '\'')
    {
      throw CrashException(Error::invalidCharInVariableName);
    }
    else if (letter == '!')
    {
      state = BIND_MIDDLE;
    }
    else
    {
      state = BIND_BEGIN;
      token.push_back(letter);
    }
    break;
  case BIND_MIDDLE:
    if (letter == closeChar)
    {
      state = BIND_END;
    }
    else
    {
      throw CrashException(Error::invalidCharInVariableName);
    }
    break;
  case BIND_END:
    tooManyLetters(letter);
    break;
  }
}

void MultiLexer::tooManyLetters(char letter)
{
  throw InternalException(string("MultiLexer is done but was sent a letter: '")
                          + letter + "'");
}

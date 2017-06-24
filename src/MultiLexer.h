// MultiLexer.h

#ifndef MULTI_LEXER_H_RAIL_1
#define MULTI_LEXER_H_RAIL_1

class MultiLexer
{
public:
  MultiLexer();
  void reset(void);
  void add(char letter);

  bool isStarted(void) const;
  bool isDone(void) const;
  bool isFunction(void) const;
  bool isConstant(void) const;
  bool isUse(void) const;
  bool isBind(void) const;
  std::string const & getToken(void) const;
private:
  void inFunction(char letter);
  void inConstant(char letter);
  void inUse(char letter);
  void inBind(char letter);
  void tooManyLetters(char letter);
private:
  enum StateT
  {
    EMPTY=0,

    FUNCTION_BEGIN,
    FUNCTION_END,

    CONSTANT_BEGIN,
    CONSTANT_END,
    ESCAPE_BEGIN,
    ESCAPE_END,

    VARIABLE_BEGIN,
    USE_BEGIN,
    USE_END,
    BIND_BEGIN,
    BIND_MIDDLE,
    BIND_END
  };
private:
  std::string token;
  StateT state;
  char openChar;
  char closeChar;
};

#endif

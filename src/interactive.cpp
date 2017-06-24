// interactive.cpp

#include "lib.h"
#include "interactive.h"
#include "Thread.h"
#include "Board.h"
#include "Vec.h"
#include "Binding.h"
#include "ActivationRecord.h"

using namespace std;

// In theory, these should all be mutually exclusive.

#ifdef _WIN32

#include "windows.h"

void redisplay(Thread & program);
void gotoPosition(int x, int y);
void printString(string const & line);
void setColor(WORD bits);
char readChar(void);
string getLastErrorString(void);
string convertToDebugString(string const & line);

HANDLE inputHandle;
HANDLE outputHandle;

static const WORD FOREGROUND_WHITE = FOREGROUND_BLUE | FOREGROUND_GREEN
                                     | FOREGROUND_RED;

static const WORD BACKGROUND_WHITE = BACKGROUND_BLUE | BACKGROUND_GREEN
                                     | BACKGROUND_RED;

void runInteractive(Thread & program)
{
  inputHandle = GetStdHandle(STD_INPUT_HANDLE);
  outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleMode(inputHandle, ENABLE_PROCESSED_INPUT);
  SetConsoleMode(outputHandle, ENABLE_PROCESSED_OUTPUT);
  COORD bufferSize;
  bufferSize.X = 80;
  bufferSize.Y = 25;
  SetConsoleScreenBufferSize(outputHandle, bufferSize);

  char choice = '\0';
  bool doRead = true;
  while (program.getStatus() == Error::runnable)
  {
    redisplay(program);
    if (doRead)
    {
      choice = readChar();
      if (choice == 's')
      {
        program.step();
      }
      else if (choice == 'r')
      {
        doRead = false;
      }
      else if (choice == 'x')
      {
        break;
      }
      else
      {
        // Do nothing.
      }
    }
    else
    {
      program.step();
    }
  }
}

void redisplay(Thread & program)
{
  list<Binding> const & dataStack(program.getDataStack());
  list<ActivationRecord> const & programStack(program.getProgramStack());
  ActivationRecord const & currentRecord(programStack.front());
  Board const * currentBoard(currentRecord.getFunction());
  if (currentBoard == NULL)
  {
    throw InternalException("redisplay(): currentBoard is NULL");
  }
  Vec currentPos = currentRecord.getPosition();
  // We want the current position in the center. Therefore we subtract
  // this offset from all position calculations in the program world.
  static const int windowOffset = 11;
  int y = 0;
  string line;
  line.reserve(35);

  // Print window on the board.
  setColor(FOREGROUND_WHITE);
  for (y = 0; y < 23; ++y)
  {
    line.clear();
    for (int x = 0; x < 23; ++ x)
    {
      line.push_back(currentBoard->at(Vec(x + currentPos.x - windowOffset,
                                          y + currentPos.y - windowOffset)));
    }
    gotoPosition(0, y);
    printString(line);
  }

  // Print current position in a different color for emphasis.
  gotoPosition(11,11);
  setColor(FOREGROUND_WHITE | BACKGROUND_BLUE);
  line.clear();
  line.push_back(currentBoard->at(currentPos));
  printString(line);

  // Print program stack
  setColor(FOREGROUND_WHITE);
  gotoPosition(24, 0);
  printString("Program Stack (top)");
  gotoPosition(24, 1);
  printString("-------------------");
  list<ActivationRecord>::const_iterator programPos = programStack.begin();
  list<ActivationRecord>::const_iterator programLimit = programStack.end();
  if (programStack.size() >= 21)
  {
    y = 2;
  }
  else
  {
    y = 23 - static_cast<int>(programStack.size());
  }
  //                                            1
  //                                  0123456789012345678
  static const string programBlank = "                   ";
  int blankY = 0;
  for (blankY = 2; blankY < y; ++blankY)
  {
    gotoPosition(24, blankY);
    printString(programBlank);
  }
  for (; programPos != programLimit && y < 23; ++programPos, ++y)
  {
    gotoPosition(24, y);
    line = programPos->getFunction()->getName();
    if (line.size() > 19)
    {
      line.resize(19);
    }
    while (line.size() < 19)
    {
      line.push_back(' ');
    }
    printString(line);
  }

  // Print data stack
  if (dataStack.size() >= 21)
  {
    y = 2;
  }
  else
  {
    y = 23 - static_cast<int>(dataStack.size());
  }
  static const string dataBlank =
  //           1         2         3         4
  // 012345678901234567890123456789012345678901234
    "                                             ";
  for (blankY = 0; blankY < y; ++blankY)
  {
    gotoPosition(44, blankY);
    printString(dataBlank);
  }
  gotoPosition(44, 0);
  printString("Data Stack (top)");
  gotoPosition(44, 1);
  printString("----------------");
  list<Binding>::const_iterator dataPos = dataStack.begin();
  list<Binding>::const_iterator dataLimit = dataStack.end();
  for (; dataPos != dataLimit && y < 23; ++dataPos, ++y)
  {
    gotoPosition(44, y);
    line = convertToDebugString((*dataPos)->toString());
    if (line.size() > 36)
    {
      line.resize(36);
    }
    while (line.size() < 36)
    {
      line.push_back(' ');
    }
    printString(line);
  }

  // Print blank columns
  setColor(FOREGROUND_WHITE | BACKGROUND_GREEN);
  static const string blankCol = " ";
  for (y = 0; y < 23; ++y)
  {
    gotoPosition(23, y);
    printString(blankCol);
    gotoPosition(43, y);
    printString(blankCol);
  }
  for (int x = 0; x < 80; ++x)
  {
    gotoPosition(x, 23);
    printString(blankCol);
  }
  gotoPosition(0, 23);
  printString("(" + intToString(currentPos.x) + ", "
              + intToString(currentPos.y) + ") "
              + Dir::dirToString(currentRecord.getDirection()));

  setColor(FOREGROUND_WHITE | BACKGROUND_RED);
  for (int x = 0; x < 80; ++x)
  {
    gotoPosition(x, 24);
    printString(blankCol);
  }
  gotoPosition(0, 24);
  printString("Please (s)tep, (r)un, or e(x)it");
  setColor(FOREGROUND_WHITE);
}

void gotoPosition(int x, int y)
{
  COORD pos;
  pos.X = x;
  pos.Y = y;
  if (SetConsoleCursorPosition(outputHandle, pos) == 0)
  {
    throw InternalException("redisplay(): Failed SetConsoleCursorPosition ("
                            + intToString(pos.X) + ", " + intToString(pos.Y)
                            + "): " + getLastErrorString());
  }
}

void printString(string const & line)
{
  DWORD writeCount = 0;
  BOOL error = WriteConsole(outputHandle, line.c_str(), static_cast<DWORD>(line.size()),
                            &writeCount, NULL);
  if (error == 0)
  {
    throw InternalException("printString(): Failed WriteConsole (" + line
                            + "): " + getLastErrorString());
  }
  if (writeCount < line.size())
  {
    throw InternalException(
      "printString(): WriteConsole didn't write enough characters (" + line
      + ")");
  }
}

void setColor(WORD bits)
{
  if (SetConsoleTextAttribute(outputHandle, bits) == 0)
  {
    throw InternalException("setColor(): Failed SetConsoleTextAttribute(): "
                            + getLastErrorString());
  }
}

char readChar(void)
{
  char buffer[2];
  DWORD readCount = 0;
  if (ReadConsole(inputHandle, buffer, 1, &readCount, NULL) == 0)
  {
    throw InternalException("readChar(): Failed ReadConsole(): "
                            + getLastErrorString());
  }
  if (readCount < 1)
  {
    throw InternalException("readChar(): ReadConsole didn't read any characters.");
  }
  return buffer[0];
}

string getLastErrorString(void)
{
  char * buffer = NULL;
  DWORD errorNumber = GetLastError();
  string error = "Error #" + intToString(errorNumber) + ": ";
  if (!FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorNumber,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &buffer,
        0,
        NULL ))
  {
    error += "META ERROR! An error occurred while trying to get the text for GetLastError()";
  }
  else
  {
    error += buffer;
  }
  LocalFree(buffer);
  return error;
}

string convertToDebugString(string const & line)
{
  string result;
  result.reserve(line.size());
  for (size_t i = 0; i < line.size(); ++i)
  {
    if (line[i] == '\\')
    {
      result += "\\\\";
    }
    else if (line[i] == '\n')
    {
      result += "\\n\\";
    }
    else if (line[i] == '\t')
    {
      result += "\\t\\";
    }
    else if (line[i] == '[')
    {
      result += "\\[\\";
    }
    else if (line[i] == ']')
    {
      result += "\\]\\";
    }
    else
    {
      result += line[i];
    }
  }
  return result;
}

#endif

#ifdef __GNUC__

void runInteractive(Thread & program)
{
  throw InternalException("Interactive mode is not yet implemented for *nix.");
}

#endif

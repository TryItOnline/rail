// Error.cpp

#include "lib.h"
#include "Error.h"

using namespace std;

namespace Error
{
  string errorToString(t current)
  {
    static string result[] = {
      "Runnable (no error)",
      "Finished (no error)",
      "Custom error: ",
      "No valid move",
      "Ambiguous move",
      "Wrong direction into junction",
      "Stack underflow",
      "Type mismatch",
      "Invalid character in function name",
      "Invalid character in constant",
      "Invalid escape sequence",
      "Invalid character in variable name",
      "No end delimiter found for multicharacter command",
      "There is no local binding with that name",
      "There is no global binding with that name",
      "A read was attempted when there is no more input"
      "Index Out Of Bounds for the 'cut' command"};
    if (current >= min && current < count)
    {
      return result[current];
    }
    else
    {
      return intToString(current);
    }
  }
}

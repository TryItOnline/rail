// Rail.cpp

#include "lib.h"
#include "Rail.h"

using namespace std;

namespace Rail
{
  struct PrimaryTable
  {
    PrimaryTable()
    {
      for (int i = min; i < count; ++i)
      {
        for (int j = min; j < count; ++j)
        {
          data[i][j] = false;
        }
      }

      data[horizontal][horizontal] = true;
      data[horizontal][ascending] = true;
      data[horizontal][descending] = true;
      data[horizontal][junction] = true;

      data[vertical][vertical] = true;
      data[vertical][ascending] = true;
      data[vertical][descending] = true;
      data[vertical][junction] = true;

      data[ascending][horizontal] = true;
      data[ascending][vertical] = true;
      data[ascending][ascending] = true;
      data[ascending][junction] = true;

      data[descending][horizontal] = true;
      data[descending][vertical] = true;
      data[descending][descending] = true;
      data[descending][junction] = true;
    }

    bool data[count][count];
  };

  bool isPrimary(t source, t dest, Dir::t current)
  {
    static PrimaryTable result;
    check(source, "Rail::isPrimary()");
    check(dest, "Rail::isPrimary()");
    Dir::check(current, "Rail::isPrimary()");

    if (source == junction)
    {
      source = dirToRail(current);
    }
//    cerr << "Primary: " << railToString(source) << " -> "
//         << railToString(dest) << endl;
    return result.data[source][dest];
  }

  struct LeftTable
  {
    LeftTable()
    {
      for (int i = min; i < count; ++i)
      {
        for (int j = min; j < count; ++j)
        {
          data[i][j] = false;
        }
      }

      data[horizontal][ascending] = true;
      data[ascending][vertical] = true;
      data[vertical][descending] = true;
      data[descending][horizontal] = true;
    }

    bool data[count][count];
  };

  bool isLeft(t source, t dest)
  {
    static LeftTable result;
    check(source, "Rail::isLeft()");
    check(dest, "Rail::isLeft()");
//    cerr << "Left: " << railToString(source) << " -> "
//         << railToString(dest) << endl;
    return result.data[source][dest];
  }

  struct RightTable
  {
    RightTable()
    {
      for (int i = min; i < count; ++i)
      {
        for (int j = min; j < count; ++j)
        {
          data[i][j] = false;
        }
      }

      data[horizontal][descending] = true;
      data[descending][vertical] = true;
      data[vertical][ascending] = true;
      data[ascending][horizontal] = true;
    }

    bool data[count][count];
  };

  bool isRight(t source, t dest)
  {
    static RightTable result;
    check(source, "Rail::isRight()");
    check(dest, "Rail::isRight()");
//    cerr << "Right: " << railToString(source) << " -> "
//         << railToString(dest) << endl;
    return result.data[source][dest];
  }

  Dir::t correctDirection(t current, Dir::t oldDir)
  {
    static const string functionName = "Rail::correctDirection()";
    check(current, functionName);
    Dir::check(oldDir, functionName);
    Dir::t result = oldDir;
    if (current == junction)
    {
      result = oldDir;
    }
    else if (current == rubble)
    {
      throw InternalException(functionName + ": Encountered rubble");
    }
    else
    {
      Dir::t forward = railToDirForward(current);
      Dir::t backward = railToDirBackward(current);
      if (oldDir == forward || oldDir == Dir::left(forward)
          || oldDir == Dir::right(forward))
      {
        result = forward;
      }
      else if (oldDir == backward || oldDir == Dir::left(backward)
               || oldDir == Dir::right(backward))
      {
        result = backward;
      }
      else
      {
        throw InternalException(functionName + ": Perpendicular direction: "
                                + railToString(current) + " -- "
                                + Dir::dirToString(oldDir));
      }
    }
    return result;
  }

  struct CharTable
  {
    CharTable()
    {
      for (int i = 0; i < 256; ++i)
      {
        data[i] = rubble;
      }

      data['-'] = horizontal;
      data['|'] = vertical;
      data['/'] = ascending;
      data['\\'] = descending;

      data['*'] = junction;
      data['+'] = junction;
      data['x'] = junction;
      data['v'] = junction;
      data['^'] = junction;
      data['<'] = junction;
      data['>'] = junction;

      data['b'] = junction;
      data['e'] = junction;
      data['i'] = junction;
      data['o'] = junction;
      data['u'] = junction;
      data['?'] = junction;

      data['['] = junction;
      data[']'] = junction;
      data['('] = junction;
      data[')'] = junction;
      data['{'] = junction;
      data['}'] = junction;

      data['a'] = junction;
      data['d'] = junction;
      data['m'] = junction;
      data['r'] = junction;
      data['s'] = junction;
      data['0'] = junction;
      data['1'] = junction;
      data['2'] = junction;
      data['3'] = junction;
      data['4'] = junction;
      data['5'] = junction;
      data['6'] = junction;
      data['7'] = junction;
      data['8'] = junction;
      data['9'] = junction;
      data['0'] = junction;

      data['c'] = junction;
      data['p'] = junction;
      data['z'] = junction;

      data['n'] = junction;
      data[':'] = junction;
      data['~'] = junction;

      data['f'] = junction;
      data['g'] = junction;
      data['q'] = junction;
      data['t'] = junction;

      data['@'] = junction;
      data['#'] = junction;
      data['&'] = junction;
      data['$'] = junction;
    }

    t data[256];
  };

  t charToRail(char current)
  {
    static CharTable result;
    return result.data[static_cast<unsigned char>(current)];
  }

  t dirToRail(Dir::t current)
  {
    static t result[] = {vertical, ascending, horizontal, descending,
                         vertical, ascending, horizontal, descending};
    Dir::check(current, "Rail::dirToRail()");
    return result[current];
  }

  Dir::t railToDirForward(t current)
  {
    static Dir::t result[] = {Dir::east, Dir::north, Dir::northeast,
                              Dir::southeast};
    check(current, "Rail::railToDirForward()");
    if (current == junction || current == rubble)
    {
      throw InternalException(
        "Rail::railToDirForward(): No inherent direction: "
        + railToString(current));
    }
    return result[current];
  }

  Dir::t railToDirBackward(t current)
  {
    return Dir::back(railToDirForward(current));
  }

  string railToString(t current)
  {
    static string result[] = {"horizontal", "vertical", "ascending",
                              "descending", "junction", "rubble"};
    if (current >= min && current <= max)
    {
      return result[current];
    }
    else
    {
      return intToString(current);
    }
  }

  void check(t current, string const & name)
  {
    if (current < min || current > max)
    {
      throw InternalException(name + ": Invalid Rail: "
                              + railToString(current));
    }
  }
}

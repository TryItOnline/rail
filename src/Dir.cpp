// Dir.cpp

#include "lib.h"
#include "Dir.h"

using namespace std;

namespace Dir
{
  t back(t current)
  {
    static t result[] = {south, southwest, west, northwest,
                         north, northeast, east, southeast};
    check(current, "Dir::back()");
    return result[current];
  }

  t left(t current)
  {
    static t result[] = {northwest, north, northeast, east,
                         southeast, south, southwest, west};
    check(current, "Dir::left()");
    return result[current];
  }

  t right(t current)
  {
    static t result[] = {northeast, east, southeast, south,
                         southwest, west, northwest, north};
    check(current, "Dir::right()");
    return result[current];
  }

  string dirToString(t current)
  {
    static string result[] = {"north", "northeast", "east", "southeast",
                              "south", "southwest", "west", "northwest"};
    if (current >= min && current <= max)
    {
      return result[current];
    }
    else
    {
      return intToString(current);
    }
  }

  Vec dirToVec(t current)
  {
    static Vec result[] = {Vec(0, -1), Vec(1, -1), Vec(1, 0), Vec(1, 1),
                           Vec(0, 1), Vec(-1, 1), Vec(-1, 0), Vec(-1, -1)};
    check(current, "Dir::dirToVec()");
    return result[current];
  }

  void check(t current, string const & name)
  {
    if (current < min || current > max)
    {
      throw InternalException(name + ": Invalid Direction: "
                              + dirToString(current));
    }
  }
}

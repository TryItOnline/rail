// Rail.h

#ifndef RAIL_H_RAIL_1
#define RAIL_H_RAIL_1

#include "Dir.h"

namespace Rail
{
  enum t
  {
    min = 0,
    horizontal = 0,
    vertical = 1,
    ascending = 2,
    descending = 3,
    junction = 4,
    rubble = 5,
    max = 5,
    count = 6
  };

  bool isPrimary(t source, t dest, Dir::t current);
  bool isLeft(t source, t dest);
  bool isRight(t source, t dest);

  Dir::t correctDirection(t current, Dir::t oldDir);

  t charToRail(char current);
  t dirToRail(Dir::t current);
  Dir::t railToDirForward(t current);
  Dir::t railToDirBackward(t current);
  std::string railToString(t current);

  void check(t current, std::string const & name);
}

#endif

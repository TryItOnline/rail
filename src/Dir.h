// Dir.h

#ifndef DIR_H_RAIL_1
#define DIR_H_RAIL_1

#include "Vec.h"

namespace Dir
{
  enum t
  {
    min = 0,
    north = 0,
    northeast = 1,
    east = 2,
    southeast = 3,
    south = 4,
    southwest = 5,
    west = 6,
    northwest = 7,
    max = 7,
    count = 8
  };

  t back(t current);
  t left(t current);
  t right(t current);
  std::string dirToString(t current);
  Vec dirToVec(t current);

  void check(t current, std::string const & name);
}

#endif

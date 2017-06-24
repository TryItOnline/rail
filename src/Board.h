// Board.h

#ifndef BOARD_H_RAIL_1
#define BOARD_H_RAIL_1

#include "Vec.h"

class Board
{
public:
  Board();
  ~Board();
  Board(std::string const & newName, std::list<std::string> const & newData);
  void reset(std::string const &newName,
             std::list<std::string> const & newData);

  char at(Vec pos) const;
  std::string const & getName(void) const;
  int getMinX(void) const;
  int getMaxX(void) const;
  int getMinY(void) const;
  int getMaxY(void) const;
private:
  std::string name;
  std::vector<std::string> data;
  int maxX;
  int maxY;
};

#endif

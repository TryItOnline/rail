// Board.cpp

#include "lib.h"
#include "Board.h"

using namespace std;

Board::Board()
{
}

Board::~Board()
{
}

Board::Board(string const & newName, list<string> const & newData)
{
  reset(newName, newData);
}

// This scheme makes maxX and maxY one greater than they have to
// be. But this doesn't matter.
void Board::reset(string const & newName, list<string> const & newData)
{
  maxX = 0;
  maxY = 0;
  name = newName;
  data.clear();
  data.reserve(newData.size());
  list<string>::const_iterator sourcePos = newData.begin();
  list<string>::const_iterator sourceLimit = newData.end();
  for (; sourcePos != sourceLimit; ++sourcePos)
  {
    data.push_back(*sourcePos);
    maxX = max(maxX, static_cast<int>(sourcePos->size()));
    ++maxY;
  }
}

char Board::at(Vec pos) const
{
  char result = ' ';
  if (pos.y >= 0 && pos.y < static_cast<int>(data.size()))
  {
    if (pos.x >= 0 && pos.x < static_cast<int>(data[pos.y].size()))
    {
      result = data[pos.y][pos.x];
    }
  }
  return result;
}

string const & Board::getName(void) const
{
  return name;
}

int Board::getMinX(void) const
{
  return 0;
}

int Board::getMaxX(void) const
{
  return maxX;
}

int Board::getMinY(void) const
{
  return 0;
}

int Board::getMaxY(void) const
{
  return maxY;
}

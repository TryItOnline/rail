// Vec.h

#ifndef VEC_H_RAIL_1
#define VEC_H_RAIL_1

struct Vec
{
  Vec() : x(0), y(0) {}
  Vec(int newX, int newY) : x(newX), y(newY) {}

  Vec & operator+=(Vec right)
  {
    x += right.x;
    y += right.y;
    return *this;
  }

  Vec operator+(Vec right)
  {
    Vec result(*this);
    result += right;
    return result;
  }

  bool operator==(Vec const & right) const
  {
    return x == right.x && y == right.y;
  }

  int x;
  int y;
};

#endif

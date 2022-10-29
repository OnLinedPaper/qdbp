#ifndef PATHER_H_
#define PATHER_H_

#include <vector>

/*
an experimental qdbp environment design is a grid of chunks, some passable
and some impassable. such a grid is split into strips, blocks, or other 
shapes, and it is up to the player to construct a viable path through them.
of course, such a path must exist in the first place! that's what this utility
is for. it should be able to take a given rectangular area of arbitrary size 
and generate a path through it.

for reference:
- unit 0,0 is the top-left corner of the area
- format is row,column (y,x)
- the path is meant to proceed from left to right.

for further reference, "0" is impassible, "1" is passable, and "2" is the
path currently being generated.
*/
class pather {
public: 

  pather(int, int);
  ~pather();

  //make a path, starting from the given int, guaranteed not to intersect
  //with itself.
  //(this can be called multiple times; the paths may overlap if this is
  //the case)
  void path(int);

  void print();
  float get_density();

private:

  //a consistent zigzagging path, with extra "branches" added in until 
  //given density is achieved
  void path_v3(int, float = 0.4);
  //random walk, with bias towards moving right
  void path_v2(int);
  //constant movement to the right, with some up and down thrown in
  void path_v1(int);


  pather();
  int **a;
  int r, c;

};

#endif

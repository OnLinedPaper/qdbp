#ifndef PATHER_H_
#define PATHER_H_

#include <vector>

//point of interest
struct p_o_i {
  p_o_i() : rank(-1), x(0), y(0) { }
  p_o_i(int ri, int xi, int yi) : rank(ri), x(xi), y(yi) { }
  int rank;
  int x, y;

  bool operator<(const p_o_i &rhs) const;
};

/*
TODO: rewrite this
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

  int get_r() const { return r; }
  int get_c() const { return c; }
  int get_start() const;
  int get_end() const;
  float get_density() const;

  //make a path, starting from the given int, guaranteed not to intersect
  //with itself.
  //(this can be called multiple times; the paths may overlap if this is
  //the case)
  //the second argument is "density" - how much of the graph should
  //be taken up
  void path(int, float = 0);

  void print() const;

  //run dijkstra's algorithm to both determine whether a path exists, and the
  //distance of any given point from the shortest path. this information is
  //placed into the graph for later use.
  bool check_path_exists(int);

  //return the array value at a given location
  //horizontal axist first, vertical axis second
  int at(int x, int y) const { return a[y][x]; }
  //TODO: vec2d support?

  //fills a provided vector with information about points of interest on the 
  //map that are far from the main path.
  //"rank" is the number of points of interest to find, with the furthest point
  //regarded as rank 1, second furthest as rank 2, and so on.
  void get_far_point(std::vector<p_o_i> &, int) const;
  void get_far_point_v1(std::vector<p_o_i> &, int) const;

private:

  //a consistent zigzagging path 
  void path_v3(int);
  //random walk, with bias towards moving right
  void path_v2(int);
  //constant movement to the right, with some up and down thrown in
  void path_v1(int);


  //random noise, but only touching other paths
  void fill_v3(float);
  //random noise, but as 2x2 squares
  void fill_v2(float);
  //random noise added into the graph
  void fill_v1(float);


  //convert all extraneous data to uniform format
  void flatten();
  //flip all signs from positive to negative
  void flip_signs();

  //if a path exists, populate the map with data about it
  void find_path_data();

  //find local maxima
  bool is_local_max(int, int) const;

  pather();
  int **a;
  int r, c;

};

#endif

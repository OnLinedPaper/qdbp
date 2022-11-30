#include "pather.h"
#include "src/utils/rng.h"
//#include "rng.cpp" //for local testing
#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
#include <vector>
#include <utility>
#include <string>


//prioritizes rank, then rightmost point, then bottommost point
bool p_o_i::operator<(const p_o_i &rhs) const {
  if(rank < rhs.rank) { return true; }
  else if (rank > rhs.rank) { return false; }
  else {
    if(x < rhs.x) { return true; }
    else if (x > rhs.x) { return false; }
    else {
      if(y < rhs.y) { return true; }
      else { return false; }
    }
  }
}


pather::~pather() {
  for(int i=0; i<r; i++) {
    delete[] a[i];
  }

  delete[] a;
} 

pather::pather(int rows, int cols): 
    r(rows)
  , c(cols) 
{
  if(r < 1 || c < 1) { throw("invalid pather dimensions!"); }

  a = new int*[r];

  for(int i=0; i<r; i++) {
    a[i] = new int[c](); //init values to 0
  }
}


//=============================================================================

void pather::path(int start, float density) {
  //this is deterministic, and will always produce the same result when run 
  //with the same seed.
  //need to work on the actual algorithm though...
  if(start < 0 || start >= r) { throw("invalid pathing start point!"); }

  path_v3(start);
  fill_v2(density);
  check_path_exists(start);
  //TODO: remove this when done testing
  print();
}

/*=============================================================================
the path functions describe various ways to walk through a grid, leaving a
traversible path behind them. 
*/

//-----------------------------------------------------------------------------
//zigzagging path
void pather::path_v3(int start) {
  //the current "position" in the path box
  int loc[2] = {start, 0};
  //the direction to travel in
  int dir = 0;
  //the current direction's max available travel distance
  int dist_max = 0;
  //the current direction's minimum required travel distance
  int dist_min = 0;
  //the actual distance chosen to travel
  int trv = 0;

  //some constants
  int UP = 0;
  int DN = 1;
  //some constants to make the array's zero-indexing easier to deal with
  int cm = c-1;
  int rm = r-1;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
part 1 - make a zigzagging path, with the following priorities addressed (in 
order of priority):

- a horizontal path should be at least as long as 3 chunk units, or 1/4 of the 
  total width of the box, whichever is longer
- no horizontal path should be longer than 1/3 of the total width of the box
- if the box is small enough that both of these rules cannot be followed, the
  minimum length rule takes precedence

- a vertical path should be at least as long as 3 chunk units, or 1/3 of the 
  total height of the box, whichever is longer
- no vertical path should be longer than 2/3 of the total height of the box
- if the box is small enough that both of these rules cannot be followed, the
  minimum length rule takes precedence

these values are likely going to be tweak-able and may eventually also become
parameters, like density
*/

  //horizontal min length (constant)
  int h_min_l_cnst = 2;
  //horizontal min length (ratio)
  float h_min_l_rtio = 1.0/5;  
  //horizontal max length (constant)
  int h_max_l_cnst = INT_MAX;
  //horizontal max length (ratio)
  float h_max_l_rtio = 1.0/2;

  //vertical min height (constant)
  int v_min_h_cnst = 3;
  //vertical min height (ratio)
  float v_min_h_rtio = 1.0/3;
  //vertical max height (constant)
  int v_max_h_cnst = INT_MAX;
  //vertical max height (ratio)
  float v_max_h_rtio = 1;

  
  //fire up a loop of horizontals and verticals
  while(loc[1] < cm) {

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

    //first do a horizontal

    //initial total dist available to travel
    dist_max = cm - loc[1];
    //clamp so it doesn't go too far
    dist_max = (dist_max < h_max_l_rtio * c ? dist_max : h_max_l_rtio * c);
    dist_max = (dist_max < h_max_l_cnst ? dist_max : h_max_l_cnst);

    //select minimum distance required to travel
    dist_min = 0;
    //clamp so it goes far enough
    dist_min = (
      h_min_l_cnst > h_min_l_rtio * c 
      ? h_min_l_cnst
      : h_min_l_rtio * c 
    );
    //make sure we don't run past boundary of the array - check this by 
    //verifying the min dist is not greater than the max dist.
    //this triggers when we're below the min distance from the wall.
    dist_min = (dist_min > dist_max ? dist_max : dist_min);

    //get a number between the two
    if(dist_max == dist_min) { trv = dist_max; }
    else {
      trv = (std::abs(rng::get().get_map()) % (dist_max-dist_min)) + dist_min;
    }

    //walk that number of steps to the right
    for(int i=0; i<trv; i++) {
      a[loc[0]][loc[1]] = 2;
      loc[1]++;
    }

    //terminate when we reach the end
    if(loc[1] == cm) { break; }

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

    /*
    now, deal with the vertical movement
    first, pick whether to go up or down, by choosing a random number and
    seeing which "side" of the line it falls on (repeating if necessary) - this
    favors the side with more empty space. 
    the line must meet the minimum sapce requirements, so if a wall is too
    close by, the "choice" is bypassed. 
    */

    //minimum distance to travel is unchanging
    dist_min = 0;
    dist_min = (
      v_min_h_cnst > v_min_h_rtio * r
      ? v_min_h_cnst
      : v_min_h_rtio
    );

    //determine of either of these directions are too close to the wall
    
    bool go_up = loc[0] - dist_min >= 0;
    bool go_dn = loc[0] + dist_min <= rm;

    //can we move either direction? if not, skip vertical completely - it's the
    //user's responsibility to set the appropriate parameters for map size
    if(!go_up && !go_dn) { continue; }
    //can we go up?
    else if(go_up && !go_dn) { dir = UP; }
    //can we go down?
    else if(!go_up && go_dn) { dir = DN; }
    //we can do both
    else {
      //randomly select whether to go up or down
      //TODO: height-1 map failsafe
      int choice = loc[0];
      while(choice == loc[0]) {
        choice = std::abs(rng::get().get_map()) % r;
      }
      dir = (choice < loc[0] ? UP : DN);
    }

    //we have a direction - calculate max distance to travel
    if(dir == UP) { dist_max = loc[0]; }
    else { dist_max = rm - loc[0]; }
    dist_max = (dist_max < v_max_h_rtio * c ? dist_max : v_max_h_rtio * c);
    dist_max = (dist_max < v_max_h_cnst ? dist_max : v_max_h_cnst);

    //get travel distance
    if(dist_max == dist_min) { trv = dist_max; }
    else {
      trv = (std::abs(rng::get().get_map()) % (dist_max-dist_min)) + dist_min;
    }

    //walk that number of steps up or down
    
    for(int i=0; i<trv; i++) {
      a[loc[0]][loc[1]] = 2;
      if(dir == UP) { loc[0]--; }
      else { loc[0]++; }
    }
  }

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  
  //one more for the final resting point
  a[loc[0]][loc[1]] = 2;
}

//-----------------------------------------------------------------------------
//random walk that picks an unknown direction each step. biased towards 
//traveling to the right
//OBSERVATIONS:
//- tends to produce either very sparse or very dense paths
//- strong tendency to "group up" on itself and make double-wide paths
//- often produces long lines instead of shorter paths
//- always produces a solution with only one exit point, but multiple entry points
void pather::path_v2(int start) {
  //the current "position" in the path box
  int location[2] = {start, 0};
  //the direction to travel in
  int dir = 0;
  //the current direction's available travel distance
  int dist = 0;
  //the actual distance to travel
  int trv = 0;

  //some constants
  int LF = 0;
  int RT = 1;
  int UP = 2;
  int DN = 3;
  //constants that make the array's zero-indexing easier to deal with
  int cm = c-1;
  int rm = r-1;

  while(location[1] < cm) {
    //pick a direction to move
    dir = std::abs(rng::get().get_map()) % 4;
    
    //calculate the distance we can move in this direction (distance to wall)
    if(dir == LF) {
      //can move back to the left wall
      dist = location[1];
    }
    else if(dir == RT) {
      //can move forward to the right wall
      dist = cm - location[1];
    }
    else if(dir == UP) {
      //can move up to the top wall
      dist = location[0];
    }
    else if(dir == DN) {
      //can move down to bottom wall
      dist = rm - location[0];
    }

    //bias towards walking to the right
    if(dir == LF) {
      dist -= 1;
      dist = (dist < 0 ? 0 : dist);
    }
    else if(dir == RT) {
      dist += 2;
      dist = (dist > (cm-location[1]) ? (cm-location[1]) : dist);
    }

    //determine how far to actually walk
    trv = (dist > 0 ? std::abs(rng::get().get_map()) % dist + 1 : 0);

    //now walk

    //walk left, stopping at the wall
    if(dir == LF) {
      for(int i=0; i<trv && location[1] >= 0; i++) {
        a[location[0]][location[1]] = 2;
        location[1]--;
      }
    }
    else if(dir == RT) {
      for(int i=0; i<trv && location[1] <= cm; i++) {
        a[location[0]][location[1]] = 2;
        location[1]++;
      }
    }
    else if(dir == UP) {
      for(int i=0; i<trv && location[0] >= 0; i++) {
        a[location[0]][location[1]] = 2;
        location[0]--;
      }
    }
    else if(dir == DN) {
      for(int i=0; i<trv && location[0] <= rm; i++) {
        a[location[0]][location[1]] = 2;
        location[0]++;
      }
    }  

    //one more for the final resting point
    a[location[0]][location[1]] = 2;

    std::cout << (dir==UP ? "UP" : (dir==DN ? "DN" : (dir==LF ? "LF" : (dir==RT ? "RT" : "XX")))) << " " << dist << " " << trv << " | " << location[0] << "," << location[1] << std::endl;
  }
}

//-----------------------------------------------------------------------------
//on each iteration, move two or more to the right, and then some amount up
//or down
//this has a lot of annoying minus-ones in it to account for the fact that this
//may be the one time an array's zero-indexing becomes annoying, but i'm too
//stubborn to switch it... for now
void pather::path_v1(int start) {
  //the current "position" in the path box
  int location[2] = {start, 0};
  //the current rng's available travel distance
  int dist = 0;
  //the actual distance to travel
  int cm = c-1;

  while(location[1] < cm) {
    //first, go to the right
    //- always travels at least 2 units
    //- never travels more than 1/2 the available units
    //- never travels past rightmost edge
    dist = ((cm - location[1]) / 2) + 2;
    
    for(int i=location[1]; i<location[1]+dist && i<(cm); i++) {
      a[location[0]][i] = 2;
    }
    location[1] += dist-1;

    //TODO: code this
    location[0]++;
  }
}

/*=============================================================================
the fill functions describe various ways to add noise to a grid
*/

//-----------------------------------------------------------------------------
//random noise as 2x2 squares
void pather::fill_v2(float density) {
  int t_r = 0;
  int t_c = 0;
  while(get_density() < density) {
    t_r = std::abs(rng::get().get_map()) % (r-1);
    t_c = std::abs(rng::get().get_map()) % (c-1);

    a  [t_r]  [t_c] = 3;
    a[t_r+1]  [t_c] = 3;
    a  [t_r][t_c+1] = 3;
    a[t_r+1][t_c+1] = 3;
  }
}

//-----------------------------------------------------------------------------
//random noise
void pather::fill_v1(float density) {
  int t_r = 0;
  int t_c = 0;
  while(get_density() < density) {
    t_r = std::abs(rng::get().get_map()) % r;
    t_c = std::abs(rng::get().get_map()) % c;
    a[t_r][t_c] = 3;
  }
}

//=============================================================================
//convert all data to uniform format
void pather::flatten() {
  for(int i=0; i<r; i++) {
    for(int j=0; j<c; j++) {
      a[i][j] = (a[i][j] == 0 ? 0 : 1);
    }
  }
}

//-----------------------------------------------------------------------------
//flip all sign values
void pather::flip_signs() {
  for(int i=0; i<r; i++) {

  }
}


//=============================================================================
//dump the path to stdout
void pather::print() const {
  std::cout << "dumping pather object\n";
  std::cout << "density: " << std::trunc(get_density() * 1000) /10 << "%" << std::endl;
  std::cout << "start: " << (get_start() != -1 ? "(0, " + std::to_string(get_start()) + ")" : "none") << std::endl;
  std::cout << "end: " << (get_end() != -1 ? "(" + std::to_string(c-1) + ", " + std::to_string(get_end()) + ")" : "none") << std::endl;
  std::cout << std::endl;

  std::cout << "    ";
  for(int i=0; i<c; i++) {
    std::cout << i % 10 << " ";
  }
  std::cout << std::endl << std::endl;

  for(int i=0; i<r; i++) {
    std::cout << i%10 << "   ";
    for(int j=0; j<c; j++) {
      //currently printing inaccessible areas differently
      std::cout << (
        a[i][j] == 0 
        ? '-' 
        : (
          a[i][j] == INT_MAX 
          ? '=' 
          : (
            a[i][j] == 1 
            ? 'X' 
            : char(a[i][j] % 10 + 48)
          )
        )
      ) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

//=============================================================================
//get the pather's self-defined starting point
//(returns -1 if no starting point is present)
int pather::get_start() const {
  for(int i=0; i<r; i++) {
    if(a[i][0] == 1) {
      return i;
    }
  }
  return -1;
}

//-----------------------------------------------------------------------------
//get the pather's self-defined ending point
//(returns -1 if no ending point is present)
int pather::get_end() const {
  for(int i=0; i<r; i++) {
    if(a[i][c-1] == 1) {
      return i;
    }
  }
  return -1;
}

//=============================================================================
//get "density" - fraction of passable spaces occupied by a path
float pather::get_density() const {
  float d = 0;
  for(int i=0; i<r; i++) {
    for(int j=0; j<c; j++) {
      if(a[i][j] != 0) { d++; }
    }
  }
  return(d / (r*c));
}

//=============================================================================
//run dijkstra's algorithm to both get shortest path and to populate graph
//with information about a given chunk's distance from said shortest path. also
//confirms whether such a path even exists, returning false if not.
//note: the given starting coordinate is the y-coordinate of a chunk on the
//left side of a pather. the ending point is any chunk on the right side of 
//the pather.
//nodes that have value "0" are considered impassable.

bool pather::check_path_exists(int start) {
  //sanity check - are we attempting to path from an invalid starting point?
  //TODO: multiple starting points?
  if(a[start][0] == 0) { return false; }

  //first, generate a temporary object to hold the "visited/unvisited" graph,
  //and also adjust values of graph
  bool path_exists = false;
  //making this a 2D vector (ew) because VLAs are forbidden in cpp
  //bool visited[r][c]; 
  std::vector<std::vector<bool>> visited(r, std::vector<bool>(c, false));
  for(int i=0; i<r; i++) {
    for(int j=0; j<c; j++) {
      //impassible nodes are not considered 
      if(a[i][j] == 0) { visited[i][j] = true; } 
      else { 
        //passable nodes are prepared for the algorithm
        a[i][j] = INT_MAX;
        visited[i][j] = false; 
      }
    }
  }

  //next, prepare the vector to hold the unvisited paths
  std::vector<std::pair<int,int>> visit_me{{start, 0}};
  //and set the starting point
  a[start][0] = 1;

  //loop until all visitable points have been visited
  while(visit_me.size() > 0) {
    //get location value for current node
    int rv = visit_me[0].first;
    int cv = visit_me[0].second;

    if(cv == c-1) { path_exists = true; }

    //check the four directions for visitable nodes, update them, and add them
    //to the queue for later visitation

    //left
    if(cv > 0 && visited[rv][cv-1] == false && a[rv][cv-1] > a[rv][cv]+1) {
      a[rv][cv-1] = a[rv][cv]+1;
      visit_me.push_back({rv, cv-1});
    }
    
    //right
    if(cv < c-1 && visited[rv][cv+1] == false && a[rv][cv+1] > a[rv][cv]+1) {
      a[rv][cv+1] = a[rv][cv]+1;
      visit_me.push_back({rv, cv+1});
    }

    //up
    if(rv > 0 && visited[rv-1][cv] == false && a[rv-1][cv] > a[rv][cv]+1) {
      a[rv-1][cv] = a[rv][cv]+1;
      visit_me.push_back({rv-1, cv});
    }

    //down
    if(rv < r-1 && visited[rv+1][cv] == false && a[rv+1][cv] > a[rv][cv]+1) {
      a[rv+1][cv] = a[rv][cv]+1;
      visit_me.push_back({rv+1, cv});
    }

    //mark the value as visited, and erase
    visited[rv][cv] = true;
    visit_me.erase(visit_me.begin());
  }

  if(path_exists) { find_path_data(); }

  return path_exists;
}

//-----------------------------------------------------------------------------
//after a path is located by check_path_exists(), the data is passed into this
//function, which marks the shortest path and then marks the distances from
//that path
//note that this always assumes a viable path exists, and ends on the right
//side of the pather

void pather::find_path_data() {
  std::pair<int, int> loc = {0, c-1};

  int min = INT_MAX;
  //first, find the path's endpoint
  for(int i=0; i<r; i++) {
    if(a[i][c-1] > 0 && a[i][c-1] < min) {
      loc.first = i; 
      min = a[i][c-1];
    }
  }
  //load the end of the path and walk backward from it
  std::vector<std::pair<int,int>> visit_me{{loc.first, loc.second}};
  std::vector<std::pair<int, int>> visit_me_after;

  //walk backwards through the path, marking the route as we go
  while(visit_me.size() > 0) {
    int rv = visit_me[0].first;
    int cv = visit_me[0].second;

    visit_me_after.push_back({rv, cv});
    visit_me.erase(visit_me.begin());

    //look for the first path node of lesser value than this one, and then mark
    //this one as "visited"
    //(always looks in the order left > up > down > right)

    //left
    if(cv > 0 && a[rv][cv-1] > 1 && a[rv][cv-1] < a[rv][cv]) {
      visit_me.push_back({rv, cv-1});
      a[rv][cv] = 1;
      continue;
    }

    //up
    if(rv > 0 && a[rv-1][cv] > 1 && a[rv-1][cv] < a[rv][cv]) {
      visit_me.push_back({rv-1, cv});
      a[rv][cv] = 1;
      continue;
    }

    //down
    if(rv < r-1 && a[rv+1][cv] > 1 && a[rv+1][cv] < a[rv][cv]) {
      visit_me.push_back({rv+1, cv});
      a[rv][cv] = 1;
      continue;
    }

    //right
    if(cv < c-1 && a[rv][cv+1] > 1 && a[rv][cv+1] < a[rv][cv]) {
      visit_me.push_back({rv, cv+1});
      a[rv][cv] = 1;
      continue;
    }

    //no shorter paths found - this is the end
    loc = {rv, cv};
  }

  visit_me_after.push_back({loc.first, loc.second});
  a[loc.first][loc.second] = 1;



  //now, propagate outwards from the central path
  while(visit_me_after.size() > 0) {
    int rv = visit_me_after[0].first;
    int cv = visit_me_after[0].second;

    visit_me_after.erase(visit_me_after.begin());

    //check the four directions

    //left
    if(cv > 0 && a[rv][cv-1] > a[rv][cv]+1) {
      a[rv][cv-1] = a[rv][cv]+1;
      visit_me_after.push_back({rv, cv-1});
    }

    //right
    if(cv < c-1 && a[rv][cv+1] > a[rv][cv]+1) {
      a[rv][cv+1] = a[rv][cv]+1;
      visit_me_after.push_back({rv, cv+1});
    }

    //up
    if(rv > 0 && a[rv-1][cv] > a[rv][cv]+1) {
      a[rv-1][cv] = a[rv][cv]+1;
      visit_me_after.push_back({rv-1, cv});
    }

    //down
    if(rv < r-1 && a[rv+1][cv] > a[rv][cv]+1) {
      a[rv+1][cv] = a[rv][cv]+1;
      visit_me_after.push_back({rv+1, cv});
    }
  }
}

//=============================================================================
void pather::get_far_point(std::vector<p_o_i> & vals, int rank) const {
  get_far_point_v1(vals, rank);
}

//-----------------------------------------------------------------------------
//given a "rank", get the coordinates and distance of a point from the central
//path. 1 is the furthest from the path, 2 is second furthest, and so on.
//fills a provided empty vector with data.
//the return value is the coordinate pair (x, y) followed by the distance.
//if 2 points are equidistant, the one furthest to the bottom right is 
//given priority.
//a returned rank of -1 means there were not enough local maxima to satisfy
//the given number of ranks. 
void pather::get_far_point_v1(std::vector<p_o_i> & vals, int rank) const {
//  std::pair<int, std::pair<int, int>> vals[rank];

  //for safety, init values to 0
  for(int i=0; i<rank; i++) {
    vals.push_back({});
  }

  //find local maxima
  for(int i=0; i<r; i++) {
    for(int j=0; j<c; j++) {
      if(a[i][j] > 0 && a[i][j] < INT_MAX && is_local_max(i, j) && a[i][j] >= vals[0].rank) {
        vals[0].rank = a[i][j]; 
        vals[0].x = j;
        vals[0].y = i;
        std::sort(vals.begin(), vals.end());
//        std::cout << "added (" << j << "," << i << "):" << a[i][j] << " and sorted: ";
  //      for(auto v : vals) { std::cout << "(" << v.x << "," << v.y << "):" << v.rank << " "; }
        std::cout << std::endl;
      }
    }
  }
  return;
}

//-----------------------------------------------------------------------------
//given a coordinate pair, determine if it is a local maximum
//note that this assumes find_path_data() has run already
//
//a block of identical values regards its corners as "maximum"
//0, 1, and INT_MAX cannot qualify as maxima
bool pather::is_local_max(int x, int y) const {
  //no adjacent values greater than this one
  bool is_max = true;
  //more than 2 edges is a corner
  int edges = 0;

  if(a[x][y] == 0 || a[x][y] == 1 || a[x][y] == INT_MAX) { return false; }
  //check each adjacent value - NOTE THAT if 2 adjacent values are the same,
  //the lower right is considered the "local maximum"
  //TODO: add the above as a potentially toggleable feature

  //up
  if(x > 0 && a[x-1][y] > a[x][y]) { is_max = false; }
  if(x == 0 || a[x-1][y] == 0) { edges++; }

  //down
  if(x < r-1 && a[x+1][y] > a[x][y]) { is_max = false; }
  if(x == r-1 || a[x+1][y] == 0) { edges++; }

  //left
  if(y > 0 && a[x][y-1] > a[x][y]) { is_max = false; }
  if(y == 0 || a[x][y-1] == 0) { edges++; }

  //right
  if(y < c-1 && a[x][y+1] > a[x][y]) { is_max = false; }
  if(y == c-1 || a[x][y+1] == 0) { edges++;; }

  return (edges >= 2 && is_max);
}

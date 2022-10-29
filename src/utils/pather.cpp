#include "pather.h"
#include "rng.cpp" //TODO - correct this reference when testing is done!
#include <iostream>
#include <cmath>
#include <bits/stdc++.h>

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
//take a random, nonintersecting walk through the array, stopping when we hit
//the rightmost edge.
void pather::path(int start) {
  //this is the entity that "walks" around the map and draws the path
  //it starts by walking towards the right, then up/down, then left/right, and
  //so on. 
  //this is deterministic, and will always produce the same result when run 
  //with the same seed.
  //need to work on the actual algorithm though...
  if(start < 0 || start >= r) { throw("invalid pathing start point!"); }

  path_v3(start);
}

//-----------------------------------------------------------------------------
//zigzagging path, with extra lines thrown randomly in afterwards until a 
//desired "density" of walkable area is achieved (0.4 by default)
void pather::path_v3(int start, float density) {
  //the current "position" in the path box
  int loc[2] = {start, 0};
  //the direction to travel in
  int dir = 0;
  //the current direction's available travel distance
  int dist = 0;
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
  int h_min_l_cnst = 3;
  //horizontal min length (ratio)
  float h_min_l_rtio = 1/4;  
  //horizontal max length (constant)
  int h_max_l_cnst = INT_MAX;
  //horizontal max length (ratio)
  float h_max_l_rtio = 1/3;

  //vertical min height (constant)
  int v_min_h_cnst = 3;
  //vertical min height (ratio)
  float v_min_h_rtio = 1/3;
  //vertical max height (constant)
  int v_max_h_cnst = INT_MAX;
  //vertical max height (ratio)
  float h_max_h_rtio = 2/3;
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
  int trv = 0;
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

//=============================================================================
//dump the path to stdout
void pather::print() {
  std::cout << "dumping pather object\n";

  std::cout << "  ";
  for(int i=0; i<c; i++) {
    std::cout << i % 10 << " ";
  }
  std::cout << std::endl;

  for(int i=0; i<r; i++) {
    std::cout << i%10 << " ";
    for(int j=0; j<c; j++) {
      std::cout << (a[i][j] == 0 ? '-' : 'X') << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "density: " << std::trunc(get_density() * 1000) /10 << "%" << std::endl;
}

//=============================================================================
//get "density" - fraction of passable spaces occupied by a path
float pather::get_density() {
  float d = 0;
  for(int i=0; i<r; i++) {
    for(int j=0; j<c; j++) {
      if(a[i][j] != 0) { d++; }
    }
  }
  return(d / (r*c));
}

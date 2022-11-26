/*TODO:
2 deques, one for the active map and one for the old one, which gets replaced
"airlock" to signify when the prior map can be unloaded?
env needs to move safely to accommodate new deque
possibly fog of war - would mean just adding 1 airlock chunk to the end and transporting that to the beginning of the next
*/

#ifndef SLIDEMAP_H_
#define SLIDEMAP_H_

#include <deque>
#include "src/environment/chunk/chunk.h"
#include "src/environment/background/background.h"

class s_map {
public:
  s_map(std::string);
  ~s_map();

  void draw() const;

  //TODO: start chunk?

  const vec2d convert_chunk_index(const vec2d &pos) const;

  //check boundaries of current chunk, then compare src and dst to see if any
  //barriers are crossed
  unsigned char check_rebuff(vec2d &curr_pos, vec2d &prev_pos) const;

  //TODO: jump gates?

  //return a certain chunk
  chunk &get_chunk(vec2d coord);

  //TODO: entity spawning, initial and closet

private:

/*
  the slidemap is made of 2 "maps" - new and old. initially, only the new map
  exists; when the player find the keygate, they build a new map that gets
  stuck onto the side of the existing one, extending it. this then becomes the
  "new" map, with their current one now "old". the old map becomes poisonous, 
  encouraging exploration, and is deleted when the next keygate is found. 

  the two deques flip back and forth; when the new map's keygate is accessed,
  the old deque is cleared and repurposed into the new one, with the 
  understanding that the player won't (or can't) proceed backwards. 
*/
  std::deque<chunk> c_deque_1, c_deque_2;

  b_ground bg;

  

};

#endif

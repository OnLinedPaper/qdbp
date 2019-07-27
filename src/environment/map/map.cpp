#include <deque>
#include <vector>
#include <sstream>
#include "map.h"
#include "src/xml_parser/xmlparse.h"
#include "src/utils/message.h"

map::map(std::string name) {
  x_dim = xmlparse::get().get_xml_int(name + "/dimensions/x_dim");
  y_dim = xmlparse::get().get_xml_int(name + "/dimensions/y_dim");
  start_chunk = vec2d(
    xmlparse::get().get_xml_int(name + "/start_chunk/x_chunk"),
    xmlparse::get().get_xml_int(name + "/start_chunk/y_chunk")
  );

  validate();

  //init c_deque, now that we have dims
  init_c_deque();
}

map::~map() { }

void map::init_c_deque() {
  //reserve space
  //c_map.reserve(x_dim * y_dim);
  int up, dn, lf, rt;
  up = dn = lf = rt = 0;

  for(int j=0; j<y_dim; j++) {
    for(int i=0; i<x_dim; i++) {
      up = dn = lf = rt = 0;
      //check for barriers
      if(i == 0) { lf = 1; }
      if(i == x_dim - 1) { rt = 1; }
      if(j == 0) { up = 1; }
      if(j == y_dim - 1) { dn = 1; }


      c_deque.emplace_back(i, j, up, dn, lf, rt);
    }
  }
}

vec2d map::get_start_pos() const {
  return(c_deque[index(start_chunk[0], start_chunk[1])].get_midpoint());
}

vec2d map::convert_chunk_index(vec2d &pos) const {
  return(vec2d(int(pos[0] / chunk::length), int(pos[1] / chunk::length)));
}

unsigned char map::check_rebuff(vec2d &curr_pos, vec2d &prev_pos) const {
  //check the last chunk that was occupied, and return the position
  //relative to that chunk

  vec2d index_pos = convert_chunk_index(prev_pos);
  return(
    c_deque[index(
      index_pos[0],
      index_pos[1]
    )].chunk_rebuff(curr_pos)
  );
}

void map::draw() const {
  //delegate this to each chunk's draw
  for(int j=0; j<y_dim; j++) {
    for(int i=0; i<x_dim; i++) {
      c_deque[index(i, j)].draw();
    }
  }
}

void map::validate() {
  std::ostringstream errors;

  if(x_dim <=0 ) {
    msg::print_warn("bad map dimension; x_dim must be > 0");
    msg::print_alert("setting to 1");
    x_dim = 1;
  }
  if(y_dim <= 0) {
    msg::print_warn("bad map dimension; y_dim must be > 0");
    msg::print_alert("setting to 1");
    y_dim = 1;
  }
  if(start_chunk[0] <= -1 || start_chunk[0] >= x_dim) {
    msg::print_warn("bad start position; x_chunk " +
    std::to_string(start_chunk[0]) +
    " is not in map - range is 0 to " +
    std::to_string((x_dim - 1)));
    msg::print_alert("setting to 0");
    start_chunk[0] = 0;
  }
  if(start_chunk[1] <= -1 || start_chunk[1] >= y_dim) {
    msg::print_warn("bad start position; y_chunk " +
    std::to_string(start_chunk[1]) +
    " is not in map - range is 0 to " +
    std::to_string((y_dim - 1)));
    msg::print_alert("setting to 0");
    start_chunk[1] = 0;
  }

}

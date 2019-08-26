#include <deque>
#include <vector>
#include <sstream>
#include <string>
#include "map.h"
#include "src/xml_parser/xmlparse.h"
#include "src/utils/message.h"

map::map(std::string n) :
  bg(xmlparse::get().get_xml_string(n + "/background")),
  name(n)
{
  x_dim = xmlparse::get().get_xml_int(name + "/dimensions/x_dim");
  y_dim = xmlparse::get().get_xml_int(name + "/dimensions/y_dim");
  start_chunk = vec2d(
    xmlparse::get().get_xml_int(name + "/start_chunk/x_chunk"),
    xmlparse::get().get_xml_int(name + "/start_chunk/y_chunk")
  );

  validate();

  //init c_deque, now that we have dims
  init_c_deque();
  //initiate any special chunks
  init_special_chunks();
}

map::~map() { }

void map::init_c_deque() {
  //reserve space
  //c_map.reserve(x_dim * y_dim);
  int up, dn, lf, rt;
  up = dn = lf = rt = 0;

  std::string default_type = "";

  if(xmlparse::get().get_xml_string(
    name + "/generation/style").compare("userdef") == 0
  ) {
    default_type = xmlparse::get().get_xml_string(name+"/generation/default_chunk_type");
    //this is a userdef map
  }

  for(int j=0; j<y_dim; j++) {
    for(int i=0; i<x_dim; i++) {
      up = dn = lf = rt = 0;
      //check for barriers
      if(i == 0) { lf = 1; }
      if(i == x_dim - 1) { rt = 1; }
      if(j == 0) { up = 1; }
      if(j == y_dim - 1) { dn = 1; }


      c_deque.emplace_back(i, j, up, dn, lf, rt, default_type);
    }
  }
}

void map::init_special_chunks() {
  //make path, for convenience's sake
  std::string s_path = name + "/special_chunks";

  //get the special chunks
  std::vector<std::string> specials = xmlparse::get().get_all_child_tags(s_path);

  //some variables we'll use
  int x = 0;
  int y = 0;
  std::string t = "";

  //some constants for the first function
  bool f = false;

  for(std::string s_c_name : specials) {
    x = xmlparse::get().get_xml_int(s_path + "/" + s_c_name + "/x_chunk");
    y = xmlparse::get().get_xml_int(s_path + "/" + s_c_name + "/y_chunk");
    t = xmlparse::get().get_xml_string(s_path + "/" + s_c_name + "/type");

    if(x < 0 || x >= x_dim || y < 0 || y >= y_dim) {
      //this chunk isn't valid - skip it
      msg::print_warn("bad coordinate for special chunk " + s_path + "/" +
        s_c_name + " (" + std::to_string(x) + "," + std::to_string(y) + ")");
      msg::print_alert("valid x coords: (0 to " + std::to_string(x_dim-1) +
        "); valid y coords: (0 to " + std::to_string(y_dim-1) + ")");
    }
    else {
      //replace a chunk with this new one
      c_deque[index(x, y)] = chunk(x, y, f, f, f, f, t);

      //check the barriers
      check_barriers(x, y);
    }
  }
}

void map::check_barriers(int x, int y) {
  //a change has been made to this chunk - check all nearby chunks
  //and add or remove barriers as needed

  //first, check to see if this is on the edge of the map
  //if it's in bounds, add a barrier - else, don't
  if(x == 0) { c_deque[index(x, y)].set_bound_b_lf(); }
  if(y == 0) { c_deque[index(x, y)].set_bound_b_up(); }
  if(x == x_dim-1) { c_deque[index(x, y)].set_bound_b_rt(); }
  if(y == y_dim-1) { c_deque[index(x, y)].set_bound_b_dn(); }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //next, check to see if existing chunks need their barriers modified
  //use XOR (^) to check them
  //- if both are in or out of bounds- no barriers
  //- if one is and one isn't- set it accordingly

  //check left chunk
  if(x > 0) {
    if(c_deque[index(x, y)].get_in_bounds() ^ c_deque[index(x-1, y)].get_in_bounds()) {
      //they are not the same
      c_deque[index(x, y)].set_bound_b_lf();
      c_deque[index(x-1, y)].set_bound_b_rt();

    }
    else {
      //they are the same - remove barriers
      c_deque[index(x, y)].set_b_lf(false);
      c_deque[index(x-1, y)].set_b_rt(false);
    }
  }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //check right chunk
  if(x < x_dim - 1) {
    //check right chunk
    if(c_deque[index(x, y)].get_in_bounds() ^ c_deque[index(x+1, y)].get_in_bounds()) {
      //they are not the same
      c_deque[index(x, y)].set_bound_b_rt();
      c_deque[index(x+1, y)].set_bound_b_lf();

    }
    else {
      //they are the same - remove barriers
      c_deque[index(x, y)].set_b_rt(false);
      c_deque[index(x+1, y)].set_b_lf(false);
    }
  }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //check up chunk
  if(y > 0) {
    if(c_deque[index(x, y)].get_in_bounds() ^ c_deque[index(x, y-1)].get_in_bounds()) {
      //they are not the same
      c_deque[index(x, y)].set_bound_b_up();
      c_deque[index(x, y-1)].set_bound_b_dn();

    }
    else {
      //they are the same - remove barriers
      c_deque[index(x, y)].set_b_up(false);
      c_deque[index(x, y-1)].set_b_dn(false);
    }
  }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //check down chunk
  if(y < y_dim - 1) {
    //check right chunk
    if(c_deque[index(x, y)].get_in_bounds() ^ c_deque[index(x, y+1)].get_in_bounds()) {
      //they are not the same
      c_deque[index(x, y)].set_bound_b_dn();
      c_deque[index(x, y+1)].set_bound_b_up();

    }
    else {
      //they are the same - remove barriers
      c_deque[index(x, y)].set_b_dn(false);
      c_deque[index(x, y+1)].set_b_up(false);
    }
  }


  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
}

const vec2d map::get_start_pos() const {
  //check if cunk is in bounds, or find one if it isn't

  if(c_deque[index(start_chunk[0], start_chunk[1])].get_in_bounds()) {
    //it's in bounds
    return(c_deque[index(start_chunk[0], start_chunk[1])].get_midpoint());
  }
  else {
    //it's NOT in bounds
    msg::print_warn("start chunk (" + std::to_string((int)start_chunk[0]) + "," + std::to_string((int)start_chunk[1]) + ") is out of bounds");

    //find one that's in bounds
    for(int j=0; j<y_dim; j++) {
      for(int i=0; i<x_dim; i++) {
        if(c_deque[index(i, j)].get_in_bounds()) {
          //this chunk is in bounds

          return(c_deque[index(i, j)].get_midpoint());
        }
      }
    }

    //no in-bounds chunks were found
    std::string error = name + " has no chunks that are in bounds!";
    msg::print_error(error);
    throw(error);
  }
}

const vec2d map::convert_chunk_index(vec2d &pos) const {
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
  //draw the bg first
  bg.draw();

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

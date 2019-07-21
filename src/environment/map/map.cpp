#include <deque>
#include "map.h"
#include "src/xml_parser/xmlparse.h"

map::map(std::string name) {
  x_dim = xmlparse::get().get_xml_int(name + "/dimensions/x_dim");
  y_dim = xmlparse::get().get_xml_int(name + "/dimensions/y_dim");
  start_chunk = vec2d(
    xmlparse::get().get_xml_int(name + "/start_chunk/x_chunk"),
    xmlparse::get().get_xml_int(name + "/start_chunk/y_chunk")
  );


  //init c_map, now that we have dims
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

void map::draw() const {
  //delegate this to each chunk's draw
  for(int j=0; j<y_dim; j++) {
    for(int i=0; i<x_dim; i++) {
      c_deque[index(i, j)].draw();
    }
  }
}

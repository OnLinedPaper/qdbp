#include "hybrid_box.h"
#include "src/xml_parser/xmlparse.h"

hy_box::hy_box(float s, vec2d &offs, int t) :
  size(s),
  b(s, offs, t),
  l({0, 0}, {0, 0}),
  line_active(false)
{ }

hy_box::hy_box(std::string path) :
  size(xmlparse::get().get_xml_float(path + "/size")),
  b(path),
  l({0, 0}, {0, 0}),
  line_active(false)
{ } 

void hy_box::calibrate() {
  //"calibrate" the line to the box's current position

    l.set_start(((b.get_tlc() + b.get_brc()) / 2));  
    l.set_end(((b.get_tlc() + b.get_brc()) / 2));  
}

void hy_box::set_box_center(vec2d &pos, float last_angle) {
  //check to see if the hitbox has moved more than its size, and trail a
  //line if it has

  //save previous pos
  vec2d prev_pos(((b.get_tlc() + b.get_brc()) / 2)); 

  //set new pos from hitbox functions
  b.set_box_center(pos, last_angle);

  //check distance
  if(b.get_tlc().dist(prev_pos) > size) {
    line_active = true;
  }
  else {
    line_active = false;
  }
    l.set_start(((b.get_tlc() + b.get_brc()) / 2));  
    l.set_end(prev_pos);


}

//TODO
bool hy_box::collides(const hy_box &hy) const { return false; }
bool hy_box::collides(const hitbox &hi) const { return false; }
bool hy_box::collides(const hitline &li) const { return false; }

void hy_box::draw() const {
  b.draw();
  if(line_active) { l.draw(); }
}

std::ostream &operator<<(std::ostream &output, const hy_box &hy) {
  output << "tlc: (" << hy.get_b().get_tlc()[0] << ", " << hy.get_b().get_tlc()[1] << ")\tbox size: " << hy.get_b().get_dims()[0]  << "\tsize var: " << hy.get_s() << "\ttype: " << hy.get_b().type_to_str(hy.get_b().get_type()) << "\n- line: " << (hy.get_la() ? "active " : "inactive ") << "\tstart: " << hy.get_l().get_start() << "\tend: " << hy.get_l().get_end() << "\tlength: " << hy.get_l().get_start().dist(hy.get_l().get_end());
 
  return output;
}

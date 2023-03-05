
#include "src/xml_parser/xmlparse.h"
int main(void) {
  xmlparse::get().build_tree("resources/gamedata.xml");
 
  return 0;
}

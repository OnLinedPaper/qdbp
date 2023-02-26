#include "main_driver.h"

//-
//TODO: remove once the stack overflow is gone
#include "src/xml_parser/xmlparse.h"
#include <iostream>
//-

int main(void) {
//-
  //TODO: remove once the stack overflow is gone
  std::cout << "building" << std::endl;
  xmlparse::get().build_tree("resources/gamedata.xml");
  std::cout << "built" << std::endl;
  return 0;
//-

  engine main_engine;
  main_engine.play();

  return 0;
}

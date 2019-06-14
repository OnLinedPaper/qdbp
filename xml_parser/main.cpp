#include "xmlnode.h"
#include "xmlparse.h"
using xmlp = xmlparse;
#include <string>
#include <iostream>

int main(void) {


  xmlp::get().build_tree("example.xml");
  xmlp::get().build_tree("ex.xml");


  try {
    std::cout << "got: " << xmlp::get().get_xml_float("/data/stuff/unique/") << std::endl;
  }
  catch (const std::string e) {
    std::cout << e;
  }

  xmlp::get().print_tree();
}

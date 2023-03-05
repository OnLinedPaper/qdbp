#include <iostream>
#include <vector>
#include <string>
#include "src/xml_parser/xmlparse.h"
int main(void) {
  std::cout << "start test" << std::endl;

  xmlparse::get();
  xmlparse::get().set_dbg_msg_flags(xmlparse::DBG_ALL);
  xmlparse::get().build_tree("input.xml");

  std::cout << (xmlparse::get().check_path("/one/three") ? "exists" : "does not exist") << std::endl;

  std::vector<std::string> v;
  xmlparse::get().get_all_child_tags("/one", v);
  for(const std::string &s  : v) {
    std::cout << s << std::endl;
  }

  std::cout << std::endl;

  xmlparse::get().print_tree();

  std::cout << std::endl;

  std::string path = "/five/six/seven";
  std::cout << xmlparse::get().get_xml_bool(path) << std::endl;

/*
//basic xmlnode functions
  xmlnode n;
  n.set_value("value");

  n.add_child("one");
  n.add_child("two");
  n.add_child("three");

  n.get_child("one")->set_value("one value");

  std::cout << n.get_child("one")->get_name() << std::endl;
  std::cout << n.get_child("one")->get_value() << std::endl;

  std::cout << n.get_child("fake") << std::endl;

  std::vector<std::string> v;
  n.get_children(v);

  for(std::string s : v) {
    std::cout << s << " ";
  }
  std::cout << std::endl;
*/

  return 0;
}

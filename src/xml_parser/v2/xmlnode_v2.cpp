#include "src/xml_parser/v2/xmlnode_v2.h"

xmlnode::xmlnode() : name(""), value(""), children() { }
xmlnode::xmlnode(std::string name) : name(name), value(""), children() {}
xmlnode::xmlnode(std::string name, std::string value) : name(name), value(value), children() {}

xmlnode::~xmlnode() {
  std::map<std::string, xmlnode*>::iterator it = children.begin();
  while(it != children.end()) {
    //recursively free all children of this element

    delete it->second;

    it++;
  }
}

xmlnode * xmlnode::add_child(const std::string &n) {
  children.insert(std::make_pair(n, new xmlnode(n)));
  return children[n];
}

xmlnode * xmlnode::get_child(const std::string &n) {
  if(children.count(n) == 0) { return NULL; }

  return children[n];
}

void xmlnode::get_children(std::vector<std::string> &v) {
  for(auto const &c : children) {
    v.push_back(c.first);
  }  
}

#include "xmlnode.h"
#include <iostream>

xmlnode::xmlnode() : name(""), value(""), children() { }
xmlnode::xmlnode(std::string name) : name(name), value(""), children() {}
xmlnode::xmlnode(std::string name, std::string value) : name(name), value(value), children() {}

xmlnode::~xmlnode() {
//  std::cerr << "xmlnode delete called\n";
  std::map<std::string, xmlnode*>::iterator it = children.begin();
  while(it != children.end()) {
    //recursively free all children of this element

    delete it->second;

    it++;
  }
}

//=============================================================================

void xmlnode::add_child(const std::string n) {
  children.insert(std::make_pair(n, new xmlnode(n)));
}

//=============================================================================

void xmlnode::insert_child(const std::string name, const std::string path) {
  this->insert_child(name, path, "");
}

//=============================================================================

void xmlnode::insert_child(const std::string name, const std::string path, const std::string value) {
  if(path == ("/")) {
    //this is where we insert
    this->add_child(name);
    this->children[name]->set_value(value);
  }
  else {
    //dive deeper

    //cut the first "/"
    std::string p = path.substr(1, std::string::npos);
    //get the new node's name
    std::string n = p.substr(0, p.find_first_of("/"));
    //get the new path
    p = p.substr(p.find_first_of("/"), std::string::npos);

    //recursively insert till we insert into the node we want
    this->children[n]->insert_child(name, p, value);
  }
}

//=============================================================================

std::string xmlnode::recursive_get_value(const std::string path) {
  try {
    if(path == ("")) {
      //this is where we get value
      return this->get_value();
    }
    else {
      //dive deeper

      //cut the first "/"
      std::string p = path.substr(1, std::string::npos);
      //get the new node's name
      std::string n = p.substr(0, p.find_first_of("/"));
      //get the new path
      std::size_t found = p.find_first_of("/");
      if(found != std::string::npos) {
        //there's still another "tag"
        p = p.substr(found, std::string::npos);
      }
      else {
        //this is it, it's the value we want
        p = "";
      }

      //check to see if this is a child
      if(this->children.find(n) == this->children.end()) {
        //it's not a child
        std::string error = "error! \"" + n + "\" is not a child of \"" + this->get_name() + "\"!\n";
        throw error;
      }
      else {
        //it's a valid child, go deeper
        return(this->children[n]->recursive_get_value(p));
      }
    }
  }
  catch (const std::string e) {
    throw;
  }
}

//=============================================================================

void xmlnode::recursive_print(const int depth) {
  //print out this node's name and value, followed by each child

  //print some spaces to give this a "tree-like" look
  for(int i=0; i< depth; i++) { std::cout << " "; }

  if(this->get_value() != "") {
    std::cout << "name: \"" << this->get_name() << "\" | value: \"" <<
      this->get_value() << "\"" << std::endl;
  }
  else {
    std::cout << "name: \"" << this->get_name() << "\"" << std::endl;
  }

  std::map<std::string, xmlnode*>::iterator it = children.begin();
  while(it != children.end()) {
    //recursive print call on child
    it->second->recursive_print(depth + 1);
    it++;
  }
}

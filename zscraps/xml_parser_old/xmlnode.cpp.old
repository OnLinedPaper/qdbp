#include "xmlnode.h"
#include "src/utils/message.h"
#include <iostream>
#include <map>

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

bool xmlnode::recursive_check_path(const std::string path, bool send_alert) {
  //check to see if a path is valid, optionally throwing errors if it's not
  if(path == "") {
    return true;
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

    if(this->children.find(n) == this->children.end()) {
      //it's not a child

      if(send_alert) {
        std::string name = this->get_name();
        std::string alert = "";
        if(name == "") {
          alert ="\"" + n + "\" is not in the xml tree!";
        }
        else {
          alert = "\"" + n + "\" is not a child of \"" + this->get_name() + "\"!";
        }
        msg::print_alert(alert);
      }

      return false;
    }
    else {
      //it's a valid child, go deeper
      return(this->children[n]->recursive_check_path(p, send_alert));
    }
  }
}

//=============================================================================

std::string xmlnode::recursive_get_value(const std::string path, bool send_alert) {
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
        std::string name = this->get_name();
        std::string error = "";
        if(name == "") {
          error ="\"" + n + "\" is not in the xml tree!";
        }
        else {
          error = "\"" + n + "\" is not a child of \"" + this->get_name() + "\"!";
        }
        std::string alert = "(might be misspelled, or have non-matching open and close tags)";
 
        if(send_alert) {
          msg::print_error(error);
          msg::print_alert(alert);
        }
        throw error;
      }
      else {
        //it's a valid child, go deeper
        return(this->children[n]->recursive_get_value(p, send_alert));
      }
    }
  }
  catch (const std::string e) {
    throw;
  }
}

//=============================================================================

std::string xmlnode::dive_name(const std::string path) const {

  //cut the first "/" we find, and then return the name left over
  std::string p = path.substr(1, std::string::npos);
  //get the new node's name
  return(p.substr(0, p.find_first_of("/")));
}

//=============================================================================

std::string xmlnode::dive_path(const std::string path) const {

  //cut the first "/"
  std::string p = path.substr(1, std::string::npos);
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

  return(p);
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

//=============================================================================

void xmlnode::recursive_file_print(std::ofstream &f, int depth) {
  //print out this node's name and value, followed by each child

  //print some spaces to give this a "tree-like" look
  for(int i=0; i< depth; i++) { f << "  "; }

  bool no_value = false;

  if(!this->get_name().empty() && this->get_value() != "") {
    f << "<" << this->get_name() << ">" <<
      this->get_value() << "</" << this->get_name() << ">\n";
  }
  else if(!this->get_name().empty()) {
    f << "<" << this->get_name() << ">\n";
    no_value = true;
  }

  std::map<std::string, xmlnode*>::iterator it = children.begin();
  while(it != children.end()) {
    //recursive print call on child
    it->second->recursive_file_print(f, depth + 1);
    it++;
  }
  
  if(!this->get_name().empty() && no_value) {
    //print the closing tag
    for(int i=0; i< depth; i++) { f << "  "; }
    f << "</" << this->get_name() << ">\n";
  }
}

//=============================================================================

std::vector<std::string> xmlnode::recursive_get_all_child_tags(const std::string path) {
  if(path == "") {
    //get the children of this node
    std::vector<std::string> v;

    //iterate through children
    for(auto const &it : this->children) {
      //add each child's name
      v.push_back(it.first);
    }

    return v;
  }
  else {
    //dive deeper
    if(children.find(dive_name(path)) != children.end()) {
      return(this->children[dive_name(path)]->recursive_get_all_child_tags(dive_path(path)));
    }
    else {
      std::string warn = "could not find children for " + dive_name(path);
      msg::print_warn(warn);
      throw(warn);
    }
  }
}

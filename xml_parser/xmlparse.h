#ifndef XMLPARSE_H_
#define XMLPARSE_H_

#include "xmlnode.h"
#include <regex>
#include <string>

class xmlparse {
public:
  xmlparse(const xmlparse&) = delete;
  xmlparse &operator=(const xmlparse&) = delete;
  ~xmlparse();

  static xmlparse &get() {
    static xmlparse instance;
    return instance;
  }

  void build_tree(const std::string file_name); //make the tree
  void recurse_build_tree(const std::smatch, const std::string);

  void print_tree() { root->recursive_print(0); }

  std::string get_xml_string(const std::string path);
  int get_xml_int(const std::string path);
  float get_xml_float(const std::string path);

private:
  //singleton

  xmlparse();

  xmlnode *root;

  //for getting a "tree" of tags - found in tree_regex.rx
  std::regex tree_rx;
  //for checking for a value inside a tag - found in tag_regex.rx
  std::regex tag_rx;
  //for getting the value inside a tag - found in value_regex.rx
  std::regex value_rx;

};


#endif

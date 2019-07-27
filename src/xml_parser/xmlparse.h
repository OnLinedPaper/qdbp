#ifndef XMLPARSE_H_
#define XMLPARSE_H_

#include "xmlnode.h"
#include <regex>
#include <string>

class xmlparse {
public:
  ~xmlparse();

  static xmlparse &get() {
    static xmlparse instance;
    return instance;
  }

  void build_tree(const std::string file_name); //make the tree
  void recurse_build_tree(const std::smatch, const std::string);

  void print_tree();

  std::string get_xml_string(const std::string path);
  int get_xml_int(const std::string path);
  double get_xml_double(const std::string path);

private:
  //singleton

  xmlparse();
  xmlparse(const xmlparse&) = delete;
  xmlparse &operator=(const xmlparse&) = delete;

  xmlnode *root;

  //for getting a "tree" of tags - previously found in tree_regex.rx
  const std::regex tree_rx =
    std::regex("<([^/].*?)>(.|\\n)*?</\\1>");
  //for checking for a value inside a tag - previously found in tag_regex.rx
  const std::regex tag_rx =
    std::regex("^\\s*<([^/].*?)>((?!<.*>).)+</\\1>\\s*$");
  //for getting the value inside a tag - previously found in value_regex.rx
  std::regex value_rx =
    std::regex(">.*?<");

};


#endif

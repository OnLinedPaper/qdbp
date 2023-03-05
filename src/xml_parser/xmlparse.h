#ifndef XMLPARSE_H_
#define XMLPARSE_H_

#include "src/xml_parser/xmlnode.h"
#include <regex>
#include <string>

#ifdef XML_TOOL_
#include <fstream>
#endif

class xmlparse {
public:
  ~xmlparse();

  static xmlparse &get() {
    static xmlparse instance;
    return instance;
  }

  void build_tree(const std::string file_name); //make the tree
  void recurse_build_tree(const std::smatch, const std::string);
  void destroy_tree() { if(root) {delete root; root = new xmlnode();} };

  void print_tree();
  std::vector<std::string> get_all_child_tags(const std::string path) const;
  bool check_path(const std::string path, bool send_alert) const;

  void set_show_debug_msg(bool b) { show_debug_msg = b; }

  std::string get_xml_string(const std::string path);
  std::string safe_get_xml_string(const std::string path, std::string def = "");

  int get_xml_int(const std::string path);
  int safe_get_xml_int(const std::string path, int def = 0);

  float get_xml_float(const std::string path);
  float safe_get_xml_float(const std::string path, float def = 0);

  bool get_xml_bool(const std::string path);
  bool safe_get_xml_bool(const std::string path, bool def = false);


#ifdef XML_TOOL_
  //DO NOT USE in normal programming. this is for the xml builder tool's use ONLY.
  xmlnode *get_root() { return root; }
  void print_as_file(std::ofstream &f) { root->recursive_file_print(f, -1); }
#endif

private:
  //singleton

  xmlparse();
  xmlparse(const xmlparse&) = delete;
  xmlparse &operator=(const xmlparse&) = delete;

  xmlnode *root;

  bool show_debug_msg;

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

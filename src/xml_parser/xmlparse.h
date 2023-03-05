#ifndef XML_HANDLER_
#define XML_HANDLER_
#include "src/xml_parser/xmlnode.h"

//second attempt at an xml tool. this will take much more of an active role
//on actual xml behaviors than the previous one did, and will not use ANY
//recursion this time. 
class xmlparse {
public:
  ~xmlparse();
  
  static xmlparse &get() {
    static xmlparse instance;
    return instance;
  }

  static const uint16_t DBG_ALL;
  static const uint16_t DBG_DEFAULT;
  static const uint16_t DBG_ONLY_FATAL;
  static const uint16_t DBG_TREE_PRINT;
  static const uint16_t DBG_FATAL_ERROR;
  static const uint16_t DBG_TREE_BUILD;
  static const uint16_t DBG_GET_FAIL;
  static const uint16_t DBG_PATH_FAIL;
  static const uint16_t DBG_NONE;

  //given a file name, build an entire xml tree based on the data inside
  void build_tree(const std::string &file_name);
  //reset the tree and build a new one
  void destroy_tree() { if(root) { delete root; root = new xmlnode();} }

  //print the tree's contents to stdout
  void print_tree();

  //go to a specific child in the tree and then get all of its children
  //(note that these are put into a vector)
  //note: will throw an exception if the path is not viable! be sure to check 
  //with check_path first if this would be a problem. 
  //note: does not guarantee tags are returned in any particular order.
  void get_all_child_tags(const std::string &, std::vector<std::string> &);

  //check the validity of a given tag path
  bool check_path(const std::string &path);

  //get an xml value. 
  //if safe_mode is false, will throw an exception if the requested xml path 
  //is not in the tree
  //if safe_mode is true, returns the default value and warns the user via 
  //stderr that the path was not found
  std::string get_xml_string(const std::string &path, bool safe_mode = false, std::string def = "");
  int get_xml_int(const std::string &path, bool safe_mode = false, int def = 0);
  float get_xml_float(const std::string &path, bool safe_mode = false, float def = 0.0);
  bool get_xml_bool(const std::string &path, bool safe_mode = false, bool def = false);

  uint16_t get_dbg_msg_flags() { return dbg_msg_flags; }
  void set_dbg_msg_flags(uint16_t i); 

private:
  xmlparse();
  xmlparse(const xmlparse &) = delete;
  xmlparse &operator=(const xmlparse&) = delete;

  //returns a pointer to a given child node
  //note that this is for INTERNAL USE ONLY. external code should have no
  //reason to access an xml node, just its values
  //returns NULL if child does not exist
  xmlnode *get_child(const std::string &);

  //all 4 get functions are nearly identical - prints out their messages
  void print_get_bad_path_msg(const std::string &func_name, const std::string &path, bool safe_mode, const std::string &def);
  void print_get_bad_value_msg(const std::string &func_name, const std::string &path, const std::string &value, bool safe_mode, const std::string &def);

  xmlnode *root;

  uint16_t dbg_msg_flags; 
};

#endif

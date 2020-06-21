#include "xmlparse.h"
#include "src/utils/message.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <regex>
#include <fstream>

xmlparse::xmlparse() : root(new xmlnode()) { }

xmlparse::~xmlparse() {
  delete root;
}

//============================================================================

void xmlparse::build_tree(const std::string file_name) {


  //get the xml file
  std::FILE *fp = std::fopen(file_name.c_str(), "rb");
  if(!fp) {
    msg::print_warn("when bulding tree, file \"" + file_name + "\" couldn't be opened!");
    msg::print_alert("(did you rename / misspell / delete the file by mistake?)");
    return;
  }

  //get the size of the string buffer, allocate that memory, and
  //fill it in
  std::string contents = "";
  std::fseek(fp, 0, SEEK_END);

  //make it this big
  contents.resize(std::ftell(fp));

  //go the fromt and read everything into the string
  std::rewind(fp);
  size_t read = 0;
  read = std::fread(&contents[0], 1, contents.size(), fp);
  if(read != contents.size()) {
    msg::print_warn("xmlparser expected " + std::to_string(contents.size()) + ", read " + std::to_string(read));
  }

  std::fclose(fp);

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//we have the contents, now load them into the nodes

  //iterator to go through the string
  std::sregex_iterator rxi(contents.begin(), contents.end(), tree_rx);
  std::sregex_iterator end;

  while(rxi != end) {
    std::smatch m = *rxi;
    recurse_build_tree(m, "/");
    rxi++;
  }
}

//============================================================================

std::string xmlparse::get_xml_string(const std::string path) {
  try {
    return(root->recursive_get_value(path));
  }
  catch (const std::string e) {
    //something went wrong, add some data and throw the exception
    msg::print_alert("bad tag path: " + path);
    std::string error = e + " bad tag path: " + path;

    throw error;
  }
}

//============================================================================

int xmlparse::get_xml_int(const std::string path) {
  std::string retval = get_xml_string(path);
  try {
    return std::stoi(retval);
  }
  catch (const std::invalid_argument& ia) {
    //they probably tried to convert a string or something
    std::string error = "can't convert this value to an int!";
    msg::print_error(error);
    msg::print_alert("value: " + retval);
    msg::print_alert("bad tag path: " + path);
    throw error;
  }
}

//============================================================================

float xmlparse::get_xml_float(const std::string path) {
  std::string retval = get_xml_string(path);
  try {
    return std::stod(retval);
  }
  catch (const std::invalid_argument& ia) {
    //they probably tried to convert a string or something
    std::string error = "can't convert this value to a float!";
    msg::print_error(error);
    msg::print_alert("value: " + retval);
    msg::print_alert("bad tag path: " + path);
    throw error;
  }
}

//============================================================================

float xmlparse::safe_get_xml_float(const std::string path) {
  try {
    return get_xml_float(path);
  }
  catch (std::string e) {
    msg::print_good("==== safe mode: returning 0 ====");
    return 0;
  }
}

//============================================================================

bool xmlparse::get_xml_bool(const std::string path) {
  std::string retval = get_xml_string(path);

  std::regex t("^[Tt][Rr][Uu][Ee]$");
  std::regex f("^[Ff][Aa][Ll][Ss][Ee]$");

  if(std::regex_match(retval, t)) {
    return true;
  }
  else if(std::regex_match(retval, f)) {
    return false;
  }
  else {
    std::string error = "can't convert this value to bool!";
    msg::print_error(error);
    msg::print_alert("value: " + retval);
    msg::print_alert("bad tag path: " + path);
    throw error;
  }
}

bool xmlparse::check_path(const std::string path, bool send_alert) const {
  return(root->recursive_check_path(path, send_alert));
}

void xmlparse::print_tree() {
  std::cout << msg::cn << std::endl;
  root->recursive_print(0);
  std::cout << msg::none << std::endl;
}

std::vector<std::string> xmlparse::get_all_child_tags(const std::string path) const {
  try {
    return(root->recursive_get_all_child_tags(path));
  }
  catch (std::string s) {
    //one of the tags was bad
    msg::print_alert("bad tag path: " + path);
    return std::vector<std::string>();
  }
}

//============================================================================

void xmlparse::recurse_build_tree(const std::smatch m, const std::string path) {
  //get the string first that holds this regex chunk
  std::string content = m.str();

  std::sregex_iterator rxi;
  std::sregex_iterator end;
  unsigned first = 0;
  unsigned last = 0;

  bool is_comment = true;

  while(is_comment) {
    is_comment = false;

    //get the top tag - note that it is expected there are
    //no children whose names are duplicated by one of
    //their parents
    first = content.find_first_of("<") + 1;
    last = content.find_first_of(">");

    //check first to see if this is a comment - if so,
    //ignore it
    if(
      !content.substr(first, 3).compare("!--") &&
      !content.substr(last-2, 2).compare("--")
    ) {
      //it's a comment
      //erase it and try again
      is_comment = true;

      content = content.erase(first-1, last-first+2);
    }
  }

  std::string top_tag = content.substr(first, last-first);
  std::string open_tag = "<" + top_tag + ">";
  std::string close_tag = "</" + top_tag + ">";

  //cut the start and end tags off
  first = content.find_first_of(open_tag) + 1;
  last = content.find_last_of(close_tag);
  std::string new_content = content.substr(first, last-first);

  rxi = std::sregex_iterator(content.begin(), content.end(), tag_rx);

  if(rxi != end) {
    //this is a tag with only a value in it

    //trim the value out
    first = content.find_first_of(">") + 1;
    last = content.find_last_of("<");
    std::string value = content.substr(first, last-first);

    std::smatch m = *rxi;
    //insert the value
    root->insert_child(top_tag, path, value);
  }
  else {

    //the tag has some html in it
    root->insert_child(top_tag, path);

    //look for more matches
    rxi = std::sregex_iterator(new_content.begin(), new_content.end(), tree_rx);

    while(rxi != end) {
      std::smatch m = *rxi;
      recurse_build_tree(m, path + top_tag + "/");
      rxi++;
    }

  }

}

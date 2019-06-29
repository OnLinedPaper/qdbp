#include "xmlparse.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <regex>
#include <fstream>

xmlparse::xmlparse() : root(new xmlnode()) {
  /*

  //load the regex from an external file
  //might change this later but it makes testing faster
  //because i can test new regex without recompiling
  std::string rx_str;

  std::ifstream in("");

  //check to see if i should put this somewhere else

  in.open("xml_parser/tree_regex.rx");
  if(!in) { std::cerr << "S-H-I-T!\n"; }
  std::getline(in, rx_str);
  tree_rx = std::regex (rx_str);
  in.close();

  in.open("xml_parser/tag_regex.rx");
  if(!in) { std::cerr << "S-H-I-T!\n"; }
  std::getline(in, rx_str);
  tag_rx = std::regex (rx_str);
  in.close();

  in.open("xml_parser/value_regex.rx");
  if(!in) { std::cerr << "S-H-I-T!\n"; }
  std::getline(in, rx_str);
  value_rx = std::regex (rx_str);
  in.close();
  */
};

xmlparse::~xmlparse() {
  delete root;
}

//============================================================================

void xmlparse::build_tree(const std::string file_name) {


  //get the xml file
  std::FILE *fp = std::fopen(file_name.c_str(), "rb");
  if(!fp) {
    std::cout << "Error! File couldn't be opened!\n";
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
  std::fread(&contents[0], 1, contents.size(), fp);

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
    std::string error = e + "- bad tag path: " + path + "\n";
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
    std::string error = "error! can't convert this value to an int!\n";
    error += "- value: " + retval + "\n";
    error += "- bad tag path: " + path + "\n";
    throw error;
  }
}

//============================================================================

double xmlparse::get_xml_double(const std::string path) {
  std::string retval = get_xml_string(path);
  try {
    return std::stod(retval);
  }
  catch (const std::invalid_argument& ia) {
    //they probably tried to convert a string or something
    std::string error = "error! can't convert this value to a float!\n";
    error += "- value: " + retval + "\n";
    error += "- bad tag path: " + path + "\n";
    throw error;
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

  //get the top tag - note that it is expected there are no children
  //whose names are duplicated by one of their parents
  first = content.find_first_of("<") + 1;
  last = content.find_first_of(">");
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

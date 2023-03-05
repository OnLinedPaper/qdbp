#include "src/xml_parser/v2/xml_handler.h"
#include "src/utils/message.h"
#include <iostream>
#include <fstream>
#include <deque>

const uint16_t xml_handler::DBG_ALL         = 1<<15;
const uint16_t xml_handler::DBG_DEFAULT     = 1<<14;
const uint16_t xml_handler::DBG_ONLY_FATAL  = 1<<13;
const uint16_t xml_handler::DBG_TREE_PRINT  = 1<< 5;
const uint16_t xml_handler::DBG_FATAL_ERROR = 1<< 4;
const uint16_t xml_handler::DBG_TREE_BUILD  = 1<< 3;
const uint16_t xml_handler::DBG_GET_FAIL    = 1<< 2;
const uint16_t xml_handler::DBG_PATH_FAIL   = 1<< 1;
const uint16_t xml_handler::DBG_NONE        = 1<< 0;


xml_handler::xml_handler() : root(new xmlnode()), dbg_msg_flags(0) { 
  set_dbg_msg_flags(DBG_DEFAULT);
}

xml_handler::~xml_handler() {
  delete root;
}

void xml_handler::set_dbg_msg_flags(uint16_t i) {
  dbg_msg_flags = 0;
  if(i & DBG_ALL) {
    dbg_msg_flags = DBG_TREE_PRINT | DBG_FATAL_ERROR | DBG_TREE_BUILD | DBG_GET_FAIL | DBG_PATH_FAIL;
  }
  else if(i & DBG_DEFAULT) {
    dbg_msg_flags = DBG_TREE_PRINT | DBG_FATAL_ERROR | DBG_GET_FAIL;
  }
  else if(i & DBG_ONLY_FATAL) {
    dbg_msg_flags = DBG_FATAL_ERROR;
  }
  else if(i & DBG_NONE) {
    return;
  }
  else {
    dbg_msg_flags = i;
  }
}

void xml_handler::build_tree(const std::string &file_name) {

  //perform xml parsing with the file pointer - never load the file into memory

  std::ifstream infile(file_name, std::ios::in);
  if(!infile) {
    if(dbg_msg_flags & DBG_TREE_BUILD) {
      msg::print_error("when bulding tree, file \"" + file_name + "\" couldn't be opened!");
      msg::print_alert("(did you rename / misspell / delete the file by mistake?)");
    }
    throw("S-H-I-T!");
  }

  //parent node, child tag
  std::vector<std::pair<xmlnode *, std::string>> unclosed_tags;
  unsigned last = 0;
  char c = ' ';
  std::string s, m = "";

  //current node that's being added to
  xmlnode *current = root;

  //will skip whitespace manually
  infile >> std::noskipws;

  //read through file
  while(infile.good() && !infile.eof()) {
    //consume whitespace
    while(std::isspace((char)infile.peek())) { infile.get(); }

    //determine if this is a tag, or content
    if((char)infile.peek() == '<') {
      //this is a tag

      //consume opening "<"
      infile.get();

      if((char)infile.peek() == '/') {
        //this is a close tag - ensure that it matches the previously opened
        //tag, and then close it  

        //consume opening "/"
        infile.get();
        s = "";
        while((char)infile.peek() != '>') {
          infile >> c;
          s += c;
        }
        
        if(s.compare(unclosed_tags.back().second) == 0) {
          //debug message
          if(dbg_msg_flags & DBG_TREE_BUILD) {
            m = "closing tag \"" + s + "\"";
            msg::get().print_alert(m);
          }

          //set the current to the parent node
          current = unclosed_tags.back().first;

          //pop the last node off the vector
          unclosed_tags.pop_back();
        }
        else {
          //uh oh. mismatched tags
          if(dbg_msg_flags & DBG_FATAL_ERROR) {
            m = "xml_handler::build_tree threw error: closing tag \"" + s + "\" does not match opening tag \"" + unclosed_tags.back().second + "\"";
            msg::get().print_error(m);
          }
          throw("S-H-I-T!");
        }
      }      
      else {
        //this is an open tag - get its name
        s = "";
        while((char)infile.peek() != '>') {
          infile >> c;
          s += c;
        }

        //debug message
        if(dbg_msg_flags & DBG_TREE_BUILD) {
          m = "adding child \"" + s + "\" to \"" + current->get_name() + "\"";
          msg::get().print_alert(m);
        }

        //save current tag to return to
        unclosed_tags.push_back(std::make_pair(current, s));

        //make a new node and switch to it
        current = current->add_child(s);

      }
      //consume closing ">"
      infile.get();
    }
    else {
      //this is a tag's value

      //get content
      s = "";
      while((char)infile.peek() != '<' && !infile.eof()) {
        infile >> c;
        s += c;
      }

      //debug message
      if(dbg_msg_flags & DBG_TREE_BUILD) {
        std::string m = "adding value \"" + s + "\" to \"" + current->get_name() + "\"";
        msg::get().print_alert(m);
      }

      current->set_value(s);
    }
  }
  return;
}

xmlnode * xml_handler::get_child(const std::string &path) {
  //extract the path from the string
  std::string s = "";

  xmlnode *current = root;

  //iterate through string
  for(char c : path) {
    if(c == '/') {
      if(!s.empty()) {
        current = current->get_child(s);

        //does this child exist?
        if(!current) {
          break;
        }

        s = "";
      }
    }
    else {
      s += c;
    }
  }
  
  if(current) {
    current = current->get_child(s);
  }

  //does this child exist?
  if(!current) {
    if(dbg_msg_flags & DBG_PATH_FAIL) {
      std::string m = "failed to locate tag \"" + s + "\" in path \"" + path + "\"";
      msg::get().print_warn(m);
    }
  }

  return current;
}

//reminder: prints to stdout
void xml_handler::print_tree() { 
  std::deque<std::pair<xmlnode *, int>> nodes;
  nodes.push_back(std::make_pair(root, 0));

  if(dbg_msg_flags & DBG_TREE_PRINT) {
      std::string m = "dumping xml tree to stdout";
      msg::get().print_alert(m);
  }

  //walk through nodes
  while(nodes.size()) {
    //get the node...
    xmlnode *current = nodes[0].first;
    //...and its depth (for formatting)
    int depth = nodes[0].second;

    //pop that entry off the front
    nodes.pop_front();

    //print node's name and value, and some formatting
    for (int i=0; i<depth; i++) { std::cout << "  "; }
    std::cout << "\"" << current->get_name() << "\" | \"";
    std::cout << current->get_value() << "\"" << std::endl;

    std::vector<std::string> v;
    current->get_children(v);

    for(const std::string &s : v) {
      nodes.push_front(std::make_pair(current->get_child(s), depth+1));
    }
  }
}

void xml_handler::get_all_child_tags(const std::string &path, std::vector<std::string> &v) {
  //get the child node
  xmlnode *current = get_child(path);
  if(current) {
    current->get_children(v);
  }
  else {
    //child does not exist
    if(dbg_msg_flags & DBG_FATAL_ERROR) {
      std::string m = "xml_handler::get_all_child_tags threw error: bad tag path \"" + path + "\"";
      msg::get().print_error(m);
    }
    throw("S-H-I-T!");
  }
}

bool xml_handler::check_path(const std::string &path) {
  bool is_valid = get_child(path) != NULL;
  return (is_valid);
}

void xml_handler::print_get_bad_value_msg(const std::string &func_name, const std::string &path, const std::string &value, bool safe_mode, const std::string &def) {
  
  if(safe_mode) {
    if(dbg_msg_flags & DBG_GET_FAIL) {
      std::string m = func_name + " issued warning: bad tag value \"" + value + "\" for path \"" + path + "\"";
      msg::get().print_warn(m);
      m = "safe mode: returning default value \"" + def + "\"";
      msg::get().print_alert(m);
    }
  }
  else {
    if(dbg_msg_flags & DBG_FATAL_ERROR) {
      std::string m = func_name + " raised error: bad tag value \"" + value + "\" for path \"" + path + "\"";
      msg::get().print_error(m);
    }
  }
}

void xml_handler::print_get_bad_path_msg(const std::string &func_name, const std::string &path, bool safe_mode, const std::string &def) {
  
  if(safe_mode) {
    if(dbg_msg_flags & DBG_GET_FAIL) {
      std::string m = func_name + " issued warning: bad tag path \"" + path + "\"";
      msg::get().print_warn(m);
      m = "safe mode: returning default value \"" + def + "\"";
      msg::get().print_alert(m);
    }
  }
  else {
    if(dbg_msg_flags & DBG_FATAL_ERROR) {
      std::string m = func_name + " raised error: bad tag path \"" + path + "\"";
      msg::get().print_error(m);
    }
  }
}

std::string xml_handler::get_xml_string(const std::string &path, bool safe_mode, std::string def) {
  xmlnode *current = get_child(path);
  if(!current) {

    print_get_bad_path_msg("xml_handler::get_xml_string", path, safe_mode, def);

    if(safe_mode) { return def; }
    else { throw("S-H-I-T!"); }
  }

  return current->get_value();
}

int xml_handler::get_xml_int(const std::string &path, bool safe_mode, int def) {
  xmlnode *current = get_child(path);
  if(!current) {

    print_get_bad_path_msg("xml_handler::get_xml_int", path, safe_mode, std::to_string(def));

    if(safe_mode) { return def; }
    else { throw("S-H-I-T!"); }
  }

  //attempt to parse value
  try {
    return std::stoi(current->get_value());
  } catch(std::invalid_argument const &e) {

    print_get_bad_value_msg("xml_handler::get_xml_int", path, current->get_value(), safe_mode, std::to_string(def));

    if(safe_mode) { return def; }
    else { throw("S-H-I-T!"); }
  }
}

float xml_handler::get_xml_float(const std::string &path, bool safe_mode, float def) {
  xmlnode *current = get_child(path);
  if(!current) {

    print_get_bad_path_msg("xml_handler::get_xml_float", path, safe_mode, std::to_string(def));

    if(safe_mode) { return def; }
    else { throw("S-H-I-T!"); }
  }

  //attempt to parse value
  try {
    return std::stof(current->get_value());
  } catch(std::invalid_argument const &e) {

    print_get_bad_value_msg("xml_handler::get_xml_float", path, current->get_value(), safe_mode, std::to_string(def));

    if(safe_mode) { return def; }
    else { throw("S-H-I-T!"); }
  }
}

bool xml_handler::get_xml_bool(const std::string &path, bool safe_mode, bool def) {
  xmlnode *current = get_child(path);
  if(!current) {

    print_get_bad_path_msg("xml_handler::get_xml_bool", path, safe_mode, std::to_string(def));

    if(safe_mode) { return def; }
    else { throw("S-H-I-T!"); }
  }

  //check true or false
  if(current->get_value().compare("true") == 0)  { return  true; }
  if(current->get_value().compare("false") == 0) { return false; }
  
  //bad input or badly formatted
  print_get_bad_value_msg("xml_handler::get_xml_bool", path, current->get_value(), safe_mode, std::to_string(def));
  msg::get().print_alert("value must be either \"true\" or \"false\", without capitalization or whitespace");
  
  if(safe_mode) { return def; }
  else { throw("S-H-I-T!"); }
}

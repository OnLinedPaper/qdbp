#define XML_TOOL_

#include <iostream>
#include <fstream>
#include <string>

#include "xmlparse.h"

int CURR_TREE = -1;
const int CHUNKTREE = 0;
const int GAMETREE = 1;
const int IMAGETREE = 2;
const int MAPTREE = 3;

const std::string CHUNKPATH = "resources/chunkdata.xml";
const std::string GAMEPATH =  "resources/gamedata.xml";
const std::string IMAGEPATH = "resources/imagedata.xml";
const std::string MAPPATH =   "resources/mapdata.xml";

void re_load_xml_trees();

void add_to_chunk_tree();
void add_to_game_tree();
void add_to_image_tree();
void add_to_map_tree();

void print_to_file();

int main(void) {

  int choice = 0;

  do {
    std::cout << 
      "1 - (re)load xml tree\n"
      "2 - print current tree\n"
      "3 - add to a tree\n"
      "4 - save a tree\n"

      "0 - quit\n"
    << std::endl;
    std::cin >> choice;

    if(choice == 1) {
      re_load_xml_trees();
    }
    else if(choice == 2) {
      xmlparse::get().print_tree();
    }
    else if(choice == 3) {
      switch(CURR_TREE) {
        case CHUNKTREE:
          add_to_chunk_tree(); break;
        case GAMETREE:
          add_to_game_tree(); break;
        case IMAGETREE:
          add_to_image_tree(); break;
        case MAPTREE:
          add_to_map_tree(); break;
        default:
          std::cout << "no valid tree currently loaded." << std::endl;
      }
    }
    else if(choice == 4) {
      print_to_file();
    }

  } while(choice != 0);

  return 0;
}

//=============================================================================

void re_load_xml_trees() {
  //load one tree from existing files.
  //THIS WILL DESTROY UNSAVED CHANGED.
  int choice = 0;
  std::cout << "this will destroy any unsaved changes! continue? (1/0)" << std::endl;
  std::cin >> choice;
  if(!choice) { return; }

  std::string path = "";
  std::cout << 
    ". load which tree?\n"
    ". 1 - chunk data\n"
    ". 2 - game data\n"
    ". 3 - image data\n"
    ". 4 - map data\n"

    ". 0 - abort\n"
  << std::endl;     
  std::cin >> choice;

  switch(choice) {
    case 1:
      path = CHUNKPATH;
      CURR_TREE = CHUNKTREE;
      break;
    case 2:
      path = GAMEPATH;
      CURR_TREE = GAMETREE;
      break;
    case 3:
      path = IMAGEPATH;
      CURR_TREE = IMAGETREE;
      break;
    case 4:
      path = MAPPATH;
      CURR_TREE = MAPTREE;
      break;

    case 0:
      return;
    default:
      std::cout << "invalid choice: " << choice << std::endl;
      return;
  }

  xmlparse::get().destroy_tree();
  xmlparse::get().build_tree(path);
}

//=============================================================================

void add_to_chunk_tree() { 

}

//=============================================================================

void add_to_game_tree() { 

}

//=============================================================================
/*
  <file> nonempty string .png </file>
  <dimensions>
    <width> positive nonzero int <width>
    <height> positive nonzero int <height>

    <pivot_x> int, blank for half width </pivot_x>
    <pivot_y> int, blank for half height </pivot_y>
    <frames> positive nonzero int, blank for 1 </frames>
    <frame_delay> positive nonzero int, blank for 1 </frame_delay>
  </dimensions>
  <blend> optional: ADD, MULT, NONE </blend>
*/

void add_to_image_tree() {
  std::string input, entry_name, file_name = "";
  int width, height, pivot_x, pivot_y, frames, frame_delay, blend = 0;

  while(input.empty()) {
    std::cout << "entry name (nonempty string): ";
    getline(std::cin, input);
  }
  entry_name = input; 
  input = "";

  while(input.empty()) {
    std::cout << "file name (nonempty string w/ .png): ";
    getline(std::cin, input);
    if(input.size() <= 4 || input.substr(input.size() - 4, input.size()).compare(".png")) {
      std::cout << "must end with .png\n";
      input = "";
    }
  }
  file_name = input;
  input = "";

  while(input.empty()) {
    std::cout << "width (positive nonzero int): ";
    getline(std::cin, input);
    try {
      width = std::stoi(input);
    } catch (std::invalid_argument) {
      input = "";
    }
  }
  input = "";

  while(input.empty()) {
    std::cout << "height (positive nonzero int): ";
    getline(std::cin, input);
    try {
      height = std::stoi(input);
    } catch (std::invalid_argument) {
      input = "";
    }
  }
  input = "";

  while(input.empty()) {
    std::cout << "pivot_x (int, blank for half): ";
    getline(std::cin, input);
    if(input.empty()) { 
      pivot_x = width / 2;
      break;
    }
    try {
      pivot_x = std::stoi(input);
    } catch (std::invalid_argument) {
      input = "";
    }
  }
  input = "";

  while(input.empty()) {
    std::cout << "pivot_y (int, blank for half): ";
    getline(std::cin, input);
    if(input.empty()) { 
      pivot_y = height / 2;
      break;
    }
    try {
      pivot_y = std::stoi(input);
    } catch (std::invalid_argument) {
      input = "";
    }
  }
  input = "";


  while(input.empty()) {
    std::cout << "frame count (positive nonzero int): ";
    getline(std::cin, input);
    try {
      frames = std::stoi(input);
    } catch (std::invalid_argument) {
      input = "";
    }
  }
  input = "";

  while(input.empty()) {
    std::cout << "frame delay (positive nonzero int): ";
    getline(std::cin, input);
    try {
      frame_delay = std::stoi(input);
    } catch (std::invalid_argument) {
      input = "";
    }
  }
  input = "";


  std::cout << "add blending? (1/0): ";
  std::cin >> blend;
  if(blend) {
    while(input.empty()) {
      std::cout << "blend mode NONE BLEND ADD MOD (1/2/3/4) ";
      getline(std::cin, input);
      try {
        blend = std::stoi(input);
        if(blend < 1 || blend > 4) {
          input = "";
        }
      } catch (std::invalid_argument) {
        input = "";
      }
    }
  }

  //add to tree
  xmlparse::get().get_root()->insert_child(entry_name, "/");
  xmlparse::get().get_root()->insert_child("file", "/" + entry_name + "/", file_name);
  xmlparse::get().get_root()->insert_child("dimensions", "/" + entry_name + "/");
  xmlparse::get().get_root()->insert_child("width", "/" + entry_name + "/dimensions/", std::to_string(width));
  xmlparse::get().get_root()->insert_child("height", "/" + entry_name + "/dimensions/", std::to_string(height));
  xmlparse::get().get_root()->insert_child("pivot_x", "/" + entry_name + "/dimensions/", std::to_string(pivot_x));
  xmlparse::get().get_root()->insert_child("pivot_y", "/" + entry_name + "/dimensions/", std::to_string(pivot_y));
  xmlparse::get().get_root()->insert_child("frames", "/" + entry_name + "/dimensions/", std::to_string(frames));
  xmlparse::get().get_root()->insert_child("frame_delay", "/" + entry_name + "/dimensions/", std::to_string(frame_delay));
  if(blend) {
    std::string mode = "";
    switch(blend) {
      case 1:
        mode = "NONE"; break;
      case 2:
        mode = "BLEND"; break;
      case 3:
        mode = "ADD"; break;
      case 4:
        mode = "MOD"; break;
    }
    xmlparse::get().get_root()->insert_child("blend", "/" + entry_name + "/", mode);
  }
}

//=============================================================================

void add_to_map_tree() {

}

//=============================================================================

void print_to_file() {
  std::string file_name = "";
  std::ofstream f;
  
  switch(CURR_TREE) {
    case CHUNKTREE:
      file_name = CHUNKPATH; break;
    case GAMETREE:
      file_name = GAMEPATH; break;
    case IMAGETREE:
      file_name = IMAGEPATH; break;
    case MAPTREE:
      file_name = MAPPATH; break;
    default:
      std::cout << "cannot print to file: no tree loaded." << std::endl;
      return;
  }

  file_name += ".NEW";

  f.open(file_name);
  if(!f.is_open()) { 
    std::cout << "couldn't open file " << file_name << " for printing! S-H-I-T!" << std::endl;
    return;
  }

  xmlparse::get().print_as_file(f);
  f.close();
}

#include "message.h"
#include <string>
#include <iostream>
#include <experimental/filesystem>

const std::string msg::rn = "\033[0;31m";
const std::string msg::rb = "\033[1;31m";
const std::string msg::yn = "\033[0;33m";
const std::string msg::yb = "\033[1;33m";
const std::string msg::gn = "\033[0;32m";
const std::string msg::gb = "\033[1;32m";
const std::string msg::cn = "\033[0;36m";
const std::string msg::cb = "\033[1;36m";
const std::string msg::none = "\033[0m";


msg::msg() : outfile(NULL) { }

msg::~msg() {
  close_log();
}

void msg::print_error(const std::string message) {
  std::cerr << rb << "error! " << rn << message << none << std::endl;
  if(get().outfile && get().outfile.is_open()) {
    get().outfile << "error! " << message << std::endl;
  }
}

void msg::print_warn(const std::string message) {
  std::cerr << yb << "warning: " << yn << message << none << std::endl;
  if(get().outfile && get().outfile.is_open()) {
    get().outfile << "warning: " << message << std::endl;
  }

}

void msg::print_alert(const std::string message) {
  std::cerr << cn << "- " << message << none << std::endl;
  if(get().outfile && get().outfile.is_open()) {
    get().outfile << "- " << message << std::endl;
  }

}

void msg::print_good(const std::string message) {
  std::cerr << gn << message << none << std::endl;
  if(get().outfile && get().outfile.is_open()) {
    get().outfile << message << std::endl;
  }

}


void msg::init_log(const std::string filename) {
  if(!(std::experimental::filesystem::exists("./log"))) {
    std::experimental::filesystem::create_directory("./log");
  }

  std::string fname = "";
  fname = (filename.empty() ? "./log/log.txt" : "./log" + filename);

  outfile.open(fname);
  if(!outfile) {
    print_warn("Couldn't open output file " + fname + "!");
  }
}

void msg::close_log() {
  if(outfile) {
    outfile.close();
  }
}

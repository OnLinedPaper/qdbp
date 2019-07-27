#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <string>

class msg {
public:
  static const std::string rn;
  static const std::string rb;
  static const std::string yn;
  static const std::string yb;
  static const std::string gn;
  static const std::string gb;
  static const std::string cn;
  static const std::string cb;
  static const std::string none;

  static void print_error(const std::string message);
  static void print_warn(const std::string message);
  static void print_alert(const std::string message);
  static void print_good(const std::string message);
};

#endif

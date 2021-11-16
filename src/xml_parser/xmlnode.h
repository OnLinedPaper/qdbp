#ifndef XMLNODE_H_
#define XMLNODE_H_

#include <string>
#include <map>
#include <vector>

class xmlnode {
public:
  xmlnode();
  xmlnode(std::string const);
  xmlnode(std::string const, std::string const);
  ~xmlnode();

  const std::string get_value() const { return value; }
  void set_value(const std::string val) { value = val; }

  const std::string get_name() const { return name; }
  void set_name(const std::string n) { name = n; }

  void add_child(const std::string n);
  void insert_child(const std::string name, const std::string path);
  void insert_child(const std::string name, const std::string path, const std::string value);

  void recursive_print(const int depth);
  std::string recursive_get_value(const std::string path, bool send_alert);
  std::vector<std::string> recursive_get_all_child_tags(const std::string path);
  bool recursive_check_path(const std::string path, bool send_alert);

protected:
  std::string dive_name(const std::string) const;
  std::string dive_path(const std::string) const;

private:
  std::string name; //one name for the tag
  std::string value;  //one value per xml tag
  std::map<std::string, xmlnode*> children; //the children of this node

};

#endif

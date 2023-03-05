#ifndef XMLNODE_V2_H_
#define XMLNODE_V2_H_

#include <map>
#include <string>
#include <vector>

//second attempt at XML parsing. not going to use any recursion. still on
//the fence about regular expressions.

//node in this case is a heavily simplified version of its previous setup
//and will rely on the handler to deal with most of the work now.
//it includes a name for the tag, a value in the tag, and a map of children.
//that's it.
class xmlnode {
public:
  xmlnode();
  xmlnode(std::string const);
  xmlnode(std::string const, std::string const);
  ~xmlnode();

  const std::string get_name() const { return name; }
  void set_name(const std::string &n) { name = n; }

  const std::string get_value() const { return value; }
  void set_value(const std::string &v) { value = v; }

  //creates a new node in the array of children with NO VALUE - this must be
  //set afterwards!
  //returns a pointer to the newly created child
  xmlnode * add_child(const std::string &n);

  //returns NULL if there is no child; otherwise returns a pointer to the 
  //child node
  xmlnode *get_child(const std::string &n); //use std::map::count

  //returns a vector of child node keys
  void get_children(std::vector<std::string> &);

private:
  std::string name;
  std::string value;
  std::map<std::string, xmlnode *> children;

};

#endif

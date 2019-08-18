#ifndef BACKGROUND_H_
#define BACKGROUND_H_

#include <string>

class b_ground {
public:
  b_ground(std::string);
  ~b_ground();

  void draw() const;

private:

  std::string bg_name;

};

#endif

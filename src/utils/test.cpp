#include "pather.cpp"
#include "rng.h"
#include<ctime>

int main(void) {

  auto seed = time(NULL);
  std::cout << "seed: " << seed <<std::endl;
  rng::get().seed(seed);

  int rows = 8;
  int cols = 8;

  pather *p = new pather(rows, cols);

  p->path(seed % rows, 0.4);

  p->check_path_exists(seed % rows);
  p->print();


  delete p;

  return 0;

}

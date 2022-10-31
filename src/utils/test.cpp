#include "pather.cpp"
#include "rng.h"
#include<ctime>

int main(void) {

  auto seed = time(NULL);
  std::cout << "seed: " << seed <<std::endl;
  rng::get().seed(seed);

  pather *p = new pather(8, 8);

  p->path(seed % 8);

  p->print();

  delete p;

  return 0;

}

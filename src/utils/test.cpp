#include "pather.cpp"
#include "rng.h"
#include <ctime>
#include <cmath>

int main(void) {

  auto seed = time(NULL);
  std::cout << "seed: " << seed <<std::endl;
  rng::get().seed(seed);

  int rows = 16;
  int cols = 20;

  pather *p = new pather(rows, cols);

  p->path(seed % rows, 0.4);

  p->print();

  std::vector<std::pair<int, std::pair<int, int>>> res;
  p->get_far_point(res, std::sqrt(rows*cols) / 2);

  delete p;

  return 0;

}

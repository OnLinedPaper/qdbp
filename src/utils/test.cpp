#include "pather.cpp"
#include "rng.h"
#include<ctime>

int main(void) {

  rng::get().seed(time(NULL));

  pather *p = new pather(8, 8);

  p->path(2);
  if(p->get_density() < 0.6) { p->path(5); }

  p->print();

  delete p;

  return 0;

}

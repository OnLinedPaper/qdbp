#include "rng.h"

rng::rng() { }

rng::~rng() { }

int rng::get_map(int min, int max) {
  return get_range(min, max, map);
}

int rng::get_shot(int min, int max) {
  return get_range(min, max, shot);
}

int rng::get_unim(int min, int max) {
  return get_range(min, max, unim);
}

int rng::get_gfx(int min, int max) {
  return get_range(min, max, gfx);
}


int rng::get_range(int min, int max, std::mt19937 &rng) {
  return (int)(rng() % (max + 1 - min) + min);
}

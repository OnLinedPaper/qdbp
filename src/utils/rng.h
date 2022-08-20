#ifndef RNG_H_
#define RNG_H_

#include <random>

//handles the rng for the game, with some stipulations. although randomly
//generated, parts of the game's behavior need to be deterministic, while
//others are not relevant enough to need this. also, rng is slow, so prng
//is used instead. 
//---- critical
//map:    used to generate the maps. accepts one seed at game start and
//        then uses it for the rest of runtime.
//shot:   used to add variance to projectiles. currently shared between player
//        and npcs.
//---- irrelevant
//unim:   unimportant rng; can be called any time by misc processes.
//gfx:    handles graphics-related rng, like frame bumps or shaking outlines.
class rng {
public:
  ~rng();

  static rng &get() {
    static rng instance;
    return instance;
  }
  
  uint32_t get_map()  { return map(); }
  uint32_t get_shot() { return shot(); }
  uint32_t get_unim() { return unim(); }
  uint32_t get_gfx()  { return gfx(); }

  void seed(int num) { map.seed(num); shot.seed(num); unim.seed(num); gfx.seed(num); }


private:
  std::mt19937 map;
  std::mt19937 shot;
  std::mt19937 unim;
  std::mt19937 gfx;
  rng();
};

#endif

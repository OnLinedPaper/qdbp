#if defined RENDER_NC
#include "src/hud/hud.h"
#include "src/viewport/viewport.h"
#include "src/xml_parser/xmlparse.h"
#include "src/image/image_handler.h"
#include "src/entity_handler/entity_handler.h"


hud::hud() { }
hud::~hud() { }
void hud::draw() { 
  //TODO:
  //- heat
  //- - normal heat
  //- - overheat
  //- - overheat flicker
  //- health
  //- - health segs
  //- - recoverable health
  //- shield
  //- - shield segs
  //- special warnings
}
void hud::draw_overheat_warning() { }
void hud::update() { }
#endif

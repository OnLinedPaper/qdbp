//this file exists as a separate entity dsigned to be included if a macro is set at compilation time to disable SDL rendering. it uses ncurses rendering instead.

#if defined RENDER_NC
#include <ctime>

#include "engine.h"
#include "src/renders/render.h"
#include "src/xml_parser/xmlparse.h"
#include "src/viewport/viewport.h"
#include "src/timeframe/timeframe.h"
#include "src/environment/map/map_handler.h"
#include "src/utils/message.h"
#include "src/entity_handler/entity_handler.h"
#include "src/text/text_handler.h"
#include "src/movers/drawable/mortal/weapon.h"
#include "src/hud/hud.h"
#include "src/utils/rng.h"
#include "src/text/text.h"


void engine::play() { }

engine::engine() : debug_swirly_int(0), controller(NULL) { }

engine::~engine() { }

void engine::next_tick() { }
#endif

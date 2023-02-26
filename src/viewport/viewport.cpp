#include "viewport.h"
#include "src/renders/render.h"

#if defined RENDER_NC
#include "viewport_nc.cpp"
#endif

#if defined RENDER_SDL
viewport::viewport() {
  view_width = view_height = 0;
  pos[0] = pos[1] = 0;

  SDL_GetWindowSize(render::get().get_w(), &view_width, &view_height);
}

void viewport::set_pos(const vec2d p) { pos = p; }
#endif

viewport::~viewport() { }

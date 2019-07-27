#include "texture.h"
#include "render.h"
#include "src/utils/message.h"
#include <string>

texture::texture(std::string path) : g_texture(NULL) {

  SDL_Surface *s = IMG_Load(path.c_str());
  if( s == NULL ) {
    msg::print_error("couldn't create image \"" + path + "\"! Error: " + std::string(SDL_GetError()));
  }
  else {
    //make the texture from the surface's pixels
    g_texture = SDL_CreateTextureFromSurface(render::get().get_r(), s);
    if(g_texture == NULL) {
      fprintf(stderr, "Couldn't create texture! Error: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(s);
  }

  return;
}

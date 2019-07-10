#include "texture.h"
#include "render.h"

texture::texture(std::string path) : g_texture(NULL) {

  SDL_Surface *s = IMG_Load(path.c_str());
  if( s == NULL ) {
    std::cerr << "Couldn't create image \"" << path << "\"! Error: " << SDL_GetError() << std::endl;
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

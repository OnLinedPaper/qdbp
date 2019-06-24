#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>

class texture {
  public:
    texture() = delete;
    texture(std::string path);
    texture(const texture&) = delete;
    texture& operator=(const texture&) = delete;
    ~texture();

    SDL_Texture *get_texture() { return g_texture; }

  private:
    SDL_Texture *g_texture;


};

#endif

#include "text_handler.h"
#include <string>

text_h::text_h() :
  active_t(),
  recycled_t()
{

  if( TTF_Init() == -1 ) {
    throw(std::string("Couldn't init SDL text! Error: ") + SDL_GetError());
  }

  font = TTF_OpenFont("./resources/fonts/thank-you-prophessor.ttf", 28);
  if(font == NULL) {
    throw(
      std::string("(From text_handler): Couldn't init font! Error: " +
      std::string(SDL_GetError()))
    );
    return;
  }
}

text_h::~text_h() {
  TTF_CloseFont(font);
}

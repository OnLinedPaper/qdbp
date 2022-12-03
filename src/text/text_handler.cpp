#include "text_handler.h"
#include "src/utils/message.h"
#include <string>

text_h::text_h() :
  active_t(),
  recycled_t()
{

  if( TTF_Init() == -1 ) {
    std::string e_msg = std::string("Couldn't init SDL text! Error: ") + SDL_GetError();
    msg::get().print_error("text_h::text_h threw error: " + e_msg);
    throw(e_msg);
  }

  font = TTF_OpenFont("./resources/fonts/thank-you-prophessor.ttf", 28);
  if(font == NULL) {
    std::string e_msg = std::string("Couldn't init font! Error: ") + SDL_GetError();
    msg::get().print_error("text_h::text_h threw error: " + e_msg);
    throw(e_msg);
    return;
  }
}

text_h::~text_h() {
  TTF_CloseFont(font);
}

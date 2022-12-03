#include "text.h"
#include "src/renders/render.h"
#include "src/utils/message.h"

text::text() :
  font(TTF_OpenFont("./resources/fonts/thank-you-prophessor.ttf", 28)),
  msg(""),
  dims({0, 0, 0, 0}),
  col({255, 255, 255, 255}),
  changed(true),
  txtt(NULL)
{ }

text::text(const std::string &m, float x, float y) :
  font(TTF_OpenFont("./resources/fonts/thank-you-prophessor.ttf", 28)),
  msg(m),
  dims({(int)x, (int)y, 0, 0}),
  col({255, 255, 255, 255}),
  changed(true),
  txtt(NULL)
{
  if(font == NULL) {
    std::string e_msg =
      std::string("Couldn't init font! Error: ") + SDL_GetError();
    msg::get().print_error("text::text threw error: " + e_msg);
    throw(e_msg);
  }
}

text::~text() {
  if(font) {
    TTF_CloseFont(font);
  }
}

void text::draw() {
  if(msg.empty()) { return; }

  //only re-create the texture if the message or the color changed
  if(changed) {

    //check to see if there's already something here
    if(txtt) {
      SDL_DestroyTexture(txtt);
    }

    SDL_Surface *txts = TTF_RenderText_Solid(font, msg.c_str(), col);
    if(txts == NULL) {
      std::string e_msg = std::string("(From text): Couldn't create surface! Error: ") + SDL_GetError();
      msg::get().print_error("text::draw threw error: " + e_msg);
      throw(e_msg);
    }
    txtt = SDL_CreateTextureFromSurface(render::get().get_r(), txts);
    dims.w = txts->w;
    dims.h = txts->h;
    SDL_FreeSurface(txts);
  }

  SDL_RenderCopy(render::get().get_r(), txtt, NULL, &dims);

}

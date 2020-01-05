#ifndef TEXT_HANDLER_H_
#define TEXT_HANDLER_H_

#include <SDL2/SDL_ttf.h>


class text_h {

public:
  ~text_h();

  static text_h &get() {
    static text_h instance;
    return instance;
  }

private:

  TTF_Font *font;

  text_h();
  text_h(const text_h&) = delete;
  text_h &operator=(const text_h&) = delete;

};

#endif

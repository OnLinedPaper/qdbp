#ifndef TEXT_H_
#define TEXT_H_

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class text {
public:
  text();
  text(const std::string&, float, float);
  text(const std::string&, float, float, const SDL_Color&);


  ~text();


  const std::string &get_msg() const { return msg; }
  void set_msg(const std::string &m) { msg = m; changed = true; }

  const SDL_Color &get_col() const { return col; }
  void set_col(const SDL_Color &c) { col = c; changed = true; }

  const SDL_Rect &get_dims() const { return dims; }
  void set_dims(float x, float y) { dims.x = x; dims.y = y; }

  void draw();

private:
  TTF_Font *font;
  std::string msg;
  SDL_Rect dims;
  SDL_Color col;

  bool changed;

  SDL_Texture *txtt;


  text(const text&) = delete;
  text &operator=(const text&) = delete;
};

#endif

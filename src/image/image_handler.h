#ifndef IMAGE_HANDLER_H_
#define IMAGE_HANDLER_H_
#include <unordered_map>
#include <src/image/image.h>
#include <random>

class image_handler {
public:
  static image_handler &get() {
    static image_handler instance;
    return instance;
  }

  ~image_handler();

  void draw_rotate(const std::string name, float x, float y, float frame_bump, float angle);
  void draw_rotate_color(const std::string name, float x, float y, 
      float frame_bump, float angle, const SDL_Color &c);
  void draw_rotate_color_opacity(const std::string name, float x, float y, 
      float frame_bump, float angle, const SDL_Color &c, float opacity);
  void draw_rotate_color_outline(const std::string name, float x, float y,
      float frame_bump, float angle, const SDL_Color &c, bool outline_color, float opacity);
  void draw_tile(const std::string name, float parallax);
  void draw_nc_bg(int density);
  void draw_nc_bg(const vec2d &tlc, const vec2d &brc, int density);
  void draw_nc_bg(int tlcx, int tlcy, int brcx, int brcy, int density);

  static void get_col_from_team(const std::string, SDL_Color &);
  static void jitter_col(int, SDL_Color &);

private:
  std::unordered_map<std::string, image> images;

  void add_image(const std::string name);

  image_handler();
  image_handler(const image_handler&) = delete;
  image_handler &operator=(const image_handler&) = delete;
};

#endif

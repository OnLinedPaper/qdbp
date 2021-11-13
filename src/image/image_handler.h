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

  //draws relative to game world
  void draw_rotate(const std::string name, float x, float y, float frame_bump, float angle);
  void draw_rotate_color(const std::string name, float x, float y, 
      float frame_bump, float angle, const SDL_Color &c);
  void draw_rotate_color_opacity(const std::string name, float x, float y, 
      float frame_bump, float angle, const SDL_Color &c, float opacity);
  void draw_rotate_color_outline(const std::string name, float x, float y,
      float frame_bump, float angle, const SDL_Color &c, bool outline_color, float opacity);
  void draw_tile(const std::string name, float parallax);

  //draws relative to the screen (used by hud)
  void draw_r_c_o_relative(const std::string name, float x,
      float y, float frame_bump, float angle, const SDL_Color &c, float opacity);

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

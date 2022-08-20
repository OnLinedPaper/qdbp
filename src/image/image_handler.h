#ifndef IMAGE_HANDLER_H_
#define IMAGE_HANDLER_H_
#include <unordered_map>
#include <src/image/image.h>

class image_handler {
public:
  static image_handler &get() {
    static image_handler instance;
    return instance;
  }

  ~image_handler();

  void draw_tile(const std::string name, float parallax, float x_offset, float y_offset);
  void draw_tile(const std::string name, float parallax)
    { draw_tile(name, parallax, 0, 0); }

  //hopefully the end-all, be-all drawing function
  void draw_v2(
      const std::string name,           //name of image to draw
      float x,                          //x value to draw at
      float y,                          //y value to draw at
      float angle,                      //angle to draw at
      bool relative_to_screen,          //uses screen units? (entities = false, HUD = true)
      float frame_bump,                 //which frame of the animation to start with
      const SDL_Color &c,               //color to modualte the sprite with
      float opacity,                    //how opaque to make the image (0 = transparent, 1 = opaque)

      const std::string ot_name = "",   //name of an overlay texture - it and all below can be blank
      float parallax = 0,               //parallax of overlay texture
      float x_offset = 0,               //x offset of overlay texture
      float y_offset = 0,               //y offset of overlay texture
      float ot_frame_bump = 0           //frame bump of overlay texture
  );

  //some overloads
  void draw_v2(const std::string name, float x, float y, float frame_bump, float angle) 
    { draw_v2(name, x, y, angle, false, frame_bump, {255, 255, 255}, 1); }

  static void get_col_from_team(const std::string, SDL_Color &);
  static void jitter_col(int, SDL_Color &);


//  void DEBUG_draw_with_tile_overlay(const std::string shape_name, float x, float y, float angle, bool relative_to_screen, float frame_bump, const SDL_Color &c, float opacity, const std::string overlay_name, float parallax, float x_offset, float y_offset, int f_bump);

private:
  std::unordered_map<std::string, image> images;

  void add_image(const std::string name);

  image_handler();
  image_handler(const image_handler&) = delete;
  image_handler &operator=(const image_handler&) = delete;
};

#endif



/*
public:
  //deprecated drawing functions - these were the steps i took to arrive at the final
  //function above

  void draw_rotate(const std::string name, float x, float y, float frame_bump, float angle);
  void draw_rotate_color(const std::string name, float x, float y, 
      float frame_bump, float angle, const SDL_Color &c);
  void draw_rotate_color_opacity(const std::string name, float x, float y, 
      float frame_bump, float angle, const SDL_Color &c, float opacity);
  void draw_rotate_color_outline(const std::string name, float x, float y,
      float frame_bump, float angle, const SDL_Color &c, bool outline_color, float opacity);
  //draws relative to the screen (used by hud)
  void draw_r_c_o_relative(const std::string name, float x,
      float y, float frame_bump, float angle, const SDL_Color &c, float opacity);
*/
 


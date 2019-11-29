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

  void draw_rotate(const std::string name, float x, float y, float frame_bump, float angle);
  void draw_tile(const std::string name, float parallax);

private:
  std::unordered_map<std::string, image> images;

  void add_image(const std::string name);

  image_handler();
  image_handler(const image_handler&) = delete;
  image_handler &operator=(const image_handler&) = delete;
};

#endif

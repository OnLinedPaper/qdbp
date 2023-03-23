#if defined RENDER_NC
#include "src/environment/background/background.h"
#include "src/image/image_handler.h"
#include <string>
#include "src/renders/render_nc.h"
#include <ncurses.h>

void b_ground::draw() const { 
  static uint32_t bg_static = 1;
  int pattern = 1;

  char *r; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  for(int i=0; i<C; i++) {
    for(int j=0; j<L; j++) {
      //use a simple-ish pattern to make "static" appear
      bg_static *= 3;
      bg_static = bg_static ^ (bg_static >> 1);

      if(pattern == 0) {
        //mild, settled-down pattern
        switch(bg_static & 1) {
          case 0:
            r[j*C+i] = ':'; break;
          case 1:
            r[j*C+i] = ';'; break;
          default:
            r[j*C+i] = ' '; break;
        }
      }

      if(pattern == 1) {
        //this one is kind of an eye-fucker. maybe safer to not use it...
        switch(bg_static & 63) {
          case 0:
            r[j*C+i] = '.'; break;
          case 1:
            r[j*C+i] = ','; break;
          case 2:
            r[j*C+i] = ':'; break;
          case 3:
            r[j*C+i] = ';'; break;
          case 4:
            r[j*C+i] = '\''; break;
          case 5:
            r[j*C+i] = '`'; break;
          default:
            r[j*C+i] = ' '; break;
        }
      }
    }
  }
}
#endif

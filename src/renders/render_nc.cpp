#if defined RENDER_NC
#include "render_nc.h"
#include "src/utils/message.h"
#include <iostream>
#include "src/viewport/viewport.h"

render::render() : prev_LINES(-1), prev_COLS(-1), w(NULL), r(NULL), nc_render_this_frame(true) { 
  initscr();
  cbreak();
  noecho();

  w = newwin(LINES, COLS, 0, 0);
  keypad(w, TRUE);
  timeout(1);
  curs_set(0);

  nc_render();
}

render::~render() { 
  delwin(w);
  endwin();
  free(r);
}

void render::get_r(char * &r_in, int &L, int &C) {
  //if the window resized, it's not safe to draw into the array anymore
  if(LINES != prev_LINES || COLS != prev_COLS) {
    r_in = NULL;
    L = -1;
    C = -1;
  }
  r_in = r;
  L = prev_LINES;
  C = prev_COLS;
}

void render::nc_render() { 
  nc_check_stale_win_size();

  //check to see if it's safe to draw
  if(nc_render_this_frame) {
    //draw
    for(int i=0; i<COLS; i++) {
      for(int j=0; j<LINES; j++) {
        //add the contents of the array to the screen buffer
        mvwaddch(w, j, i, r[j*COLS+i]);

        //reset the array
        //bg is already going to overlap everything, so it isn't exactly
        //necessary, but given that the array is probably never going to be 
        //more than around 50 thousand characters (modern screens have 
        //2 million + pixels, for comparison) i don't think it could hurt...
        //it'll also provide an indicator if BG stops working
        r[j*COLS+i] = '?';
      }
    }

    //pretty little blinky on the bottom left corner!
    draw_blinky();
    //render the buffer to the terminal
    wrefresh(w);
  }
}

void render::nc_check_stale_win_size() {
  //r contains a buffer of characters that's meant to be used for rendering.
  //it's sized to the window (terminal) and must be resized if the window
  //changed, or was just initialized. 

  //check for cases where window size has changed (or just been created) and
  //adjust size accordingly, blockign rendering for that frame
  if(r == NULL || LINES != prev_LINES || COLS != prev_COLS) {
    free(r);
    r = (char *)malloc(sizeof(char) * LINES * COLS);
    if(r == NULL) {
      //TODO: error messaging
      throw("malloc failed");
    }
    prev_LINES = LINES;
    prev_COLS = COLS;
    nc_render_this_frame = false;
  }
  else {
    nc_render_this_frame = true;
  }

}

void render::draw_blinky() {
  char c[4] = {'q', 'd', 'b', 'p'};
  static uint8_t i = 0;
  mvwaddch(w, LINES-1, 0, c[(i/ 1)%4] | A_REVERSE);
  mvwaddch(w, LINES-1, 1, c[(i/ 4)%4] | A_REVERSE);
  mvwaddch(w, LINES-1, 2, c[(i/16)%4] | A_REVERSE);
  mvwaddch(w, LINES-1, 3, c[(i/64)%4] | A_REVERSE);
  i++;

  return;
}

#endif

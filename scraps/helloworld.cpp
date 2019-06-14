#include <SDL2/SDL.h>
#include <stdio.h>

//dimensions of screen
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;



//temporary globals




//start SDL and make the window
bool init(SDL_Window **w, SDL_Surface **s);

//load the media
bool loadMedia(SDL_Surface **s);

//shut everything down
void closeSDL(SDL_Window **w, SDL_Surface **s);


int main(void) {

  //make a window
  SDL_Window* g_window = NULL;

  //surface contained by the window
  SDL_Surface* g_screenSurface = NULL;

  SDL_Surface* g_image = NULL;

  if (!init(&g_window, &g_screenSurface) || !loadMedia(&g_image)) {
    fprintf(stderr, "S-H-I-T!");
  }
  else {
    if(g_image == NULL) { fprintf(stderr, "S-H-I-T-T!");}
    SDL_BlitSurface(g_image, NULL, g_screenSurface, NULL);
    SDL_UpdateWindowSurface(g_window);
    SDL_Delay(2000);
  }

  closeSDL(&g_window, &g_image);

  return 0;
}

bool init(SDL_Window **w, SDL_Surface **s) {
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "SDL couldn't initialize! SDL_Error: %s\n", SDL_GetError());
    success = false;
  }
  else {
    *w = SDL_CreateWindow("tutorial2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(*w == NULL) {
      fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
      success = false;
    }
    else {
      //get teh surface
      *s = SDL_GetWindowSurface(*w);
    }
  }
  return success;

}

bool loadMedia(SDL_Surface **s) {
  bool success = true;

  *s = SDL_LoadBMP("./output-onlinepngtools.bmp");
  if(*s == NULL) {
        fprintf(stderr, "Unable to load image %s! SDL Error: %s\n", "IMAGE", SDL_GetError() );
        success = false;
  }
  return success;
}

void closeSDL(SDL_Window **w, SDL_Surface **s) {
  SDL_FreeSurface(*s);
  *s = NULL;

  SDL_DestroyWindow(*w);
  *w = NULL;

  SDL_Quit();
}

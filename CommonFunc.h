#ifndef COMMON_FUNCTION_H_
#define COMMON_FUNCTION_H_

#include <windows.h>
#include <string>
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_image.h"


static SDL_Window* g_window = NULL;
static SDL_Renderer* g_screen = NULL;
static SDL_Event g_event;

// Screen
const int SCREEN_WIDTH=1280;
const int SCREEN_HEIGHT=640;
const int SCREEN_BPP = 32;


const int COLOR_KEY_R=167;
const int COLOR_KEY_G=175;
const int COLOR_KEY_B=180;


const int RENDER_DRAW_COLOR=0xff;


#define TILE_SIZE 64

#define MAX_MAP_X 400
#define MAX_MAP_Y 10

typedef struct Map  
{
    int start_x_;
    int start_y_;

    int max_x_;
    int max_y_;

    int tile[MAX_MAP_Y][MAX_MAP_X];
    char* file_name_;
};

#endif
#ifndef COMMON_FUNCTION_H_
#define COMMON_FUNCTION_H_


#include <windows.h>
#include <string>
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_image.h"
#include <fstream>


static SDL_Window* g_window = NULL;
static SDL_Renderer* g_screen = NULL;
static SDL_Event g_event;

// Screen
const int FRAME_PER_SECOND=40;
const int SCREEN_WIDTH=1920;
const int SCREEN_HEIGHT=1080;
const int SCREEN_BPP = 44;


const int COLOR_KEY_R=167;
const int COLOR_KEY_G=175;
const int COLOR_KEY_B=120;


const int RENDER_DRAW_COLOR=0xff;

#define BLANK_TILE 0
#define TILE_SIZE 80

#define MAX_MAP_X 400
#define MAX_MAP_Y 8


typedef struct Input
{
    int left_;
    int right_;
    int up_;
    int down_;
    int jump_;
}Input;

typedef struct Map  
{
    int start_x_;
    int start_y_;

    int max_x_;
    int max_y_;

    int tile[MAX_MAP_Y][MAX_MAP_X];
    std::string file_name_;
}Map;

#endif
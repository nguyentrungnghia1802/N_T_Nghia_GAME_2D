#ifndef COMMON_FUNCTION_H_
#define COMMON_FUNCTION_H_


#include <windows.h>
#include <string>
#include <vector>
#include <fstream>


#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_image.h"
#include "src/include/SDL2/SDL_ttf.h"
#include "src/include/SDL2/SDL_mixer.h"
#include "TextObject.h"


static Mix_Chunk *gEarn_Heart = NULL;
static Mix_Chunk *gFire_ball = NULL;
static Mix_Chunk *gPlayer_Die = NULL;

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_screen = NULL;
static SDL_Event g_event;


// Screen
const int FRAME_PER_SECOND=60;
const int SCREEN_WIDTH=1422;
const int SCREEN_HEIGHT=800;
const int SCREEN_BPP = 44;

const int COLOR_KEY_R=167;
const int COLOR_KEY_G=175;
const int COLOR_KEY_B=120;


const int RENDER_DRAW_COLOR=0xff;

#define BLANK_TILE 0
#define TILE_SIZE 80

#define POINT_ITEM_1 1   // TILE 1 IS HEART

#define MAX_MAP_X 1011
#define MAX_MAP_Y 10

#define JOURNEY_SUM 5
#define JOURNEY_EACH_MAP 16170     // int MAP_EACH_JOURNEY = (TILE_SIZE * MAX_MAP_X) / JOURNEY_SUM  ... MAP_EACH_JOURNEY % MAP_RUN == 0

extern bool winner;
extern int minus;

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

namespace SDLCommonFunc
{
    bool CheckCollision(const SDL_Rect& object1, const SDL_Rect& object2);




}




#endif
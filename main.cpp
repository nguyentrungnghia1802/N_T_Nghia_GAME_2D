/**
 * Code demo SDL2
 * Create SDL window.
 * Hướng dẫn:
    Các thao tác build, chạy app đều thao tác bằng Terminal.
    Nếu không có Terminal bấm "      Ctrl + `       " để gọi.

    build file bằng lệnh sau ( chỉ áp dụng với project SDL2 ):

    g++ -Isrc/include -Lsrc/lib -o main main.cpp CommonFunc.cpp BaseObject.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

    Nhớ "Ctrl + S" trước khi build file.

    chạy file bằng lệnh "./main"

    Có vấn đề về lỗi vui lòng liên hệ Q.

    3:55pm 4/3/2024 

**/
/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <stdio.h>
#include "CommonFunc.h"

BaseObject g_background;

bool InitData()
{
    bool success=true;
    int ret=SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0)
    return false;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY."1");

    g_window = SDL_CreateWindow("Gane Cpp SDL 2.0- Blog: Phattrienphanmem123az", SDL_WINDOWPOS_UNDERFINED,
                                                                                 SDL_WINDOWPOS_UNDERFINED,
                                                                                 SCREEN_WIDTH,
                                                                                 SCREEN_HEIGHT,
                                                                                 SDL_WINDOW_SHOWN);
    if(g_window==NULL)
    {
        success=false;
    }
    else
    {
        g_screen=SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if(g_screen=NULL)
        success = false;
        else
        {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags=IMG_INIT_PNG;
            if(!(IMG_Init(imgFlags) && imgFlags))
            success=false;
        }
    }

    return success;
}

bool LoadBackground()
{
    bool set = g_background.LoadImg("background.png", g_screen);
    if (ret==false)
        return false;

    return true;

}


int main(int argc, char* argv[])
{
    if (InitData()==false)
        return -1;


        //...............LOADING
	return 0;
}




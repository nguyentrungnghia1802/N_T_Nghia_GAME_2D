/**
 * Code demo SDL2
 * Create SDL window.
 * Hướng dẫn:
    Các thao tác build, chạy app đều thao tác bằng Terminal.
    Nếu không có Terminal bấm "      Ctrl + `       " để gọi.

    build file bằng lệnh sau ( chỉ áp dụng với project SDL2 ):

    g++ -Isrc/include -Lsrc/lib -o main main.cpp -lmingw32 -lSDL2main -lSDL2

    Nhớ "Ctrl + S" trước khi build file.

    chạy file bằng lệnh "./main"

    Có vấn đề về lỗi vui lòng liên hệ Q.

    3:55pm 4/3/2024 

**/
#include <iostream>
#include "src/include/SDL2/SDL.h"

using std::cin;
using std::cout;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

SDL_Window* gwindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* BackGround = NULL;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO)<0) {
        cout<<"SDL Could not Initialize, SDL Error:\n"<<SDL_GetError();
        return false;
    }
    else {
        gwindow = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gwindow == NULL) {
            cout<<"Window could not be created\nSDL Error:\n"<<SDL_GetError();
            return false;
        }
        else {
            gScreenSurface = SDL_GetWindowSurface(gwindow);
        }
    }
    return true;
}

bool loadmedia(const char path[], SDL_Surface*& Carry_Surface) {
    Carry_Surface = SDL_LoadBMP(path);
    return Carry_Surface != NULL;
}

void stay_window() {
    SDL_Event e;
    bool quit = false;
    while(!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } 
        }
    }
}
int main(int argc, char* args[]) {
    if (init()) {
        stay_window();
    }
    return 0;
}




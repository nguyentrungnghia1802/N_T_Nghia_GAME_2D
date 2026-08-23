#define SDL_MAIN_HANDLED
#include "../src/MainObject.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace
{
void Expect(bool condition, const char *name)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << name << '\n';
        std::exit(1);
    }
}
}

int main()
{
    MainObject player;
    player.SetXPos(500.0f);

    SDL_Event move_right = {};
    move_right.type = SDL_KEYDOWN;
    move_right.key.keysym.sym = SDLK_d;
    player.HandelInputAction(move_right, NULL);

    Map map = {};
    map.max_x_ = MAX_MAP_X * TILE_SIZE;
    map.max_y_ = MAX_MAP_Y * TILE_SIZE;

    player.PrepareRespawn(1.0f);
    player.DoPlayer(map, NULL, 1.0f);
    Expect(std::fabs(player.GetXPos() - 500.0f) < 0.001f,
           "respawn does not add a forward offset");

    player.DoPlayer(map, NULL, 1.0f);
    Expect(std::fabs(player.GetXPos() - 500.0f) < 0.001f,
           "held movement input does not survive respawn");

    std::cout << "respawn input tests passed\n";
    return 0;
}

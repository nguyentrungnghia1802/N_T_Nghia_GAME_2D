#define SDL_MAIN_HANDLED
#include "../src/gamemap.h"

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

void ExpectTightVisibleRange(const GameMap &game_map, const Map &map)
{
    const GameMap::TileRange range = game_map.GetVisibleTileRange(map);
    const int right_edge = map.start_x_ + SCREEN_WIDTH;
    const int bottom_edge = map.start_y_ + SCREEN_HEIGHT;

    Expect(range.first_x == map.start_x_ / TILE_SIZE, "first visible column");
    Expect(range.first_y == map.start_y_ / TILE_SIZE, "first visible row");
    Expect(range.last_x * TILE_SIZE < right_edge, "last column intersects viewport");
    Expect(range.last_y * TILE_SIZE < bottom_edge, "last row intersects viewport");
    if (range.last_x < MAX_MAP_X - 1)
    {
        Expect((range.last_x + 1) * TILE_SIZE >= right_edge, "no extra right column");
    }
    if (range.last_y < MAX_MAP_Y - 1)
    {
        Expect((range.last_y + 1) * TILE_SIZE >= bottom_edge, "no extra bottom row");
    }
}
}

int main()
{
    GameMap game_map;
    Expect(game_map.LoadMap("res/pic/map/map01.txt"), "valid map loads");

    Map &runtime_map = game_map.GetMap();
    Map *runtime_address = &runtime_map;
    const int original_tile = runtime_map.tile[0][0];
    runtime_map.tile[0][0] = original_tile == BLANK_TILE ? 2 : BLANK_TILE;
    game_map.ResetFromBaseMap();
    Expect(&game_map.GetMap() == runtime_address, "runtime map reference remains stable");
    Expect(runtime_map.tile[0][0] == original_tile, "restart restores base tiles");

    const int start_x = runtime_map.start_x_;
    game_map.MapRun(0.5f);
    game_map.MapRun(0.5f);
    Expect(runtime_map.start_x_ == start_x + MAP_RUN, "split camera step preserved");
    game_map.ResetFromBaseMap();
    Expect(runtime_map.start_x_ == start_x, "restart restores camera");

    runtime_map.start_x_ = 0;
    runtime_map.start_y_ = 0;
    ExpectTightVisibleRange(game_map, runtime_map);
    runtime_map.start_x_ = TILE_SIZE - 1;
    ExpectTightVisibleRange(game_map, runtime_map);
    runtime_map.start_x_ = MAX_MAP_X * TILE_SIZE - SCREEN_WIDTH;
    ExpectTightVisibleRange(game_map, runtime_map);

    std::cout << "map system tests passed\n";
    return 0;
}

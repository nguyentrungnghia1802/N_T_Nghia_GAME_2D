
#include <iostream>
#include "gamemap.h"
#include <algorithm>
#include <fstream>
#include <vector>

GameMap::GameMap()
{
    game_map_ = {};
    base_map_ = {};
    has_base_map_ = false;
}

GameMap::~GameMap()
{
    FreeTiles();
}

bool GameMap::LoadMap(const char path[])
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Unable to load map " << path << "\n";
        return false;
    }

    Map loaded_map = {};

    for (int i = 0; i < MAX_MAP_Y; i++)
    {
        for (int j = 0; j < MAX_MAP_X; j++)
        {
            int val = 0;
            if (!(file >> val))
            {
                std::cerr << "Map " << path << " does not contain " << MAX_MAP_X * MAX_MAP_Y << " tile values\n";
                return false;
            }
            if (val < BLANK_TILE || val >= LOADED_TILE_COUNT)
            {
                std::cerr << "Map " << path << " contains unsupported tile id " << val << " at " << j << "," << i << "\n";
                return false;
            }

            loaded_map.tile[i][j] = val;
            if (val > 0)
            {
                if (j > loaded_map.max_x_)
                {
                    loaded_map.max_x_ = j;
                }

                if (i > loaded_map.max_y_)
                {
                    loaded_map.max_y_ = i;
                }
            }
        }
    }

    int extra_value = 0;
    if (file >> extra_value)
    {
        std::cerr << "Map " << path << " contains more than " << MAX_MAP_X * MAX_MAP_Y << " tile values\n";
        return false;
    }

    loaded_map.max_x_ = (loaded_map.max_x_ + 1) * TILE_SIZE;
    loaded_map.max_y_ = (loaded_map.max_y_ + 1) * TILE_SIZE;

    loaded_map.start_x_ = 0;
    loaded_map.start_y_ = 0;
    loaded_map.file_name_ = path;

    game_map_ = loaded_map;
    base_map_ = game_map_;
    has_base_map_ = true;
    return true;
}

bool GameMap::LoadTiles(SDL_Renderer *screen)
{
    const char *map_paths[LOADED_TILE_COUNT] = {
        "res/pic/map/0.png", "res/pic/map/1.png", "res/pic/map/2.png", "res/pic/map/3.png",
        "res/pic/map/4.png", "res/pic/map/5.png", "res/pic/map/6.png", "res/pic/map/7.png"};

    for (int i = 0; i < LOADED_TILE_COUNT; ++i)
    {
        if (!tile_mat[i].LoadImg(map_paths[i], screen))
        {
            std::cerr << "Unable to load tile texture " << map_paths[i] << "\n";
            FreeTiles();
            return false;
        }
    }
    return true;
}

void GameMap::FreeTiles()
{
    for (int i = 0; i < MAX_TILES; ++i)
    {
        tile_mat[i].Free();
    }
}

void GameMap::DrawMap(SDL_Renderer *screen)
{
    int x1 = 0;

    int y1 = 0;

    x1 = (game_map_.start_x_ % TILE_SIZE) * -1;
    y1 = (game_map_.start_y_ % TILE_SIZE) * -1;

    const TileRange visible_range = GetVisibleTileRange(game_map_);
    int draw_y = y1;
    for (int map_y = visible_range.first_y; map_y <= visible_range.last_y; map_y++, draw_y += TILE_SIZE)
    {
        int draw_x = x1;
        for (int map_x = visible_range.first_x; map_x <= visible_range.last_x; map_x++, draw_x += TILE_SIZE)
        {
            int val = game_map_.tile[map_y][map_x];
            if (val > BLANK_TILE && val < LOADED_TILE_COUNT)
            {
                tile_mat[val].SetRect(draw_x, draw_y);
                tile_mat[val].Render(screen);
            }
        }
    }
}

GameMap::TileRange GameMap::GetVisibleTileRange(const Map &map_data) const
{
    TileRange range;
    range.first_x = std::max(0, map_data.start_x_ / TILE_SIZE);
    range.last_x = std::min(MAX_MAP_X - 1, (map_data.start_x_ + SCREEN_WIDTH + TILE_SIZE - 1) / TILE_SIZE);
    range.first_y = std::max(0, map_data.start_y_ / TILE_SIZE);
    range.last_y = std::min(MAX_MAP_Y - 1, (map_data.start_y_ + SCREEN_HEIGHT + TILE_SIZE - 1) / TILE_SIZE);
    return range;
}

void GameMap::ResetFromBaseMap()
{
    if (has_base_map_)
    {
        game_map_ = base_map_;
    }
}

void GameMap::ResetMap(Map &map_data)
{
    if (winner == true)
    {
        map_data.start_x_ = 0;
    }
    else if (winner == false)
    {
        if (map_start < JOURNEY_EACH_MAP * 1 + 280)
        {
            map_data.start_x_ = 0;
        }
        else if (map_start >= JOURNEY_EACH_MAP * 1 + 280 && map_start < JOURNEY_EACH_MAP * 2 + 280)
        {
            map_data.start_x_ = JOURNEY_EACH_MAP * 1 + 280;
        }
        else if (map_start >= JOURNEY_EACH_MAP * 2 + 280)
        {
            map_data.start_x_ = JOURNEY_EACH_MAP * 2 + 280;
        }
    }
}

bool GameMap::LoadMap_Return(const char path[])
{
    if (!LoadMap(path))
    {
        return false;
    }

    ResetMap(game_map_);
    return true;
}

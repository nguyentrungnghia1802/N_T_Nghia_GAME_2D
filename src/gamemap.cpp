
#include <iostream>
#include "gamemap.h"
#include <algorithm>
#include <fstream>
#include <vector>

GameMap::GameMap()
{
    has_base_map_ = false;
}

void GameMap::LoadMap(const char path[])
{
    std::ifstream file;
    file.open(path);
    if (!file.is_open())
    {
        std::cout << "Uable to load " << path << "\n";
        return;
    }

    game_map_.max_x_ = 0;
    game_map_.max_y_ = 0;

    for (int i = 0; i < MAX_MAP_Y; i++)
    {
        for (int j = 0; j < MAX_MAP_X; j++)
        {
            file >> game_map_.tile[i][j];
            int val = game_map_.tile[i][j];
            if (val > 0)
            {
                if (j > game_map_.max_x_)
                {
                    game_map_.max_x_ = j;
                }

                if (i > game_map_.max_y_)
                {
                    game_map_.max_y_ = i;
                }
            }
        }
    }

    game_map_.max_x_ = (game_map_.max_x_ + 1) * TILE_SIZE;
    game_map_.max_y_ = (game_map_.max_y_ + 1) * TILE_SIZE;

    game_map_.start_x_ = 0;
    game_map_.start_y_ = 0;

    file.close();
    base_map_ = game_map_;
    has_base_map_ = true;
}

void GameMap::LoadTiles(SDL_Renderer *screen)
{
    std::vector<std::string> map_path = {"res/pic/map/0.png", "res/pic/map/1.png", "res/pic/map/2.png", "res/pic/map/3.png", "res/pic/map/4.png", "res/pic/map/5.png", "res/pic/map/6.png", "res/pic/map/7.png"};
    tile_mat[0].LoadImg(map_path[0], screen);
    tile_mat[1].LoadImg(map_path[1], screen); // heart
    tile_mat[2].LoadImg(map_path[2], screen);
    tile_mat[3].LoadImg(map_path[3], screen);
    tile_mat[4].LoadImg(map_path[4], screen); //  4 + 5 + 6 + 7 is Isha_character_Image
    tile_mat[5].LoadImg(map_path[5], screen);
    tile_mat[6].LoadImg(map_path[6], screen);
    tile_mat[7].LoadImg(map_path[7], screen);
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
            if (val > 0)
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

void GameMap::LoadMap_Return(const char path[])
{
    std::ifstream file;
    file.open(path);
    if (!file.is_open())
    {
        std::cout << "Uable to load " << path << "\n";
        return;
    }

    game_map_.max_x_ = 0;
    game_map_.max_y_ = 0;

    for (int i = 0; i < MAX_MAP_Y; i++)
    {
        for (int j = 0; j < MAX_MAP_X; j++)
        {
            file >> game_map_.tile[i][j];
            int val = game_map_.tile[i][j];
            if (val > 0)
            {
                if (j > game_map_.max_x_)
                {
                    game_map_.max_x_ = j;
                }

                if (i > game_map_.max_y_)
                {
                    game_map_.max_y_ = i;
                }
            }
        }
    }

    game_map_.max_x_ = (game_map_.max_x_ + 1) * TILE_SIZE;
    game_map_.max_y_ = (game_map_.max_y_ + 1) * TILE_SIZE;

    if (winner == true)
    {
        game_map_.start_x_ = 0;
    }
    else if (winner == false)
    {
        if (map_start < JOURNEY_EACH_MAP * 1 + 280)
        {
            game_map_.start_x_ = 0;
        }
        else if (map_start >= JOURNEY_EACH_MAP * 1 + 280 && map_start < JOURNEY_EACH_MAP * 2 + 280)
        {
            game_map_.start_x_ = JOURNEY_EACH_MAP * 1 + 280;
        }
        else if (map_start >= JOURNEY_EACH_MAP * 2 + 280)
        {
            game_map_.start_x_ = JOURNEY_EACH_MAP * 2 + 280;
        }
    }

    file.close();
}

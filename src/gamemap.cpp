
#include <iostream>
#include "gamemap.h"
#include <fstream>
#include <vector>

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
    int x2 = 0;

    int y1 = 0;
    int y2 = 0;

    int map_x = 0;
    int map_y = 0;

    map_x = game_map_.start_x_ / TILE_SIZE;
    x1 = (game_map_.start_x_ % TILE_SIZE) * -1;
    x2 = x1 + SCREEN_WIDTH + (x1 == 0 ? 0 : TILE_SIZE);

    map_y = game_map_.start_y_ / TILE_SIZE;

    y1 = (game_map_.start_y_ % TILE_SIZE) * -1;
    y2 = y1 + SCREEN_HEIGHT + (y1 == 0 ? 0 : TILE_SIZE);

    for (int i = y1; i < y2; i += TILE_SIZE)
    {
        map_x = game_map_.start_x_ / TILE_SIZE;
        for (int j = x1; j < x2; j += TILE_SIZE)
        {
            int val = game_map_.tile[map_y][map_x];
            if (val > 0)
            {
                tile_mat[val].SetRect(j, i);
                tile_mat[val].Render(screen);
            }
            map_x++;
        }
        map_y++;
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
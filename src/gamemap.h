// Xay dung cau truc map
#ifndef GAME_MAP_H_
#define GAME_MAP_H_

#include "CommonFunc.h"
#include "BaseObject.h"

#define MAP_RUN 6
const int LOADED_TILE_COUNT = 8;

class TileMat : public BaseObject
{
public:
    TileMat() { ; }
    ~TileMat() { ; }
};

class GameMap
{
public:
    struct TileRange
    {
        int first_x;
        int last_x;
        int first_y;
        int last_y;
    };

    GameMap();
    ~GameMap();

    bool LoadMap(const char path[]);
    bool LoadTiles(SDL_Renderer *screen);
    void FreeTiles();
    void DrawMap(SDL_Renderer *screen);
    Map &GetMap() { return game_map_; }
    const Map &GetMap() const { return game_map_; }
    void MapRun(float frame_scale);
    void ResetMap();
    void ResetFromBaseMap();
    TileRange GetVisibleTileRange(const Map &map_data) const;

private:
    Map game_map_;
    Map base_map_;
    bool has_base_map_;
    float map_run_remainder_;
    TileMat tile_mat[LOADED_TILE_COUNT];
};

#endif

// Xay dung cau truc map
#ifndef GMAE_MAP_H
#define GAME_MAP_H

#include "CommonFunc.h"
#include "BaseObject.h"

#define MAX_TILES 20
#define MAP_RUN 6

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
    ~GameMap() { ; }

    void LoadMap(const char path[]);
    void LoadMap_Return(const char path[]);
    void LoadTiles(SDL_Renderer *screen);
    void DrawMap(SDL_Renderer *screen);
    Map getMap() const { return game_map_; };
    void SetMap(Map &map_data) { game_map_ = map_data; }
    void MapRun(Map &map_data) { map_data.start_x_ += MAP_RUN; }
    void ResetMap(Map &map_data);
    void ResetFromBaseMap();
    TileRange GetVisibleTileRange(const Map &map_data) const;

private:
    Map game_map_;
    Map base_map_;
    bool has_base_map_;
    TileMat tile_mat[MAX_TILES];
};

#endif

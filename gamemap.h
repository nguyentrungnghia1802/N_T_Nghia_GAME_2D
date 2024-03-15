//Xay dung cau truc map
#ifndef GMAE_MAP_H
#define GAME_MAP_H

#include "CommonFunc.h"
#include "BaseObject.h"

#define MAX_TILES 20

class TileMat : public BaseObject
{
    public:
    TileMat(){;}
    ~TileMat(){;}
};


class GameMap
{
public:
    GameMap(){;}
    ~GameMap(){;}

    void LoadMap(const char path[]);
    void LoadTiles(SDL_Renderer* screen);
    void DrawMap(SDL_Renderer* screen);
    Map getMap() const {return game_map_;};
private: 
    Map game_map_;
    TileMat tile_mat[MAX_TILES];
};

#endif

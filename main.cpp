

#include <stdio.h>
#include <iostream>
#include "CommonFunc.h"
#include "BaseObject.h"
#include "gamemap.h"
#include "MainObject.h"
#include "ImpTimer.h"
#include "ThreatObject.h"


BaseObject g_background;

bool InitData()
{
    bool success=true;
    int ret=SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0)
        return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    g_window = SDL_CreateWindow("Game 2d", 
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SCREEN_WIDTH, SCREEN_HEIGHT,
                                 SDL_WINDOW_SHOWN);
    if (g_window == NULL)
    {
        success=false;
    }

    else
    {
        g_screen=SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if(g_screen==NULL)
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
    bool ret = g_background.LoadImg("img/background.jpg", g_screen);
    if (ret==false)
        return false;

    return true;
}

void close()
{
    g_background.Free();

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window=NULL;

    IMG_Quit();
    SDL_Quit();
}


std::vector <ThreatsObject* >MakeThreats()
{
    std::vector <ThreatsObject*> list_threats;


    ThreatsObject* dynamic_threats=new ThreatsObject[20];

    for(int i=0; i<NUM_THREATS_LIST;i++)
    {
        ThreatsObject* p_threat = (dynamic_threats + 1);

        if(p_threat!=NULL)
        {
            p_threat->LoadImg(" img/threat_2_left.png",g_screen);
            p_threat-> set_clips();
            p_threat->set_type_move(ThreatsObject::MOVE_INSPACE_THREAT);
            p_threat->set_x_pos(500+i*1000);
            p_threat->set_y_pos(230);

            int pos1=p_threat-> get_x_pos() - 60;
            int pos2=p_threat->get_x_pos() + 60;

            list_threats.push_back(p_threat);
        }
    }


    ThreatsObject* threatsobj = new ThreatsObject[NUM_THREATS_LIST];

    for(int i=0; i<NUM_THREATS_LIST;i++)
    {
        ThreatsObject* p_threat=(threatsobj+i);
        if(p_threat!=NULL)
        {
            p_threat->LoadImg("img/threat_1.png",g_screen);        // loading threats
            p_threat->set_clips();
            p_threat->set_x_pos(2000+i*2000);
            p_threat->set_y_pos(250);

            list_threats.push_back(p_threat);
        }
    }
    return list_threats;
}



int main(int argc, char* argv[])
{
    ImpTimer fps_timer;

    if (InitData()==false)
        return -1;

    if (LoadBackground()==false)
        return -1;


    GameMap game_map;
    game_map.LoadMap("map/map01.txt");
    game_map.LoadTiles(g_screen);


    MainObject p_player;
    p_player.LoadImg("img/player_right1.png",g_screen);
    p_player.set_clips();


    std::vector <ThreatsObject*> threats_list = MakeThreats();


    bool is_quit = false;
    while (!is_quit)
    {
        fps_timer.start();
        while (SDL_PollEvent(&g_event)!=0)
        {
            if (g_event.type==SDL_QUIT)
            {
                is_quit=true;
            }

            p_player.HandelInputAction(g_event,g_screen);
        }

        SDL_SetRenderDrawColor(g_screen,RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
        SDL_RenderClear(g_screen);

        g_background.Render(g_screen, NULL);

        Map map_data=game_map.getMap();

        p_player.HanleBullet(g_screen);
        p_player.SetMapXY(map_data.start_x_,map_data.start_y_);
        p_player.DoPlayer(map_data);
        p_player.Show(g_screen);
        

        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);


        for (int i=0; i<threats_list.size(); i++)
        {
            ThreatsObject* p_threat = threats_list.at(i);
            if(p_threat!=NULL)
            
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen);
        }


        SDL_RenderPresent(g_screen);
        
        int real_imp_time=fps_timer.get_ticks();
        int time_one_frame=1000/FRAME_PER_SECOND;

        if(real_imp_time < time_one_frame)
        {
            int delay_time=time_one_frame-real_imp_time;
            if(delay_time>=0)
            SDL_Delay(delay_time);
        }

    }

    close();
	return 0;
}




#include <stdio.h>
#include <iostream>
#include "CommonFunc.h"
#include "BaseObject.h"
#include "gamemap.h"
#include "MainObject.h"
#include "ImpTimer.h"
#include "ThreatObject.h"
#include "PlayText.h"


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


    ThreatsObject* dynamic_threats=new ThreatsObject[NUM_THREATS_LIST];

    for(int i=0; i<NUM_THREATS_LIST;i++)
    {
        ThreatsObject* p_threat = (dynamic_threats + i);

        if(p_threat!=NULL)
        {
            p_threat->LoadImg("img/threat_2_left.png",g_screen);
            p_threat-> set_clips();
            p_threat->set_type_move(ThreatsObject::MOVE_INSPACE_THREAT);
            p_threat->set_x_pos(500+i*1000);
            p_threat->set_y_pos(200);

            int pos1=p_threat-> get_x_pos() - 200;
            int pos2=p_threat->get_x_pos() + 200;
            p_threat->SetAnimationPos(pos1, pos2);
            p_threat->set_input_left(1);

            list_threats.push_back(p_threat);
        }
    }


    ThreatsObject* ThreatFly = new ThreatsObject[NUM_THREATS_LIST];

    for(int i=0; i<NUM_THREATS_LIST;i++)
    {
        ThreatsObject* p_threat=(ThreatFly+i);
        if(p_threat!=NULL)
        {
            p_threat->LoadImg("img/threat_3_left.png",g_screen);        // loading threats
            p_threat->set_clips();
            p_threat->set_x_pos(290+i*5000);
            p_threat->set_y_pos(250);
            p_threat->set_type_move(ThreatsObject::THREATS_FLY_STATIC);

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


    PlayerPower player_power;
    player_power.Init(g_screen);






    std::vector <ThreatsObject*> threats_list = MakeThreats();


    int num_die=0;


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

        player_power.Show(g_screen); 
        bool die = p_player.FallToHole(map_data);



        for (int i=0; i<threats_list.size(); i++)
        {
            ThreatsObject* p_threat = threats_list.at(i);
            if(p_threat!=NULL)
            {
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen);


                SDL_Rect rect_player=p_player.GetRectFrame();
                SDL_Rect rect_threat=p_threat->GetRectFrame();
                bool bCol2 = SDLCommonFunc::CheckCollision(rect_player,rect_threat);
                


                if(bCol2||die)
                {
                    num_die++;
                    if(num_die<=3)
                    {
                        die=false;
                        p_player.SetRect(0,0);
                        p_player.set_comeback_time(3);
                        SDL_Delay(1000);
                        player_power.Decrease();
                        player_power.Render(g_screen);
                        continue;
                    }
                    else
                    {
                        if(MessageBoxW(NULL,L"Game Over",L"Info",MB_OK | MB_ICONSTOP)==IDOK)
                        {
                            p_threat->Free();
                            close();
                            SDL_Quit();
                            return 0;
                        }
                    }
                }
            }
        }


        std::vector<BulletObject*> bullet_arr = p_player.get_bullet_list();
        for (int r = 0; r< bullet_arr.size();r++)
        {
            BulletObject* p_bullet=bullet_arr.at(r);
            {
                if(p_bullet!=NULL)
                {
                    for(int t=0; t<threats_list.size();t++)
                    {
                        ThreatsObject* obj_threat = threats_list.at(t);
                        if(obj_threat!=NULL)
                        {
                            SDL_Rect tRect;
                            tRect.x=obj_threat->GetRect().x;
                            tRect.y=obj_threat->GetRect().y;
                            tRect.w=obj_threat->get_width_frame();
                            tRect.h=obj_threat->get_height_frame();

                            SDL_Rect bRect=p_bullet->GetRect();

                            bool bCol=SDLCommonFunc::CheckCollision(bRect,tRect);

                            if(bCol)
                            {
                                p_player.RemoveBullet(r);
                                obj_threat->Free();
                                threats_list.erase(threats_list.begin()+t);
                            }



                        }
                    }
                }
            }
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





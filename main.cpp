#include <stdio.h>
#include <iostream>
#include "CommonFunc.h"
#include "BaseObject.h"
#include "gamemap.h"
#include "MainObject.h"
#include "ImpTimer.h"
#include "ThreatObject.h"
#include "PlayHealth.h"
#include "TextObject.h"

BaseObject g_background;
ImpTimer fps_timer;
GameMap game_map;
MainObject p_player;
PlayerPower player_power; // HP survival
PlayerMoney player_heart; // point
TextObject time_game;
TextObject heart_game;
Map map_data;

TTF_Font *font_time = NULL;
TTF_Font *font_heart = NULL;
TTF_Font *gFont1 = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
TTF_Font *gFont4 = NULL;

Mix_Music *gMusic = NULL;
Mix_Chunk *gMainMusic = NULL;
Mix_Chunk *gGame_Start = NULL;
Mix_Chunk *gThreats_Die = NULL;

SDL_Surface *g_img_menu;
SDL_Event eve;
SDL_Event eve_win;
SDL_Texture *menu;
SDL_Rect menuRect;
SDL_Surface *gWin_game;
SDL_Texture *WinGame;
SDL_Rect WinGameRect;

std::vector<ThreatsObject *> threats_list;
std::vector<BulletObject *> bullet_arr; // bullet
std::string heart_str;

bool isRestarting = false; // Replay game if game over
bool is_quit = false;      // Turn off game
bool start_Game = false;   // After that, we can Play Game
bool bCol2 = false;        // Collide:   Player and  Threats

int num_die = 0;
int heart_count = 0;

void Restart(Map &map_data, int &num_die, int &heart_count, MainObject &p_player, PlayerPower &player_power, std::vector<ThreatsObject *> threats_list);
bool InitData();
bool LoadBackground();
void close();

std::vector<ThreatsObject *> MakeThreats();
void renderText(const std::string &text, int x, int y, TTF_Font *font);
void LoadFromFile();
void load_Menu();
void Render_Menu();
void destroy_Menu();
void Call_Menu();

void Win_Game()
{          
    bool replay_game = false;
    while (replay_game == false)
    {
        SDL_RenderCopy(g_screen, WinGame, NULL, &WinGameRect);
        renderText("POINT: ", SCREEN_WIDTH / 2 - 280, 220, gFont3);
        renderText(std::to_string(heart_count).c_str(), SCREEN_WIDTH / 2 + 40, 220, gFont3);
        SDL_RenderPresent(g_screen);
        while (SDL_PollEvent(&eve_win))
        {
            if (eve_win.type == SDL_KEYDOWN && eve_win.key.keysym.sym == SDLK_SPACE)
            {
                Mix_PlayChannel(-1, gGame_Start, 0);
                isRestarting = true;
                replay_game=true;
            }
            if (eve_win.type == SDL_KEYDOWN && eve_win.key.keysym.sym == SDLK_ESCAPE)
            {
                replay_game=true;
                is_quit = true;
            }
        }
    }
    replay_game=false;
}

int main(int argc, char *argv[])
{
    std::srand(time(NULL));
    if (InitData() == false)
        return -1;

    if (LoadBackground() == false)
        return -1;

    LoadFromFile();

    game_map.LoadTiles(g_screen); // Load Map

    p_player.set_clips(); // Load Main Player

    // Load and set position HP_player  and   Heart_point
    player_power.Init(g_screen);
    player_heart.Init(g_screen);
    player_heart.SetPos(SCREEN_WIDTH * 0.5 - 12, 42);

    // Text
    time_game.SetColor(TextObject::WHITE_TEXT);
    heart_game.SetColor(TextObject::RED_TEXT);

    // MENU
    Call_Menu();

    threats_list = MakeThreats();

    WinGame = SDL_CreateTextureFromSurface(g_screen, gWin_game); //    Load background Win_Game
    WinGameRect = {0, 0, gWin_game->w, gWin_game->h};  

    while (!is_quit)
    {
        if (isRestarting)
        {
            for (int i = 0; i < threats_list.size(); i++)
            {
                ThreatsObject *p_threat = threats_list.at(i);
                if (p_threat != NULL)
                {
                    p_threat->Free();
                    break;
                }
            }
            threats_list.clear();
            threats_list = MakeThreats();
            Restart(map_data, num_die, heart_count, p_player, player_power, threats_list);
            isRestarting = !isRestarting;
        }

        heart_count = p_player.GetMoneyCount();

        //            FPS
        fps_timer.start();
        while (SDL_PollEvent(&g_event) != 0)
        {
            if (g_event.type == SDL_QUIT)
            {
                is_quit = true;
            }

            p_player.HandelInputAction(g_event, g_screen, gFire_ball);
        }

        SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
        SDL_RenderClear(g_screen);

        g_background.Render(g_screen, NULL);

        //             MAP
        map_data = game_map.getMap();
        game_map.MapRun(map_data);

        //            PLAYER
        p_player.HanleBullet(g_screen);
        p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
        p_player.DoPlayer(map_data, gEarn_Heart);
        p_player.Show(g_screen);

        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);

        player_power.Show(g_screen);
        player_heart.Show(g_screen);

        bool is_minusLinve = p_player.GetIsMinusLive();

        for (int i = 0; i < threats_list.size(); i++)
        {
            ThreatsObject *p_threat = threats_list.at(i);
            if (p_threat != NULL)
            {
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen);

                SDL_Rect rect_player = p_player.GetRectFrame();
                SDL_Rect rect_threat = p_threat->GetRectFrame();
                bCol2 = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
                if (bCol2 == true)
                {
                    p_threat->Free();
                    threats_list.erase(threats_list.begin() + i);
                    break;
                }
            }
        }

        if (bCol2 || is_minusLinve == true)
        {
            Mix_PlayChannel(-1, gPlayer_Die, 0);
            num_die++;
            if (is_minusLinve == true)
            {
                p_player.RetsetMinusLive();
            }

            if (num_die <= 3)
            {
                p_player.SetRect(0, 0);
                p_player.set_comeback_time(3);
                SDL_Delay(1000);
                player_power.Decrease();
                player_power.Render(g_screen);
                continue;
            }
            else
            {
                bool quit_game_over = false;

                while (quit_game_over == false)
                {

                    renderText("POINT: ", SCREEN_WIDTH / 2 - 280, 220, gFont3);
                    renderText(std::to_string(heart_count).c_str(), SCREEN_WIDTH / 2 + 40, 220, gFont3);

                    renderText("SPACE TO REPLAY!", SCREEN_WIDTH / 2 - 420, 380, gFont3);
                    SDL_RenderPresent(g_screen);
                    while (SDL_PollEvent(&eve))
                    {
                        if (eve.type == SDL_KEYDOWN && eve.key.keysym.sym == SDLK_SPACE)
                        {
                            Mix_PlayChannel(-1, gGame_Start, 0);
                            SDL_Delay(4000);

                            isRestarting = true;
                            quit_game_over = true;
                        }
                        if (eve.type == SDL_KEYDOWN && eve.key.keysym.sym == SDLK_ESCAPE)
                        {
                            quit_game_over = true;
                            is_quit = true;
                        }
                    }
                }
                quit_game_over = false;
            }
            for (int i = 0; i < 8000; i++)
            {
                bCol2 = false;
            }
        }




        if(winner==true)                                                       /////////////////////////////////////////////////////////////
        {
            std::cout<<"yes";
            Win_Game();
            winner=false;
        }




        //      Dan ban
        bullet_arr = p_player.get_bullet_list();
        for (int r = 0; r < bullet_arr.size(); r++)
        {
            BulletObject *p_bullet = bullet_arr.at(r);
            {
                if (p_bullet != NULL)
                {
                    for (int t = 0; t < threats_list.size(); t++)
                    {
                        ThreatsObject *obj_threat = threats_list.at(t);
                        if (obj_threat != NULL)
                        {
                            SDL_Rect tRect;
                            tRect.x = obj_threat->GetRect().x;
                            tRect.y = obj_threat->GetRect().y;
                            tRect.w = obj_threat->get_width_frame();
                            tRect.h = obj_threat->get_height_frame();

                            SDL_Rect bRect = p_bullet->GetRect();

                            bool bCol = SDLCommonFunc::CheckCollision(bRect, tRect);

                            if (bCol)
                            {
                                Mix_PlayChannel(-1, gThreats_Die, 0);
                                p_player.RemoveBullet(r);
                                obj_threat->Free();
                                threats_list.erase(threats_list.begin() + t);
                            }
                        }
                    }
                }
            }
        }


        // Show game time
        std::string str_time = "Days: ";
        Uint32 time_val = SDL_GetTicks() / 1000;
        Uint32 val_time = 0 + time_val;

        if (val_time >= 5000)
        {
            if (MessageBoxW(NULL, L"T-kun lost her!", L"Info", MB_OK | MB_ICONSTOP) == IDOK)
            {
                is_quit = true;
                break;
            }
        }

        else
        {
            std::string str_val = std::to_string(val_time);
            str_time += str_val;

            time_game.SetText(str_time);
            time_game.LoadFromRenderText(font_time, g_screen);
            time_game.RenderText(g_screen, SCREEN_WIDTH - 200, 15);
        }

        heart_str = std::to_string(heart_count);
        heart_game.SetText(heart_str);
        heart_game.LoadFromRenderText(font_heart, g_screen);
        heart_game.RenderText(g_screen, SCREEN_WIDTH * 0.5, 5);

        SDL_RenderPresent(g_screen);

        int real_imp_time = fps_timer.get_ticks();
        int time_one_frame = 1000 / FRAME_PER_SECOND;

        if (real_imp_time < time_one_frame)
        {
            int delay_time = time_one_frame - real_imp_time;
            if (delay_time >= 0)
                SDL_Delay(delay_time);
        }
    }
    return 0;
}

void Restart(Map &map_data, int &num_die, int &heart_count, MainObject &p_player, PlayerPower &player_power, std::vector<ThreatsObject *> threats_list)
{
    game_map.LoadMap("map/map01.txt");
    game_map.LoadTiles(g_screen);
    game_map.ResetMap(map_data);

    p_player.SetXPos(200);
    p_player.HeartCount(0);
    player_power.Init(g_screen);

    num_die = 0;
    heart_count = 0;

    p_player.SetRect(0, 0);
    p_player.set_comeback_time(3);
}

bool InitData()
{
    bool success = true;
    int ret = SDL_Init(SDL_INIT_VIDEO);
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
        success = false;
    }

    else
    {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if (g_screen == NULL)
            success = false;
        else
        {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags))
                success = false;
        }

        if (TTF_Init() == -1)
        {
            success = false;
            std::cout << "khong the mo tep";
        }

        font_time = TTF_OpenFont("font/1.ttf", 35);
        font_heart = TTF_OpenFont("font/1.ttf", SIZE_FONT_HEART);

        if (font_time == NULL)
        {
            success = false;
            std::cout << "khong the mo tep ";
        }

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        {
            printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
            success = false;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        {
            printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
            success = false;
        }
    }

    return success;
}

bool LoadBackground()
{
    bool ret = g_background.LoadImg("img/background.jpg", g_screen);
    if (ret == false)
        return false;

    return true;
}

void close()
{
    g_background.Free();

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

    Mix_FreeChunk(gEarn_Heart);
    Mix_FreeChunk(gMainMusic);
    Mix_FreeChunk(gFire_ball);
    Mix_FreeChunk(gPlayer_Die);
    Mix_FreeChunk(gGame_Start);
    Mix_FreeChunk(gThreats_Die);

    gEarn_Heart = NULL;
    gMainMusic = NULL;
    gFire_ball = NULL;
    gPlayer_Die = NULL;
    gGame_Start = NULL;
    gThreats_Die = NULL;

    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void renderText(const std::string &text, int x, int y, TTF_Font *font)
{
    SDL_Color textColor = {255, 255, 255}; // White color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (textSurface == nullptr)
    {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_screen, textSurface);
    if (texture == nullptr)
    {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect dstRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(g_screen, texture, nullptr, &dstRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);
}

void LoadFromFile()
{
    gFont1 = TTF_OpenFont("font/2.ttf", 30);
    gFont2 = TTF_OpenFont("font/2.ttf", 30);
    gFont3 = TTF_OpenFont("font/1.ttf", 120);
    gFont4 = TTF_OpenFont("font/2.ttf", 100);

    g_img_menu = IMG_Load("menu/menu.png");
    game_map.LoadMap("map/map01.txt");
    p_player.LoadImg("img/player_right1.png", g_screen);

    gMainMusic = Mix_LoadWAV("Music/through_Map_music.wav");
    gEarn_Heart = Mix_LoadWAV("Music/earn_Heart.wav");
    gFire_ball = Mix_LoadWAV("Music/Fire_Ball.wav");
    gPlayer_Die = Mix_LoadWAV("Music/Player_Die.wav");
    gGame_Start = Mix_LoadWAV("Music/Start.wav");
    gThreats_Die = Mix_LoadWAV("Music/Threats_Die.wav");
    gWin_game = IMG_Load("map/WIN_GAME.png");
}

void Render_Menu()
{
    SDL_RenderCopy(g_screen, menu, NULL, &menuRect);
    renderText("SPACE TO START!", SCREEN_WIDTH - 300, 420, gFont1);
    renderText("ESC TO EXIT!", 30, 420, gFont2);
    SDL_RenderPresent(g_screen);
}

void destroy_Menu()
{
    SDL_FreeSurface(g_img_menu);
    SDL_DestroyTexture(menu);
}
void load_Menu()
{
    menu = SDL_CreateTextureFromSurface(g_screen, g_img_menu); //    Load background_menu
    menuRect = {0, 0, g_img_menu->w, g_img_menu->h};           //    set menu_position
}

void Call_Menu()
{
    load_Menu(); // load_Menu_backgound

    while (start_Game == false)
    {
        Render_Menu(); // Render_Menu_Texture

        while (SDL_PollEvent(&eve))
        {
            if (eve.type == SDL_KEYDOWN && eve.key.keysym.sym == SDLK_SPACE)
            {
                start_Game = true; // Ready to Play Game
                Mix_PlayChannel(-1, gGame_Start, 0);
                SDL_Delay(4000);
                Mix_PlayChannel(-1, gMainMusic, -1);
                destroy_Menu();
                break;
            }
            if (eve.type == SDL_KEYDOWN && eve.key.keysym.sym == SDLK_ESCAPE)
            {
                destroy_Menu();
                close(); // Exit Game
            }
        }
    }
}

std::vector<ThreatsObject *> MakeThreats()
{
    std::vector<ThreatsObject *> list_threats;

    ThreatsObject *dynamic_threats = new ThreatsObject[NUM_THREATS_LIST];

    for (int i = 0; i < NUM_THREATS_LIST; i++)
    {
        ThreatsObject *p_threat = (dynamic_threats + i);

        if (p_threat != NULL)
        {
            p_threat->LoadImg("img/threat_2_left.png", g_screen);
            p_threat->set_clips();
            p_threat->set_type_move(ThreatsObject::MOVE_INSPACE_THREAT);
            p_threat->set_x_pos(15500 + i * (1000 + 100 * ((rand() % 3) + 7))); //  Set Threats_position
            p_threat->set_y_pos(200);

            int pos1 = p_threat->get_x_pos() - 100;
            int pos2 = p_threat->get_x_pos() + 100;
            p_threat->SetAnimationPos(pos1, pos2);
            p_threat->set_input_left(1);

            list_threats.push_back(p_threat);
        }
    }

    ThreatsObject *ThreatFly = new ThreatsObject[NUM_THREATS_LIST];

    for (int i = 0; i < NUM_THREATS_LIST; i++)
    {
        ThreatsObject *p_threat = (ThreatFly + i);
        if (p_threat != NULL)
        {
            p_threat->LoadImg("img/threat_3_left.png", g_screen);
            p_threat->set_clips();
            p_threat->set_x_pos(5000 + i * (1380 + 100 * ((rand() % 3) + 3))); //  Set Threats_position
            p_threat->set_y_pos(200 + 10 * (rand() % 5));
            p_threat->set_type_move(ThreatsObject::THREATS_FLY_STATIC);

            list_threats.push_back(p_threat);
        }
    }

    return list_threats;
}

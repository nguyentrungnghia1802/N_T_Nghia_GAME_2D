#include <stdio.h>
#include <iostream>
#include "CommonFunc.h"
#include "BaseObject.h"
#include "gamemap.h"
#include "MainObject.h"
#include "ThreatObject.h"
#include "PlayHealth.h"
#include "TextObject.h"
#include "Profiler.h"
#include <algorithm>
#include <memory>

BaseObject g_background;
BaseObject gMonster;
BaseObject gPlayerLeftTexture;
BaseObject gPlayerRightTexture;
BaseObject gBulletTexture;
BaseObject gThreat1Texture;
BaseObject gThreat2LeftTexture;
BaseObject gThreat2RightTexture;
BaseObject gThreat3LeftTexture;
BaseObject gThreat3RightTexture;
BaseObject gThreat4Texture;

GameMap game_map;
MainObject p_player;
PlayerPower player_power; // HP survival
PlayerMoney player_heart; // point
TextObject time_game;
TextObject heart_game;
TextObject high_score_game;
TextObject text_menu[2];

Map map_data;

TTF_Font *font_time = NULL;
TTF_Font *font_heart = NULL;
TTF_Font *gFont1 = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
TTF_Font *gFont4 = NULL;

Mix_Music *gMusic = NULL; // Don't use now
Mix_Chunk *gMainMusic = NULL;
Mix_Chunk *gGame_Start = NULL;
Mix_Chunk *gThreats_Die = NULL;
Mix_Chunk *gCongrat = NULL;

SDL_Surface *g_img_menu;
SDL_Event eve;
SDL_Event eve_win;
SDL_Texture *menu;
SDL_Rect menuRect;
SDL_Surface *gWin_game;
SDL_Texture *WinGame;
SDL_Rect WinGameRect;

SDL_Surface *journey_Surface_1;
SDL_Surface *journey_Surface_2;
SDL_Surface *journey_Surface_3;
SDL_Surface *journey_Surface_4;
SDL_Surface *journey_Surface_5;
SDL_Texture *journey_Texture_1;
SDL_Texture *journey_Texture_2;
SDL_Texture *journey_Texture_3;
SDL_Texture *journey_Texture_4;
SDL_Texture *journey_Texture_5;
SDL_Rect journey_Rect_1;
SDL_Rect journey_Rect_2;
SDL_Rect journey_Rect_3;
SDL_Rect journey_Rect_4;
SDL_Rect journey_Rect_5;
SDL_Rect start_button;
SDL_Rect quit_button;

Uint32 start_time;
Uint32 current_time;
Uint32 time_render;

using ThreatList = std::vector<std::unique_ptr<ThreatsObject>>;

struct ThreatCollisionTarget
{
    ThreatsObject *threat;
    SDL_Rect rect;
};

ThreatList threats_list;
std::string heart_str;
std::string str_val;
std::string high_score_str;

bool isRestarting = false; // Replay game if game over
bool is_quit = false;      // Turn off game
bool start_Game = false;   // After that, we can Play Game
bool bCol2 = false;        // Collide:   Player and  Threats
bool win_and_restart = false;
bool is_minusLinve = false;
bool focus_mouse = false;

int num_die = 0;
int heart_count = 0;
int high_score = 0;

enum class GameState
{
    MENU,
    PLAYING,
    GAME_OVER,
    WIN,
    JOURNEY,
    QUIT
};

const float MAX_DELTA_TIME = 0.05f;
const int THREAT_ACTIVE_MARGIN = SCREEN_WIDTH;
GameState game_state = GameState::MENU;
Uint32 last_frame_ticks = 0;
float delta_time = 0.0f;

void Restart(Map &map_data, int &num_die, int &heart_count, MainObject &p_player, PlayerPower &player_power);
bool InitData();
bool LoadBackground();
void close();
void FreeSurface(SDL_Surface *&surface);
void DestroyTexture(SDL_Texture *&texture);
void CloseFont(TTF_Font *&font);
void FreeChunk(Mix_Chunk *&chunk);
void FreeMenuResources();
void FreeJourneyResources();
void FreeWinResources();
void renderText(const std::string &text, int x, int y, TTF_Font *font);
void LoadFromFile();
void Call_Menu();
void Win_Game(); // Win_Game when Main Player reach the goal
void render_journey_img();
void Create_texture();
ThreatList MakeThreats();

TTF_Font *OpenProfiledFont(const char *path, int size);
SDL_Surface *LoadProfiledSurface(const char *path);
SDL_Texture *CreateProfiledTextureFromSurface(SDL_Renderer *screen, SDL_Surface *surface);
Mix_Chunk *LoadProfiledWav(const char *path);
void LoadRuntimeTextures();
void FreeRuntimeTextures();
void ConfigureDynamicThreat(ThreatsObject *p_threat);
bool IsThreatActive(const ThreatsObject *p_threat, const Map &map_data);
float UpdateDeltaTime();
void CapFrameRate(Uint32 frame_start_ticks);
bool WaitWithEventPump(Uint32 wait_ms);

int main(int argc, char *argv[])
{
    std::srand(time(NULL));
    if (InitData() == false)
    {
        close();
        return -1;
    }

    Profiler::Init();

    if (LoadBackground() == false)
    {
        close();
        return -1;
    }

    LoadFromFile(); // Load Files

    game_map.LoadTiles(g_screen); // Load Map

    p_player.SetTextureRefs(gPlayerLeftTexture.GetObject(), gPlayerLeftTexture.GetRect().w, gPlayerLeftTexture.GetRect().h,
                            gPlayerRightTexture.GetObject(), gPlayerRightTexture.GetRect().w, gPlayerRightTexture.GetRect().h);
    p_player.SetBulletTextureRef(gBulletTexture.GetObject(), gBulletTexture.GetRect().w, gBulletTexture.GetRect().h);
    p_player.set_clips(); // Load Main Player

    // Load and set position HP_player  and   Heart_point
    player_power.Init(g_screen);
    player_heart.Init(g_screen);
    player_heart.SetPos(SCREEN_WIDTH * 0.5 - 191, 5);

    // Text
    time_game.SetColor(TextObject::WHITE_TEXT);
    heart_game.SetColor(TextObject::RED_TEXT);
    high_score_game.SetColor(TextObject::WHITE_TEXT);

    // MENU
    Call_Menu();
    if (is_quit)
    {
        close();
        return 0;
    }

    threats_list = MakeThreats();

    Create_texture();
    Profiler::StartInterval();
    game_state = GameState::PLAYING;
    last_frame_ticks = SDL_GetTicks();

    //      _START_GAME_
    while (!is_quit)
    {
        const Uint32 frame_start_ticks = SDL_GetTicks();
        delta_time = UpdateDeltaTime();
        Profiler::BeginFrame();

        //      CHECK RESTART
        if (isRestarting)
        {
            threats_list.clear();          // Delete old Threats
            threats_list = MakeThreats();
            Restart(map_data, num_die, heart_count, p_player, player_power);
            isRestarting = !isRestarting;
        }

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

        //        BackGround run
        minus -= 2;
        if (minus <= -SCREEN_WIDTH)
        {
            minus = 0;
        }
        g_background.Render1(g_screen, NULL);

        //          Journey
        render_journey_img();

        //             MAP
        map_data = game_map.getMap();
        if (map_data.start_x_ < MAX_MAP_X * TILE_SIZE - 1500)
        {
            game_map.MapRun(map_data);
        }
        map_start = map_data.start_x_;

        //            PLAYER
        heart_count = p_player.GetMoneyCount();
        p_player.HanleBullet(g_screen);
        p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
        p_player.DoPlayer(map_data, gEarn_Heart);
        p_player.Show(g_screen);

        //            SET MAP
        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);

        //      SHOW_GAME_INFORMATION
        player_power.Show(g_screen);
        player_heart.Show(g_screen);

        is_minusLinve = p_player.GetIsMinusLive();
        bCol2 = false;
        SDL_Rect screen_viewport = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        std::vector<ThreatCollisionTarget> active_threats;
        active_threats.reserve(threats_list.size());

        for (size_t i = 0; i < threats_list.size(); i++)
        {
            ThreatsObject *p_threat = threats_list.at(i).get();
            if (p_threat != NULL)
            {
                if (!IsThreatActive(p_threat, map_data))
                {
                    continue;
                }

                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen, &screen_viewport);

                SDL_Rect rect_player = p_player.GetRectFrame();
                SDL_Rect rect_threat = p_threat->GetRectFrame();
                active_threats.push_back({p_threat, rect_threat});
                bCol2 = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
                if (bCol2 == true)
                {
                    threats_list.erase(threats_list.begin() + i);
                    break;
                }
            }
        }

        //              MONSTER
        gMonster.Render(g_screen, NULL);

        //   Collision
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
                WaitWithEventPump(1000);
                player_power.Decrease();
                player_power.Render(g_screen);
                Profiler::EndFrame();
                continue;
            }
            else                              // When LOSE
            {
                game_state = GameState::GAME_OVER;
                bool quit_game_over = false;

                while (quit_game_over == false)
                {

                    renderText("SCORE: ", SCREEN_WIDTH / 2 - 280, 220, gFont3);
                    renderText(std::to_string(heart_count).c_str(), SCREEN_WIDTH / 2 + 40, 220, gFont3);

                    renderText("SPACE TO REPLAY!", SCREEN_WIDTH / 2 - 420, 380, gFont3);
                    SDL_RenderPresent(g_screen);
                    while (SDL_PollEvent(&eve))
                    {
                        if (eve.type == SDL_KEYDOWN && eve.key.keysym.sym == SDLK_SPACE)         // REPLAY
                        {
                            Mix_PlayChannel(-1, gGame_Start, 0);
                            if (WaitWithEventPump(4000))
                            {
                                isRestarting = true;
                            }
                            quit_game_over = true;
                        }
                        if (eve.type == SDL_KEYDOWN && eve.key.keysym.sym == SDLK_ESCAPE)        // EXIT
                        {
                            quit_game_over = true;
                            is_quit = true;
                        }
                        if(eve.type==SDL_QUIT)
                        {
                            quit_game_over = true;
                            is_quit = true;
                        }
                    }
                    SDL_Delay(1);
                }
                game_state = is_quit ? GameState::QUIT : GameState::PLAYING;
                quit_game_over = false;
            }
        }
        //           Win_Game
        if (winner == true)
        {
            game_state = GameState::WIN;
            start_time = current_time;     //SET_TIME_START_BACK
            Mix_PlayChannel(-1, gCongrat, 0);
            Win_Game();
            if (win_and_restart == true)
            {
                threats_list.clear();
                threats_list = MakeThreats();
                Restart(map_data, num_die, heart_count, p_player, player_power);
                win_and_restart = false;
            }
            winner = false;
            if (!is_quit)
            {
                game_state = GameState::PLAYING;
            }
        }

        //            Bullet
        const std::vector<BulletObject *> &bullet_arr = p_player.get_bullet_list();
        for (size_t r = 0; r < bullet_arr.size();)
        {
            BulletObject *p_bullet = bullet_arr.at(r);
            bool bullet_removed = false;
            {
                if (p_bullet != NULL)
                {
                    for (size_t t = 0; t < active_threats.size() && !bullet_removed; t++)
                    {
                        ThreatCollisionTarget &target = active_threats.at(t);
                        if (target.threat != NULL)
                        {
                            SDL_Rect bRect = p_bullet->GetRect();

                            bool bCol = SDLCommonFunc::CheckCollision(bRect, target.rect);

                            if (bCol)
                            {
                                Mix_PlayChannel(-1, gThreats_Die, 0);
                                p_player.RemoveBullet(r);
                                ThreatsObject *hit_threat = target.threat;
                                ThreatList::iterator hit_it = std::find_if(threats_list.begin(), threats_list.end(),
                                                                            [hit_threat](const std::unique_ptr<ThreatsObject> &threat)
                                                                            {
                                                                                return threat.get() == hit_threat;
                                                                            });
                                if (hit_it != threats_list.end())
                                {
                                    threats_list.erase(hit_it);
                                }
                                active_threats.erase(active_threats.begin() + t);
                                bullet_removed = true;
                            }
                        }
                    }
                }
            }
            if (!bullet_removed)
            {
                r++;
            }
        }

        //    Show game time
        current_time = SDL_GetTicks() / 1000;
        time_render = current_time - start_time;

        //    LIMITED TIME
        if (time_render >= 9999)
        {
            if (MessageBoxW(NULL, L"T-kun lost her!", L"Info", MB_OK | MB_ICONSTOP) == IDOK)
            {
                is_quit = true;
                break;
            }
        }
        else
        {
            str_val = std::to_string(time_render);
            time_game.SetText("Days: " + str_val);
            time_game.LoadFromRenderText(font_time, g_screen);
            time_game.RenderText(g_screen, SCREEN_WIDTH - 200, 15);
        }

        //      HEART_ITEM
        heart_str = std::to_string(heart_count);
        heart_game.SetText(heart_str);
        heart_game.LoadFromRenderText(font_heart, g_screen);
        heart_game.RenderText(g_screen, SCREEN_WIDTH * 0.5 - 140, 5);

        if (heart_count > high_score)
        {
            high_score = heart_count;
        }

        high_score_str = std::to_string(high_score);
        high_score_game.SetText("HIGH SCORE: " + high_score_str);
        high_score_game.LoadFromRenderText(font_heart, g_screen);
        high_score_game.RenderText(g_screen, SCREEN_WIDTH * 0.5 + 40, 5);

        SDL_RenderPresent(g_screen);
        Profiler::EndFrame();

        //        FPS
        CapFrameRate(frame_start_ticks);
    }
    close();
    return 0;
}

void LoadFromFile()
{
    gFont1 = OpenProfiledFont("res/font/2.ttf", 30);
    gFont2 = OpenProfiledFont("res/font/2.ttf", 30);
    gFont3 = OpenProfiledFont("res/font/1.ttf", 120);
    gFont4 = OpenProfiledFont("res/font/2.ttf", 100);
    font_time = OpenProfiledFont("res/font/1.ttf", 35);
    font_heart = OpenProfiledFont("res/font/1.ttf", SIZE_FONT_HEART);

    g_img_menu = LoadProfiledSurface("res/pic/menu/menu.png");
    gWin_game = LoadProfiledSurface("res/pic/map/WIN_GAME.png");
    journey_Surface_1 = LoadProfiledSurface("res/pic/journey/journey_1.png");
    journey_Surface_2 = LoadProfiledSurface("res/pic/journey/journey_2.png");
    journey_Surface_3 = LoadProfiledSurface("res/pic/journey/journey_3.png");
    journey_Surface_4 = LoadProfiledSurface("res/pic/journey/journey_4.png");
    journey_Surface_5 = LoadProfiledSurface("res/pic/journey/journey_5.png");

    game_map.LoadMap("res/pic/map/map01.txt");
    gMonster.LoadImg("res/pic/threats/Monster.png", g_screen);
    LoadRuntimeTextures();

    gMainMusic = LoadProfiledWav("res/Music/through_Map_music.wav");
    gEarn_Heart = LoadProfiledWav("res/Music/earn_Heart.wav");
    gFire_ball = LoadProfiledWav("res/Music/Fire_Ball.wav");
    gPlayer_Die = LoadProfiledWav("res/Music/Player_Die.wav");
    gGame_Start = LoadProfiledWav("res/Music/Start.wav");
    gThreats_Die = LoadProfiledWav("res/Music/Threats_Die.wav");
    gCongrat = LoadProfiledWav("res/Music/Congrats.wav");
}

TTF_Font *OpenProfiledFont(const char *path, int size)
{
    Profiler::CountFontLoad();
    return TTF_OpenFont(path, size);
}

SDL_Surface *LoadProfiledSurface(const char *path)
{
    Profiler::CountImageLoad();
    return IMG_Load(path);
}

SDL_Texture *CreateProfiledTextureFromSurface(SDL_Renderer *screen, SDL_Surface *surface)
{
    Profiler::CountTextureCreate();
    return SDL_CreateTextureFromSurface(screen, surface);
}

Mix_Chunk *LoadProfiledWav(const char *path)
{
    Profiler::CountSoundLoad();
    return Mix_LoadWAV(path);
}

float UpdateDeltaTime()
{
    const Uint32 now = SDL_GetTicks();
    if (last_frame_ticks == 0)
    {
        last_frame_ticks = now;
        return 1.0f / FRAME_PER_SECOND;
    }

    const Uint32 elapsed_ticks = now - last_frame_ticks;
    last_frame_ticks = now;

    const float elapsed_seconds = static_cast<float>(elapsed_ticks) / 1000.0f;
    return std::min(elapsed_seconds, MAX_DELTA_TIME);
}

void CapFrameRate(Uint32 frame_start_ticks)
{
    const Uint32 target_frame_ms = 1000 / FRAME_PER_SECOND;
    const Uint32 frame_ticks = SDL_GetTicks() - frame_start_ticks;
    if (frame_ticks < target_frame_ms)
    {
        SDL_Delay(target_frame_ms - frame_ticks);
    }
}

bool WaitWithEventPump(Uint32 wait_ms)
{
    const Uint32 start_ticks = SDL_GetTicks();
    SDL_Event wait_event;

    while (!is_quit && SDL_GetTicks() - start_ticks < wait_ms)
    {
        while (SDL_PollEvent(&wait_event) != 0)
        {
            if (wait_event.type == SDL_QUIT)
            {
                is_quit = true;
                game_state = GameState::QUIT;
                return false;
            }
            if (wait_event.type == SDL_KEYDOWN && wait_event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_quit = true;
                game_state = GameState::QUIT;
                return false;
            }
        }

        SDL_Delay(1);
    }

    return !is_quit;
}

void close()
{
    static bool is_closed = false;
    if (is_closed)
    {
        return;
    }
    is_closed = true;

    threats_list.clear();
    p_player.ClearBulletList();

    time_game.Free();
    heart_game.Free();
    high_score_game.Free();
    text_menu[0].Free();
    text_menu[1].Free();

    g_background.Free();
    gMonster.Free();
    p_player.Free();
    player_power.Free();
    player_heart.Free();
    FreeRuntimeTextures();

    FreeMenuResources();
    FreeWinResources();
    FreeJourneyResources();

    CloseFont(gFont1);
    CloseFont(gFont2);
    CloseFont(gFont3);
    CloseFont(gFont4);
    CloseFont(font_time);
    CloseFont(font_heart);

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

    FreeChunk(gEarn_Heart);
    FreeChunk(gMainMusic);
    FreeChunk(gFire_ball);
    FreeChunk(gPlayer_Die);
    FreeChunk(gGame_Start);
    FreeChunk(gThreats_Die);
    FreeChunk(gCongrat);

    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    Mix_Quit();
    IMG_Quit();
    if (TTF_WasInit())
    {
        TTF_Quit();
    }
    SDL_Quit();
}

void FreeSurface(SDL_Surface *&surface)
{
    if (surface != NULL)
    {
        SDL_FreeSurface(surface);
        surface = NULL;
    }
}

void DestroyTexture(SDL_Texture *&texture)
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
}

void CloseFont(TTF_Font *&font)
{
    if (font != NULL)
    {
        TTF_CloseFont(font);
        font = NULL;
    }
}

void FreeChunk(Mix_Chunk *&chunk)
{
    if (chunk != NULL)
    {
        Mix_FreeChunk(chunk);
        chunk = NULL;
    }
}

void FreeMenuResources()
{
    DestroyTexture(menu);
    FreeSurface(g_img_menu);
}

void FreeWinResources()
{
    DestroyTexture(WinGame);
    FreeSurface(gWin_game);
}

void FreeJourneyResources()
{
    DestroyTexture(journey_Texture_1);
    DestroyTexture(journey_Texture_2);
    DestroyTexture(journey_Texture_3);
    DestroyTexture(journey_Texture_4);
    DestroyTexture(journey_Texture_5);

    FreeSurface(journey_Surface_1);
    FreeSurface(journey_Surface_2);
    FreeSurface(journey_Surface_3);
    FreeSurface(journey_Surface_4);
    FreeSurface(journey_Surface_5);
}

void LoadRuntimeTextures()
{
    gPlayerLeftTexture.LoadImg("res/pic/img/player_left1.png", g_screen);
    gPlayerRightTexture.LoadImg("res/pic/img/player_right1.png", g_screen);
    gBulletTexture.LoadImg("res/pic/img/fire.png", g_screen);
    gThreat1Texture.LoadImg("res/pic/threats/threat_1.png", g_screen);
    gThreat2LeftTexture.LoadImg("res/pic/threats/threat_2_left.png", g_screen);
    gThreat2RightTexture.LoadImg("res/pic/threats/threat_2_right.png", g_screen);
    gThreat3LeftTexture.LoadImg("res/pic/threats/threat_3_left.png", g_screen);
    gThreat3RightTexture.LoadImg("res/pic/threats/threat_3_right.png", g_screen);
    gThreat4Texture.LoadImg("res/pic/threats/threat_4.png", g_screen);
}

void FreeRuntimeTextures()
{
    gPlayerLeftTexture.Free();
    gPlayerRightTexture.Free();
    gBulletTexture.Free();
    gThreat1Texture.Free();
    gThreat2LeftTexture.Free();
    gThreat2RightTexture.Free();
    gThreat3LeftTexture.Free();
    gThreat3RightTexture.Free();
    gThreat4Texture.Free();
}

void ConfigureDynamicThreat(ThreatsObject *p_threat)
{
    p_threat->SetDynamicTextureRefs(gThreat2LeftTexture.GetObject(), gThreat2LeftTexture.GetRect().w, gThreat2LeftTexture.GetRect().h,
                                    gThreat2RightTexture.GetObject(), gThreat2RightTexture.GetRect().w, gThreat2RightTexture.GetRect().h,
                                    gThreat3LeftTexture.GetObject(), gThreat3LeftTexture.GetRect().w, gThreat3LeftTexture.GetRect().h,
                                    gThreat3RightTexture.GetObject(), gThreat3RightTexture.GetRect().w, gThreat3RightTexture.GetRect().h);
}

bool IsThreatActive(const ThreatsObject *p_threat, const Map &map_data)
{
    if (p_threat == NULL)
    {
        return false;
    }

    const int active_left = map_data.start_x_ - THREAT_ACTIVE_MARGIN;
    const int active_right = map_data.start_x_ + SCREEN_WIDTH + THREAT_ACTIVE_MARGIN;
    const int threat_left = static_cast<int>(p_threat->get_x_pos());
    const int threat_right = threat_left + p_threat->get_width_frame();

    return threat_right >= active_left && threat_left <= active_right;
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
            std::cout << "Cannot open folder!";
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
    bool ret = g_background.LoadImg("res/pic/img/background.jpg", g_screen);
    if (ret == false)
        return false;

    return true;
}

void renderText(const std::string &text, int x, int y, TTF_Font *font)
{
    SDL_Color textColor = {255, 255, 255}; // White color
    Profiler::CountTextRender();
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (textSurface == nullptr)
    {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    Profiler::CountTextureCreate();
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

void Call_Menu()
{
    game_state = GameState::MENU;
    int xm = 0;
    int ym = 0;
    bool selected[2] = {false, false};
    menu = CreateProfiledTextureFromSurface(g_screen, g_img_menu); //    Load background_menu
    menuRect = {0, 0, g_img_menu->w, g_img_menu->h};           //    set menu_position

    text_menu[0].SetText("EXIT");
    text_menu[0].LoadFromRenderText(gFont3, g_screen);

    text_menu[1].SetText("START");
    text_menu[1].LoadFromRenderText(gFont3, g_screen);

    start_button = {SCREEN_WIDTH - 350, 420, 350, 200};
    quit_button = {50, 420, 220, 200};

    while (start_Game == false)
    {
        SDL_RenderCopy(g_screen, menu, NULL, &menuRect);

        text_menu[1].RenderText(g_screen, SCREEN_WIDTH - 350, 420);
        text_menu[0].RenderText(g_screen, 50, 420);

        SDL_RenderPresent(g_screen);
        while (SDL_PollEvent(&eve))
        {
            if (eve.type == SDL_MOUSEMOTION)
            {
                SDL_GetMouseState(&xm, &ym);
                for (int i = 0; i < 2; i++)
                {
                    if (i == 1) // START_BUTTON
                    {
                        if (SDLCommonFunc::CheckFocusMouse(xm, ym, start_button))
                        {
                            if (selected[i] == false)
                            {
                                selected[i] = true;
                                text_menu[i].SetColor(TextObject::RED_TEXT);
                                text_menu[i].LoadFromRenderText(gFont3, g_screen);
                                text_menu[i].RenderText(g_screen, SCREEN_WIDTH - 350, 420);
                                SDL_RenderPresent(g_screen);
                            }
                        }

                        else
                        {
                            if (selected[i] == true)
                            {
                                selected[i] = false;
                                text_menu[i].SetColor(TextObject::WHITE_TEXT);
                                text_menu[i].LoadFromRenderText(gFont3, g_screen);
                                text_menu[i].RenderText(g_screen, SCREEN_WIDTH - 350, 420);
                                SDL_RenderPresent(g_screen);
                            }
                        }
                    }

                    if (i == 0) // QUIT_BUTTON
                    {
                        if (SDLCommonFunc::CheckFocusMouse(xm, ym, quit_button))
                        {
                            if (selected[i] == false)
                            {
                                selected[i] = true;
                                text_menu[i].SetColor(TextObject::RED_TEXT);
                                text_menu[i].LoadFromRenderText(gFont3, g_screen);
                                text_menu[i].RenderText(g_screen, 50, 420);
                                SDL_RenderPresent(g_screen);
                            }
                        }

                        else
                        {
                            if (selected[i] == true)
                            {
                                selected[i] = false;
                                text_menu[i].SetColor(TextObject::WHITE_TEXT);
                                text_menu[i].LoadFromRenderText(gFont3, g_screen);
                                text_menu[i].RenderText(g_screen, 50, 420);
                                SDL_RenderPresent(g_screen);
                            }
                        }
                    }
                }
            }
            if (eve.type == SDL_MOUSEBUTTONDOWN && eve.button.button == SDL_BUTTON_LEFT)
            {
                if (selected[1] == true)
                {
                    start_Game = true; // Ready to Play Game
                    Mix_PlayChannel(-1, gGame_Start, 0);
                    if (WaitWithEventPump(4000))
                    {
                        Mix_PlayChannel(-1, gMainMusic, -1);
                    }
                    FreeMenuResources();
                    break;
                }
                else if (selected[0] == true)
                {
                    is_quit = true;
                    game_state = GameState::QUIT;
                    start_Game = true;
                    FreeMenuResources();
                    break;
                }
            }
            if (eve.type == SDL_QUIT)
            {
                is_quit = true;
                game_state = GameState::QUIT;
                start_Game = true;
                FreeMenuResources();
                break;
            }
        }
    }
}

void Win_Game()
{
    game_state = GameState::WIN;
    bool replay_game = false;
    while (replay_game == false)
    {
        SDL_RenderCopy(g_screen, WinGame, NULL, &WinGameRect);
        renderText("T-Kun finds Isha after: ", SCREEN_WIDTH / 2 - 676, 180, gFont4);
        renderText(str_val, SCREEN_WIDTH / 2 + 290, 180, gFont4);
        renderText("DAYS ", SCREEN_WIDTH / 2 + 455, 180, gFont4);

        heart_game.SetText("SCORE: " + heart_str);
        heart_game.LoadFromRenderText(gFont3, g_screen);
        heart_game.RenderText(g_screen, SCREEN_WIDTH / 2 - 252, 30);

        SDL_RenderPresent(g_screen);
        while (SDL_PollEvent(&eve_win))
        {
            if (eve_win.type == SDL_KEYDOWN && eve_win.key.keysym.sym == SDLK_SPACE)
            {
                Mix_PlayChannel(-1, gGame_Start, 0);
                if (WaitWithEventPump(4000))
                {
                    win_and_restart = true;
                }
                replay_game = true;
            }
            if (eve_win.type == SDL_QUIT)
            {
                replay_game = true;
                is_quit = true;
                game_state = GameState::QUIT;
            }
        }
        SDL_Delay(1);
    }
    replay_game = false;
}

void Restart(Map &map_data, int &num_die, int &heart_count, MainObject &p_player, PlayerPower &player_power)
{
    game_map.LoadMap_Return("res/pic/map/map01.txt");
    game_map.ResetMap(map_data);

    if (winner == true)
    {
        p_player.SetXPos(200);
    }

    else if (winner == false)
    {
        if (map_start < JOURNEY_EACH_MAP * 1 + 280)
        {
            p_player.SetXPos(JOURNEY_EACH_MAP * 0 + 200);
        }
        else if (map_start >= JOURNEY_EACH_MAP * 1 + 280 && map_start < JOURNEY_EACH_MAP * 2 + 280)
        {
            p_player.SetXPos(JOURNEY_EACH_MAP * 1 + 500);
        }
        else if (map_start >= JOURNEY_EACH_MAP * 2 + 280)
        {
            p_player.SetXPos(JOURNEY_EACH_MAP * 2 + 500);
        }
    }
    p_player.HeartCount(0);
    player_power.Init(g_screen);

    num_die = 0;
    heart_count = 0;

    p_player.SetRect(0, 0);
    p_player.set_comeback_time(3);
}

void Create_texture()
{
    WinGame = CreateProfiledTextureFromSurface(g_screen, gWin_game); //    Load background Win_Game
    WinGameRect = {0, 0, gWin_game->w, gWin_game->h};

    journey_Texture_1 = CreateProfiledTextureFromSurface(g_screen, journey_Surface_1);
    journey_Rect_1 = {0, 0, journey_Surface_1->w, journey_Surface_1->h};

    journey_Texture_2 = CreateProfiledTextureFromSurface(g_screen, journey_Surface_2);
    journey_Rect_2 = {0, 0, journey_Surface_2->w, journey_Surface_2->h};

    journey_Texture_3 = CreateProfiledTextureFromSurface(g_screen, journey_Surface_3);
    journey_Rect_3 = {0, 0, journey_Surface_3->w, journey_Surface_3->h};

    journey_Texture_4 = CreateProfiledTextureFromSurface(g_screen, journey_Surface_4);
    journey_Rect_4 = {0, 0, journey_Surface_4->w, journey_Surface_4->h};

    journey_Texture_5 = CreateProfiledTextureFromSurface(g_screen, journey_Surface_5);
    journey_Rect_5 = {0, 0, journey_Surface_5->w, journey_Surface_5->h};
}

void render_journey_img()
{
    if (map_data.start_x_ == JOURNEY_EACH_MAP * 0 + 280 ||
        map_data.start_x_ == JOURNEY_EACH_MAP * 1 + 280 ||
        map_data.start_x_ == JOURNEY_EACH_MAP * 2 + 280 ||
        map_data.start_x_ == JOURNEY_EACH_MAP * 3 + 280 ||
        map_data.start_x_ == JOURNEY_EACH_MAP * 4 + 280)
    {
        game_state = GameState::JOURNEY;
        bool jour_img = false;
        while (jour_img == false)
        {
            if (map_data.start_x_ == JOURNEY_EACH_MAP * 0 + 280)
            {
                SDL_RenderCopy(g_screen, journey_Texture_1, NULL, &journey_Rect_1);
            }
            else if (map_data.start_x_ == JOURNEY_EACH_MAP * 1 + 280)
            {
                if (change_threats == true)
                {
                    change_threats = false;
                }
                SDL_RenderCopy(g_screen, journey_Texture_2, NULL, &journey_Rect_2);
            }
            else if (map_data.start_x_ == JOURNEY_EACH_MAP * 2 + 280)
            {
                if (change_threats == false)
                {
                    change_threats = true;
                }
                SDL_RenderCopy(g_screen, journey_Texture_3, NULL, &journey_Rect_3);
            }
            else if (map_data.start_x_ == JOURNEY_EACH_MAP * 3 + 280)
            {
                SDL_RenderCopy(g_screen, journey_Texture_4, NULL, &journey_Rect_4);
            }
            else if (map_data.start_x_ == JOURNEY_EACH_MAP * 4 + 280)
            {
                SDL_RenderCopy(g_screen, journey_Texture_5, NULL, &journey_Rect_5);
            }
            SDL_RenderPresent(g_screen);
            while (SDL_PollEvent(&eve_win))
            {
                if (eve_win.type == SDL_KEYDOWN && eve_win.key.keysym.sym == SDLK_SPACE)
                {
                    jour_img = true;
                }
                if (eve_win.type == SDL_KEYDOWN && eve_win.key.keysym.sym == SDLK_ESCAPE)
                {
                    jour_img = true;
                    is_quit = true;
                    game_state = GameState::QUIT;
                }
            }
            SDL_Delay(1);
        }
        jour_img = false;
        if (!is_quit)
        {
            game_state = GameState::PLAYING;
        }
    }
}

ThreatList MakeThreats()
{
    ThreatList list_threats;

    //             -  THREAT 1 -
    for (int i = 0; i < NUM_THREATS_LIST; i++)
    {
        std::unique_ptr<ThreatsObject> p_threat(new ThreatsObject());
        if (p_threat != NULL)
        {
            p_threat->UseCachedTexture(gThreat1Texture.GetObject(), gThreat1Texture.GetRect().w, gThreat1Texture.GetRect().h); //  Orc_Fly
            p_threat->set_clips();
            p_threat->set_x_pos(JOURNEY_EACH_MAP * 0 + 2000 + i * (780 + 100 * ((rand() % 3) + 3)));
            p_threat->set_y_pos(200 + 10 * (rand() % 5));
            p_threat->set_type_move(ThreatsObject::THREATS_FLY_STATIC);
            list_threats.push_back(std::move(p_threat));
        }
    }

    //              -  THREAT 2 -
    for (int i = 0; i < NUM_THREATS_LIST; i++)
    {
        std::unique_ptr<ThreatsObject> p_threat(new ThreatsObject());

        if (p_threat != NULL)
        {
            p_threat->UseCachedTexture(gThreat2LeftTexture.GetObject(), gThreat2LeftTexture.GetRect().w, gThreat2LeftTexture.GetRect().h); //  WHITE Dinasaur
            ConfigureDynamicThreat(p_threat.get());
            p_threat->set_clips();
            p_threat->set_type_move(ThreatsObject::MOVE_INSPACE_THREAT);
            p_threat->set_x_pos(JOURNEY_EACH_MAP * 1 + 500 + i * (780 + 100 * ((rand() % 3) + 3)));
            p_threat->set_y_pos(200);
            int pos1 = p_threat->get_x_pos() - 100;
            int pos2 = p_threat->get_x_pos() + 100;
            p_threat->SetAnimationPos(pos1, pos2);
            p_threat->set_input_left(1);
            list_threats.push_back(std::move(p_threat));
        }
    }

    //              -  THREAT 3 -
    for (int i = 0; i < NUM_THREATS_LIST; i++)
    {
        std::unique_ptr<ThreatsObject> p_threat(new ThreatsObject());

        if (p_threat != NULL)
        {
            p_threat->UseCachedTexture(gThreat3LeftTexture.GetObject(), gThreat3LeftTexture.GetRect().w, gThreat3LeftTexture.GetRect().h);
            ConfigureDynamicThreat(p_threat.get());
            p_threat->set_clips();
            p_threat->set_type_move(ThreatsObject::MOVE_INSPACE_THREAT);
            p_threat->set_x_pos(JOURNEY_EACH_MAP * 2 + 500 + i * (780 + 100 * ((rand() % 3) + 3)));
            p_threat->set_y_pos(200);
            int pos1 = p_threat->get_x_pos() - 100;
            int pos2 = p_threat->get_x_pos() + 100;
            p_threat->SetAnimationPos(pos1, pos2);
            p_threat->set_input_left(1);
            list_threats.push_back(std::move(p_threat));
        }
    }

    //              -  THREAT 4 -
    for (int i = 0; i < NUM_THREATS_LIST; i++)
    {
        std::unique_ptr<ThreatsObject> p_threat(new ThreatsObject());
        if (p_threat != NULL)
        {
            p_threat->UseCachedTexture(gThreat4Texture.GetObject(), gThreat4Texture.GetRect().w, gThreat4Texture.GetRect().h); //  Pterosaurs
            p_threat->set_clips();
            p_threat->set_x_pos(JOURNEY_EACH_MAP * 3 + 500 + i * (780 + 100 * ((rand() % 3) + 3)));
            p_threat->set_y_pos(200 + 10 * (rand() % 5));
            p_threat->set_type_move(ThreatsObject::THREATS_FLY_STATIC);

            list_threats.push_back(std::move(p_threat));
        }
    }

    return list_threats;
}

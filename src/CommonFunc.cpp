#include "CommonFunc.h"
#include "Profiler.h"

#include <iostream>

bool winner = false;
int minus = 0;
bool change_threats = false;
int map_start = 0;
Mix_Chunk *gEarn_Heart = NULL;
Mix_Chunk *gFire_ball = NULL;
Mix_Chunk *gPlayer_Die = NULL;
SDL_Window *g_window = NULL;
SDL_Renderer *g_screen = NULL;
SDL_Event g_event;

SDL_Rect SDLCommonFunc::MakePlayerBulletHitbox(const SDL_Rect &object)
{
  // Preserve the collision footprint used by the original game for both the
  // player and bullets. Keeping it explicit prevents sprite-sheet dimensions
  // from silently changing gameplay difficulty.
  return {object.x, object.y, 115, 95};
}

SDL_Rect SDLCommonFunc::MakeThreatHitbox(const SDL_Rect &object)
{
  return {object.x, object.y, 150, 100};
}

bool SDLCommonFunc::CheckCollision(const SDL_Rect &object1, const SDL_Rect &object2)
{
  Profiler::CountCollisionCheck();

  if (object1.w <= 0 || object1.h <= 0 || object2.w <= 0 || object2.h <= 0)
  {
    return false;
  }

  const long long left_a = object1.x;
  const long long right_a = left_a + object1.w;
  const long long top_a = object1.y;
  const long long bottom_a = top_a + object1.h;
  const long long left_b = object2.x;
  const long long right_b = left_b + object2.w;
  const long long top_b = object2.y;
  const long long bottom_b = top_b + object2.h;

  // Strict inequalities retain the original rule that touching edges do not
  // count as a hit, while correctly handling containment and crossing edges.
  return left_a < right_b && right_a > left_b &&
         top_a < bottom_b && bottom_a > top_b;
}

bool SDLCommonFunc::CheckFocusMouse(const int &x, const int &y,const SDL_Rect& pos)
{
  if (x > pos.x && x < pos.x + pos.w && y > pos.y && y < pos.y + pos.h)
  {
    return true;
  }
  return false;
}

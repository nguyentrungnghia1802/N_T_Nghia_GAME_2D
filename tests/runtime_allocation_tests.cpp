#define SDL_MAIN_HANDLED
#include "../src/MainObject.h"

#include <cstdlib>
#include <iostream>

namespace
{
void Expect(bool condition, const char *name)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << name << '\n';
        std::exit(1);
    }
}

void Fire(MainObject &player)
{
    SDL_Event event = {};
    event.type = SDL_MOUSEBUTTONDOWN;
    event.button.button = SDL_BUTTON_LEFT;
    player.HandelInputAction(event, NULL);
}
}

int main()
{
    MainObject player;

    Fire(player);
    Expect(player.get_bullet_list().size() == 1, "first bullet created");
    BulletObject *first_allocation = player.get_bullet_list().front().get();

    player.RemoveBullet(0);
    Expect(player.get_bullet_list().empty(), "removed bullet leaves active list");

    Fire(player);
    Expect(player.get_bullet_list().front().get() == first_allocation,
           "removed bullet object reused");

    player.ResetBulletList();
    Expect(player.get_bullet_list().empty(), "restart clears active bullets");
    Fire(player);
    Expect(player.get_bullet_list().front().get() == first_allocation,
           "restart bullet object reused");

    player.ClearBulletList();
    Expect(player.get_bullet_list().empty(), "shutdown clears bullets");

    std::cout << "runtime allocation tests passed\n";
    return 0;
}

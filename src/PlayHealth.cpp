
#include <iostream>
#include "PlayHealth.h"

PlayerPower::PlayerPower()
{
    number_ = 0;
}

PlayerPower::~PlayerPower()
{
}

void PlayerPower::AddPos(const int &xp)
{
    pos_list_.push_back(xp);
}

void PlayerPower::Init(SDL_Texture *texture, int width, int height)
{
    UseTexture(texture, width, height);
    number_ = 3;
    if (pos_list_.size() > 0)
    {
        pos_list_.clear();
    }

    AddPos(20);
    AddPos(60);
    AddPos(100);
}

void PlayerPower::Show(SDL_Renderer *screen)
{
    for (size_t i = 0; i < pos_list_.size(); i++)
    {
        rect_.x = pos_list_.at(i);
        rect_.y = 0;
        Render(screen);
    }
}

void PlayerPower::Decrease()
{
    if (!pos_list_.empty())
    {
        number_--;
        pos_list_.pop_back();
    }
}

void PlayerPower::InitCrease()
{
    int last_pos = pos_list_.empty() ? -20 : pos_list_.back();
    number_++;
    last_pos += 40;
    pos_list_.push_back(last_pos);
}

PlayerMoney::PlayerMoney()
{
    x_pos_ = 0;
    y_pos_ = 0;
}

PlayerMoney::~PlayerMoney()
{
}

void PlayerMoney::Init(SDL_Texture *texture, int width, int height)
{
    UseTexture(texture, width, height);
}

void PlayerMoney::Show(SDL_Renderer *screen)
{
    rect_.x = x_pos_;
    rect_.y = y_pos_;
    Render(screen);
}

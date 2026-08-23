#include <iostream>
#include "BulletObject.h"

BulletObject::BulletObject()
{
    x_val_ = 0;
    y_val_ = 0;
    is_move_ = false;
    bullet_dir_ = DIR_RIGHT;
    movement_remainder_ = 0.0f;
}

BulletObject::~BulletObject()
{
}

void BulletObject::HandleMove(const int &x_border, float frame_scale)
{
    const float movement = x_val_ * frame_scale + movement_remainder_;
    const int whole_pixels = static_cast<int>(movement);
    movement_remainder_ = movement - whole_pixels;

    if (bullet_dir_ == DIR_RIGHT)
    {
        rect_.x += whole_pixels;
        if (rect_.x > x_border)
        {
            is_move_ = false;
        }
    }
    else if (bullet_dir_ == DIR_LEFT)
    {
        rect_.x -= whole_pixels;
        if (rect_.x < 0)
        {
            is_move_ = false;
        }
    }
}

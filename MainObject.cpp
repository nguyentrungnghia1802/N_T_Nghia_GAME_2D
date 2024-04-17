
#include <iostream>
#include "MainObject.h"

MainObject::MainObject()
{
    frame_ = 0;
    x_pos_ = 500;
    y_pos_ = 0;
    x_val_ = 0;
    y_val_ = 0;
    width_frame_ = 0;
    height_frame_ = 0;
    status_ = -1;
    input_type_.left_ = 0;
    input_type_.right_ = 0;
    input_type_.jump_ = 0;
    input_type_.down_ = 0;
    input_type_.up_ = 0;
    on_ground_ = false;
    map_x_ = 0;
    map_y_ = 0;
    come_back_time_ = 0;
    heart_count = 0;
    is_minus_live = false;
}

MainObject::~MainObject()
{
}

bool MainObject::LoadImg(std::string path, SDL_Renderer *screen)
{
    bool ret = BaseObject::LoadImg(path, screen);

    if (ret == true)
    {
        width_frame_ = (rect_.w / MAX_FRAME_PLAYER); // so luong frame la 6
        height_frame_ = rect_.h;
    }

    return ret;
};

SDL_Rect MainObject::GetRectFrame()
{
    SDL_Rect rect;
    rect.x = rect_.x;
    rect.y = rect_.y;
    rect.w = rect_.w / width_frame_;
    rect.h = height_frame_;
    return rect;
}

void MainObject::set_clips()
{
    if (width_frame_ > 0 & height_frame_ > 0)
    {
        frame_clip_[0].x = 0;
        frame_clip_[0].y = 0;
        frame_clip_[0].w = width_frame_;
        frame_clip_[0].h = height_frame_;

        frame_clip_[1].x = width_frame_;
        frame_clip_[1].y = 0;
        frame_clip_[1].w = width_frame_;
        frame_clip_[1].h = height_frame_;

        frame_clip_[2].x = 2 * width_frame_;
        frame_clip_[2].y = 0;
        frame_clip_[2].w = width_frame_;
        frame_clip_[2].h = height_frame_;

        frame_clip_[3].x = 3 * width_frame_;
        frame_clip_[3].y = 0;
        frame_clip_[3].w = width_frame_;
        frame_clip_[3].h = height_frame_;

        frame_clip_[4].x = 4 * width_frame_;
        frame_clip_[4].y = 0;
        frame_clip_[4].w = width_frame_;
        frame_clip_[4].h = height_frame_;

        frame_clip_[5].x = 5 * width_frame_;
        frame_clip_[5].y = 0;
        frame_clip_[5].w = width_frame_;
        frame_clip_[5].h = height_frame_;
    }
}

int delay_frame = 0;

void MainObject::Show(SDL_Renderer *des)
{
    if (status_ == WALK_LEFT)
    {
        LoadImg("img/player_left1.png", des);
    }
    else
    {
        LoadImg("img/player_right1.png", des);
    }

    if (input_type_.left_ == 1 ||
        input_type_.right_ == 1)
    {
        delay_frame++;
        if (delay_frame == 2)
        {
            frame_++;
            delay_frame = 0;
        }
    }
    else
    {
        frame_ = 0;
    }
    if (frame_ >= MAX_FRAME_PLAYER)
    {
        frame_ = 0;
    }

    if (come_back_time_ == 0)
    {
        rect_.x = x_pos_ - map_x_;
        rect_.y = y_pos_ - map_y_;

        SDL_Rect *current_clip = &frame_clip_[frame_];
        SDL_Rect renderQuad = {rect_.x, rect_.y, width_frame_, height_frame_};

        SDL_RenderCopy(des, p_object_, current_clip, &renderQuad);
    }
}

void MainObject::HandelInputAction(SDL_Event events, SDL_Renderer *screen, Mix_Chunk *gFire_ball)
{
    if (events.type == SDL_KEYDOWN)
    {
        switch (events.key.keysym.sym)
        {
        case SDLK_d:
        {
            status_ = WALK_RIGHT;
            input_type_.right_ = 1;
            input_type_.left_ = 0;
        }
        break;
        case SDLK_a:
        {
            status_ = WALK_LEFT;
            input_type_.left_ = 1;
        }
        break;
        }
    }
    else if (events.type == SDL_KEYUP)
    {
        switch (events.key.keysym.sym)
        {
        case SDLK_d:
        {
            input_type_.right_ = 0;
        }
        break;
        case SDLK_a:
        {
            input_type_.left_ = 0;
        }
        break;
        }
    }

    if (events.type == SDL_KEYDOWN)
    {
        if (events.key.keysym.sym == SDLK_w)
        {
            input_type_.jump_ = 1;
        }
    }
    else if (events.type == SDL_MOUSEBUTTONDOWN)
    {
        if (events.button.button == SDL_BUTTON_LEFT)
        {
            Mix_PlayChannel(-1, gFire_ball, 0);
            BulletObject *p_bullet = new BulletObject();
            p_bullet->LoadImg("img/fire.png", screen);

            if (status_ == WALK_LEFT)
            {
                p_bullet->set_bullet_dir(BulletObject::DIR_LEFT);
                p_bullet->SetRect(this->rect_.x - 20, rect_.y);
            }
            else if (status_ == WALK_RIGHT)
            {
                p_bullet->set_bullet_dir(BulletObject::DIR_RIGHT);
                p_bullet->SetRect(this->rect_.x + width_frame_ - 20, rect_.y);
            }

            rect_.x = x_pos_ - map_x_;
            rect_.y = y_pos_ - map_y_;

            p_bullet->set_x_val(20);
            p_bullet->set_is_move(true);

            p_bullet_list_.push_back(p_bullet);
        }
    }
}

void MainObject::HanleBullet(SDL_Renderer *des)
{
    for (int i = 0; i < p_bullet_list_.size(); i++)
    {
        BulletObject *p_bullet = p_bullet_list_.at(i);
        if (p_bullet != NULL)
        {
            if (p_bullet->get_is_move() == true)
            {
                p_bullet->HandleMove(SCREEN_WIDTH, SCREEN_HEIGHT);
                p_bullet->Render(des);
            }
            else
            {
                p_bullet_list_.erase(p_bullet_list_.begin() + i);
                if (p_bullet != NULL)
                {
                    delete p_bullet;
                    p_bullet = NULL;
                }
            }
        }
    }
}

void MainObject::RemoveBullet(const int &idx)
{
    int size = p_bullet_list_.size();
    if (size > 0 && idx < size)
    {
        BulletObject *p_bullet = p_bullet_list_.at(idx);
        p_bullet_list_.erase(p_bullet_list_.begin() + idx);

        if (p_bullet)
        {
            delete p_bullet;
            p_bullet = NULL;
        }
    }
}

void MainObject::DoPlayer(Map &map_data, Mix_Chunk *gEarn_Heart)
{

    if (come_back_time_ == 0)
    {
        x_val_ = 0;
        y_val_ += GRAVITY_SPEED;

        if (y_val_ >= MAX_FALL_SPEED)
        {
            y_val_ = MAX_FALL_SPEED;
        }

        if (input_type_.left_ == 1)
        {
            x_val_ -= PLAYER_SPEED;
        }
        else if (input_type_.right_ == 1)
        {
            x_val_ += PLAYER_SPEED;
        }

        if (input_type_.jump_ == 1)
        {
            if (on_ground_ == true)
            {
                y_val_ = -PLAYER_JUMP_VAL;
            }
            on_ground_ = false;           // DELETE this current to JUMP unlimited
            input_type_.jump_ = 0;
        }

        CheckToMap(map_data, gEarn_Heart);
    }

    if (come_back_time_ > 0)
    {
        come_back_time_--;
        if (come_back_time_ <= 0)
        {
            if (check_x == true)
            {
                x_pos_ += 500;
                check_x = false;
            }
            else
            {
                if (x_pos_ > 1500)
                {
                    x_pos_ += 100;
                }
                else
                {
                    x_pos_ += 100;
                }
            }
            y_pos_ = 0;
            x_val_ = 0;
            y_val_ = 0;
            come_back_time_ = 0;
        }
    }
}

void MainObject::CheckToMap(Map &map_data, Mix_Chunk *gEarn_Heart)
{
    int x1 = 0;
    int x2 = 0;

    int y1 = 0;
    int y2 = 0;

    // Check horizontal
    int height_min = height_frame_ < TILE_SIZE ? height_frame_ : TILE_SIZE;

    x1 = (x_pos_ + x_val_) / TILE_SIZE;
    x2 = (x_pos_ + x_val_ + width_frame_ - 1) / TILE_SIZE;

    y1 = (y_pos_) / TILE_SIZE;
    y2 = (y_pos_ + height_min - 1) / TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (x_val_ > 0) // Main Player move to right
        {
            int val1 = map_data.tile[y1][x2];
            int val2 = map_data.tile[y2][x2];

            if (val1 == POINT_ITEM_1 || val2 == POINT_ITEM_1)
            {
                map_data.tile[y1][x2] = 0;
                map_data.tile[y2][x2] = 0;
                Mix_PlayChannel(-1, gEarn_Heart, 0);
                IncreaseMoney();
            }
            else
            {
                if (val1 != BLANK_TILE || val2 != BLANK_TILE)
                {
                    x_pos_ = x2 * TILE_SIZE;
                    x_pos_ -= width_frame_ + 1;
                    x_val_ = 0;
                }
            }
            if(map_data.start_x_>=MAX_MAP_X*TILE_SIZE-1500)
            {
                map_data.start_x_=MAX_MAP_X*TILE_SIZE-1500;
            }
            if (x_pos_>=MAX_MAP_X*TILE_SIZE-16*TILE_SIZE)       ///////////////////////////// NEED TO CHANGE IF MAP CHANGE //////////////////////////
            {
                winner = true;
                x_pos_ = 0;
            }
        }
        else if (x_val_ < 0)
        {
            int val1 = map_data.tile[y1][x2];
            int val2 = map_data.tile[y2][x2];

            if (val1 == POINT_ITEM_1 || val2 == POINT_ITEM_1)
            {
                map_data.tile[y1][x2] = 0;
                map_data.tile[y2][x2] = 0;
                Mix_PlayChannel(-1, gEarn_Heart, 0); 
                IncreaseMoney();
            }
            else
            {
                if (val1 != BLANK_TILE || val2 != BLANK_TILE)
                {
                    x_pos_ = (x1 + 1) * TILE_SIZE;
                    x_val_ = 0;
                }
            }
        }
    }

    // Check vertical

    int width_min = width_frame_ < TILE_SIZE ? width_frame_ : TILE_SIZE;
    x1 = (x_pos_) / TILE_SIZE;
    x2 = (x_pos_ + width_min) / TILE_SIZE;

    y1 = (y_pos_ + y_val_) / TILE_SIZE;
    y2 = (y_pos_ + y_val_ + height_frame_ - 1) / TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (y_val_ > 0)
        {
            int val1 = map_data.tile[y2][x1];
            int val2 = map_data.tile[y2][x2];

            if (val1 == POINT_ITEM_1 || val2 == POINT_ITEM_1)
            {
                map_data.tile[y2][x1] = 0;
                map_data.tile[y2][x2] = 0;
                IncreaseMoney();
            }
            else
            {
                if (val1 != BLANK_TILE || val2 != BLANK_TILE)
                {
                    y_pos_ = y2 * TILE_SIZE;
                    y_pos_ -= (height_frame_ + 1);
                    y_val_ = 0;
                    on_ground_ = true;
                }
            }
        }
        else if (y_val_ < 0)
        {
            int val1 = map_data.tile[y1][x1];
            int val2 = map_data.tile[y1][x2];

            if (val1 == POINT_ITEM_1 || val2 == POINT_ITEM_1)
            {
                map_data.tile[y1][x1] = 0;
                map_data.tile[y1][x2] = 0;
                IncreaseMoney();
            }
            else
            {
                if (map_data.tile[y1][x1] != BLANK_TILE || map_data.tile[y1][x2] != BLANK_TILE)
                {
                    y_pos_ = (y1 + 1) * TILE_SIZE;
                    y_val_ = 0;
                }
            }
        }
    }

    x_pos_ += x_val_;
    y_pos_ += y_val_;

    if (x_pos_ + width_frame_ > map_data.max_x_)
    {
        x_pos_ = map_data.max_x_ - width_frame_ - 1;
    }
    if (y_pos_ > map_data.max_y_ || x_pos_ < map_data.start_x_)
    {
        if (x_pos_ < map_data.start_x_)
        {
            check_x = true;
        }
        come_back_time_ = 3;
        is_minus_live = true;
    }
}

void MainObject::IncreaseMoney()
{
    heart_count++;
}

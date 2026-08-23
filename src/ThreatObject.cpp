
#include <iostream>
#include "ThreatObject.h"
#include "BaseObject.h"
#include "Profiler.h"

ThreatsObject::ThreatsObject()
{
    width_frame_ = 0;
    height_frame_ = 0;
    x_val_ = 0.0;
    y_val_ = 0.0;
    x_pos_ = 0.0;
    y_pos_ = 0.0;
    on_ground_ = 0;
    map_x_ = 0;
    map_y_ = 0;
    come_back_time_ = 0;
    frame_ = 0;

    animation_a_ = 0;
    animation_b_ = 0;
    input_type_.left_ = 0;
    input_type_.right_ = 0;
    input_type_.up_ = 0;
    input_type_.down_ = 0;
    input_type_.jump_ = 0;
    type_move_ = STATIC_THREAT;
    threat2_left_ = {NULL, 0, 0};
    threat2_right_ = {NULL, 0, 0};
    threat3_left_ = {NULL, 0, 0};
    threat3_right_ = {NULL, 0, 0};
}

ThreatsObject::~ThreatsObject()
{
}

bool ThreatsObject::LoadImg(std::string path, SDL_Renderer *screen)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret)
    {
        width_frame_ = rect_.w / THREAT_FRAME_NUM;
        height_frame_ = rect_.h;
    }
    return ret;
}

void ThreatsObject::UseCachedTexture(SDL_Texture *texture, int width, int height)
{
    if (texture == NULL)
    {
        return;
    }
    if (p_object_ == texture && rect_.w == width && rect_.h == height)
    {
        return;
    }

    UseTexture(texture, width, height);
    width_frame_ = rect_.w / THREAT_FRAME_NUM;
    height_frame_ = rect_.h;
    set_clips();
}

void ThreatsObject::SetDynamicTextureRefs(SDL_Texture *threat2_left, int threat2_left_width, int threat2_left_height,
                                          SDL_Texture *threat2_right, int threat2_right_width, int threat2_right_height,
                                          SDL_Texture *threat3_left, int threat3_left_width, int threat3_left_height,
                                          SDL_Texture *threat3_right, int threat3_right_width, int threat3_right_height)
{
    threat2_left_ = {threat2_left, threat2_left_width, threat2_left_height};
    threat2_right_ = {threat2_right, threat2_right_width, threat2_right_height};
    threat3_left_ = {threat3_left, threat3_left_width, threat3_left_height};
    threat3_right_ = {threat3_right, threat3_right_width, threat3_right_height};
}

void ThreatsObject::set_clips()
{
    if (width_frame_ > 0 && height_frame_ > 0)
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
    }
}

void ThreatsObject::Show(SDL_Renderer *des, const SDL_Rect *viewport)
{
    if (come_back_time_ == 0 && des != NULL && p_object_ != NULL)
    {
        rect_.x = x_pos_ - map_x_;
        rect_.y = y_pos_ - map_y_;
        frame_++;

        if (frame_ >= THREAT_FRAME_NUM)
        {
            frame_ = 0;
        }

        SDL_Rect *currentClip = &frame_clip_[frame_];
        SDL_Rect rendQuad = {rect_.x, rect_.y, width_frame_, height_frame_};
        if (viewport != NULL && !IsVisibleInViewport(*viewport))
        {
            return;
        }

        Profiler::CountEntityRender();
        SDL_RenderCopy(des, p_object_, currentClip, &rendQuad);
    }
}

void ThreatsObject::DoPlayer(Map &gMap)
{
    Profiler::CountEntityUpdate();

    if (come_back_time_ == 0)
    {
        x_val_ = 0;
        y_val_ += THREAT_GRAVITY_SPEED;

        if (y_val_ >= MAX_FALL_SPEED)
        {
            y_val_ = MAX_FALL_SPEED;
        }

        if (input_type_.left_ == 1)
        {
            x_val_ -= THREAT_SPEED;
        }
        else if (input_type_.right_ == 1)
        {
            x_val_ += THREAT_SPEED;
        }

        CheckToMap(gMap);
    }
    else if (come_back_time_ > 0)
    {
        come_back_time_--;
        if (come_back_time_ == 0)
        {
            InitThreats();
        }
    }
}

void ThreatsObject::InitThreats()
{
    x_val_ = 0;
    y_val_ = 0;
    if (x_pos_ > 1500)
    {
        x_pos_ -= 500;
        animation_a_ -= 500;
        animation_b_ -= 500;
    }
    else
    {
        x_pos_ = rand()%700 + 555;
    }
    y_pos_ = 0;
    come_back_time_ = 0;
    input_type_.left_ = 1;
}

void ThreatsObject::CheckToMap(Map &map_data)
{

    if (type_move_ != THREATS_FLY_STATIC)
    {

        int x1 = 0;
        int x2 = 0;

        int y1 = 0;
        int y2 = 0;

        int height_min = height_frame_ < TILE_SIZE ? height_frame_ : TILE_SIZE;

        x1 = (x_pos_ + x_val_) / TILE_SIZE;
        x2 = (x_pos_ + x_val_ + width_frame_ - 1) / TILE_SIZE;

        y1 = (y_pos_) / TILE_SIZE;
        y2 = (y_pos_ + height_min - 1) / TILE_SIZE;

        if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
        {
            if (x_val_ > 0)
            {
                int val1 = map_data.tile[y1][x2];
                int val2 = map_data.tile[y2][x2];

                if ((val1 != BLANK_TILE && val1 != POINT_ITEM_1) || (val2 != BLANK_TILE && val2 != POINT_ITEM_1))
                {
                    x_pos_ = x2 * TILE_SIZE;
                    x_pos_ -= width_frame_ + 1;
                    input_type_.left_ = 1;
                    input_type_.right_ = 0;
                }
            }
            else if (x_val_ < 0)
            {
                int val1 = map_data.tile[y1][x2];
                int val2 = map_data.tile[y2][x2];

                if ((val1 != BLANK_TILE && val1 != POINT_ITEM_1) || (val2 != BLANK_TILE && val2 != POINT_ITEM_1))
                {
                    x_pos_ = (x1 + 1) * TILE_SIZE;
                    input_type_.right_ = 1;
                    input_type_.left_ = 0;
                }
            }
        }

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

                if ((val1 != BLANK_TILE && val1 != POINT_ITEM_1) || (val2 != BLANK_TILE && val2 != POINT_ITEM_1))
                {
                    y_pos_ = y2 * TILE_SIZE;
                    y_pos_ -= (height_frame_ + 1);
                    y_val_ = 0;
                    on_ground_ = true;
                }
            }
            else if (y_val_ < 0)
            {
                int val1 = map_data.tile[y1][x1];
                int val2 = map_data.tile[y1][x2];

                if ((val1 != BLANK_TILE && val1 != POINT_ITEM_1) || (val2 != BLANK_TILE && val2 != POINT_ITEM_1))
                {
                    y_pos_ = (y1 + 1) * TILE_SIZE;
                    y_val_ = 0;
                }
            }
        }

        x_pos_ += x_val_;
        y_pos_ += y_val_;

        if (x_pos_ < 0)
        {
            x_pos_ = 0;
        }
        else if (x_pos_ + width_frame_ > map_data.max_x_)
        {
            x_pos_ = map_data.max_x_ - width_frame_ - 1;
        }

        if (y_pos_ > map_data.max_y_)
        {
            Free();
        }
        if(y_pos_ < 200)
        {
            y_pos_ = 560;
        }
    }
    else
    {
        int x1 = 0;
        int x2 = 0;

        int y1 = 0;
        int y2 = 0;

        int height_min = height_frame_ < TILE_SIZE ? height_frame_ : TILE_SIZE;

        x1 = (x_pos_ + x_val_) / TILE_SIZE;
        x2 = (x_pos_ + x_val_ + width_frame_ - 1) / TILE_SIZE;

        y1 = (y_pos_) / TILE_SIZE;
        y2 = (y_pos_ + height_min - 1) / TILE_SIZE;

        if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
        {
            int val1 = map_data.tile[y1][x1];
            int val2 = map_data.tile[y1][x2];

            if ((val1 != BLANK_TILE && val1 != POINT_ITEM_1) || (val2 != BLANK_TILE && val2 != POINT_ITEM_1))
            {
                y_pos_ = (y1 + 1) * TILE_SIZE;
            }
        }
    }
}

void ThreatsObject::ImpMoveType()
{
    if (type_move_ == STATIC_THREAT)
    {
        ;
    }
    else if (type_move_ == MOVE_INSPACE_THREAT)
    {
        if (on_ground_ == true)
        {
            if (x_pos_ > animation_b_)
            {
                input_type_.left_ = 1;
                input_type_.right_ = 0;
                if (change_threats == false)
                {
                    UseCachedTexture(threat2_left_.texture, threat2_left_.width, threat2_left_.height);
                }
                else if (change_threats==true)
                {
                    UseCachedTexture(threat3_left_.texture, threat3_left_.width, threat3_left_.height);
                }
            }
            else if (x_pos_ < animation_a_)
            {
                input_type_.left_ = 0;
                input_type_.right_ = 1;
                if (change_threats == false)
                {
                    UseCachedTexture(threat2_right_.texture, threat2_right_.width, threat2_right_.height);
                }
                else if (change_threats == true)
                {
                    UseCachedTexture(threat3_right_.texture, threat3_right_.width, threat3_right_.height);
                }
            }
        }
        else
        {
            if (input_type_.left_ == 1)
            {
                if (change_threats == false)
                {
                    UseCachedTexture(threat2_left_.texture, threat2_left_.width, threat2_left_.height);
                }
                else if (change_threats == true)
                {
                    UseCachedTexture(threat3_left_.texture, threat3_left_.width, threat3_left_.height);
                }
            }
        }
    }

    else if (type_move_ == THREATS_FLY_STATIC)
    {
        ;
    }
}

SDL_Rect ThreatsObject::GetRectFrame()
{
    SDL_Rect rect = {rect_.x, rect_.y, width_frame_, height_frame_};
    return rect;
}

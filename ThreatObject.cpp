
#include <iostream>
#include "ThreatObject.h"
#include "BaseObject.h"


ThreatsObject::ThreatsObject()
{
    width_frame_=0;
    height_frame_=0;
    x_val_=0.0;
    y_val_=0.0;
    x_pos_=0.0;
    y_pos_=0.0;
    on_ground_=0;
    come_back_time_=0;
    frame_=0;

    animation_a_=0;
    animation_b_=0;
    input_type_.left_=0;
    type_move_=STATIC_THREAT;



}

ThreatsObject::~ThreatsObject()
{

}

bool ThreatsObject::LoadImg(std::string path,SDL_Renderer* screen)
{
    bool ret=BaseObject::LoadImg(path,screen);
    if(ret)
    {
        width_frame_ = rect_.w/THREAT_FRAME_NUM;
        height_frame_=rect_.h;
    }
    return ret;
}

void ThreatsObject::set_clips()
{
    if(width_frame_>0&&height_frame_>0)
    {
        frame_clip_[0].x=0;
        frame_clip_[0].y=0;
        frame_clip_[0].w=width_frame_;
        frame_clip_[0].h=height_frame_;

        frame_clip_[1].x=width_frame_;
        frame_clip_[1].y=0;
        frame_clip_[1].w=width_frame_;
        frame_clip_[1].h=height_frame_;


        frame_clip_[2].x=2*width_frame_;
        frame_clip_[2].y=0;
        frame_clip_[2].w=width_frame_;
        frame_clip_[2].h=height_frame_;

        frame_clip_[3].x=3*width_frame_;
        frame_clip_[3].y=0;
        frame_clip_[3].w=width_frame_;
        frame_clip_[3].h=height_frame_;

        frame_clip_[4].x=4*width_frame_;
        frame_clip_[4].y=0;
        frame_clip_[4].w=width_frame_;
        frame_clip_[4].h=height_frame_;

    }
}

void ThreatsObject::Show(SDL_Renderer* des)
{
    if(come_back_time_==0)
    {
        rect_.x=x_pos_ - map_x_;
        rect_.y=y_pos_ - map_y_;
        frame_++;
        if(frame_>=THREAT_FRAME_NUM)
        {
            frame_ = 0;
        }

        SDL_Rect* currentClip = &frame_clip_ [frame_];
        SDL_Rect rendQuad = {rect_.x,rect_.y,width_frame_,height_frame_};
        SDL_RenderCopy(des, p_object_, currentClip, &rendQuad);
        

    }
}

void ThreatsObject::DoPlayer(Map& gMap)
{
    if(come_back_time_==0)
    {
        x_val_=0;
        y_val_+= THREAT_GRAVITY_SPEED;


        if(y_val_ >= MAX_FALL_SPEED)
        {
            y_val_ = MAX_FALL_SPEED;

        } 

        if(input_type_.left_==1)
        {
            x_val_ -= THREAT_SPEED;
        }
        else if(input_type_.right_==1)
        {
            x_val_ += THREAT_SPEED;
        }


        CheckToMap(gMap);
    }
    else if(come_back_time_ > 0)
    {
        come_back_time_ --;
        if(come_back_time_==0)
        {
            InitThreats();
        }
    }
}

void ThreatsObject::InitThreats()
{
    x_val_=0;
    y_val_=0;
    if(x_pos_ > 1500)
        {
            x_pos_ -= 500;
            animation_a_ -=500;
            animation_b_ -=500;

        }
    else
        {
            x_pos_=0;
        }
        y_pos_=0;
        come_back_time_ = 0;
        input_type_.left_=1;
}

void ThreatsObject::CheckToMap(Map& map_data)
{

    if(type_move_!=THREATS_FLY_STATIC)
    {
        

    int x1=0;
    int x2=0;

    int y1=0;
    int y2=0;

    int height_min=height_frame_ <TILE_SIZE ? height_frame_ : TILE_SIZE;

    x1=(x_pos_+x_val_)/TILE_SIZE;
    x2=(x_pos_+x_val_+width_frame_-1)/TILE_SIZE;

    y1=(y_pos_)/TILE_SIZE;
    y2=(y_pos_ + height_min-1)/TILE_SIZE;


    if(x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if( x_val_>0) 
        {
            int val1 = map_data.tile[y1][x2];
            int val2 = map_data.tile[y2][x2];

            if((val1 !=BLANK_TILE&&val1 != POINT_ITEM_1) || (val2 !=BLANK_TILE&&val2 != POINT_ITEM_1))
            {
                  x_pos_=x2*TILE_SIZE;
                  x_pos_-=width_frame_+1;
                  input_type_.left_=0;
                  input_type_.right_=1;
            }
        }
        else if (x_val_<0)
        {
            int val1 = map_data.tile[y1][x2];
            int val2 = map_data.tile[y2][x2];

           
            if((val1!=BLANK_TILE&&val1!=POINT_ITEM_1)||( val2!=BLANK_TILE&&val2!=POINT_ITEM_1))
            {
                x_pos_=(x1+1)*TILE_SIZE;
                input_type_.right_=0;
                input_type_.left_=1;
            }
        }
    }


    int width_min = width_frame_ < TILE_SIZE ? width_frame_ : TILE_SIZE;
    x1 = (x_pos_)/TILE_SIZE;
    x2 = (x_pos_+width_min)/TILE_SIZE;

    y1=(y_pos_+y_val_)/TILE_SIZE;
    y2=(y_pos_+y_val_+ height_frame_ - 1)/TILE_SIZE;

    if( x1>=0 && x2 < MAX_MAP_X && y1>=0 && y2 < MAX_MAP_Y)
    {
        if (y_val_ > 0)
        {
            int val1=map_data.tile[y2][x1];
            int val2=map_data.tile[y2][x2];

            if((val1!=BLANK_TILE&&val1!=POINT_ITEM_1)||(val2!=BLANK_TILE&&val2!=POINT_ITEM_1))
            {
                y_pos_=y2*TILE_SIZE;
                y_pos_-=(height_frame_+1);
                y_val_=0;
                on_ground_=true;
            }
        }
        else if (y_val_<0)
        {
            int val1=map_data.tile[y1][x1];
            int val2=map_data.tile[y1][x2];

                if ((val1 !=BLANK_TILE&&val1!=POINT_ITEM_1) || (val2!=BLANK_TILE!=POINT_ITEM_1))
            {
                y_pos_=(y1+1)*TILE_SIZE;
                y_val_=0;
            }
        }
    }

    x_pos_+=x_val_;
    y_pos_+=y_val_;

    if (x_pos_<0)
    {
        x_pos_=0;
    }
    else if(x_pos_ + width_frame_ > map_data.max_x_)
    {
        x_pos_=map_data.max_x_ - width_frame_ - 1;
    }
    
    if (y_pos_>map_data.max_y_)
    {
        Free();
    }

    }

}

void ThreatsObject::ImpMoveType(SDL_Renderer* screen)
{
    if(type_move_==STATIC_THREAT)
    {
        ;
    }
    else if(type_move_==MOVE_INSPACE_THREAT)
    {
        if(on_ground_==true)
        {
            if(x_pos_ > animation_b_)
            {
                input_type_.left_=1;
                input_type_.right_=0;
                LoadImg("img/threat_2_left.png",screen);

            }
            else if(x_pos_<animation_a_)
            {
                input_type_.left_=0;
                input_type_.right_=1;
                LoadImg("img/threat_2_right.png",screen);
            }
        }
        else
        {
            if(input_type_.left_==1)
            {
                LoadImg("img/threat_2_left.png",screen);
            }
        }
    }


    else if(type_move_==THREATS_FLY_STATIC)
    {
        ;
    }



}





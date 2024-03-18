#ifndef THREAT_OBJECT_H_
#define THREAT_OBJECT_h_

#include "CommonFunc.h"
#include "BaseObject.h"

#define THREAT_FRAME_NUM 8
#define THREAT_GRAVITY_SPEED 1
#define MAX_FALL_SPEED 10


class ThreatsObject: public BaseObject
{
    public: 
        ThreatsObject();
        ~ThreatsObject();

        void set_x_val(const float& xVal) {x_val_ =xVal;}
        void set_y_val(const float& yVal) {y_val_ = yVal;}

        void set_x_pos(const float& xp) {x_pos_ = xp;}
        void set_x_pos(const float& yp) {y_pos_=yp;}

        float get_x_pos() const {return x_pos_;}
        float get_y_pos() const {return y_pos_;}
        void SetMapXY(const int& mp_x, const int& mp_y) {map_x_=mp_x; map_y_=mp_y;}

        void set_clips();
        bool LoadImg(std::string path, SDL_Renderer* screen);
        void Show(SDL_Renderer* des);
        int get_width_frame() const {return width_frame_;}
        int get_height_frame() const{return height_frame_;}
        void DoPlayer (Map& gMap);
        void CheckToMap(Map& gMap);




    private:
        bool on_ground_;
        float x_pos_;
        float y_pos_;
        float x_val_;
        float y_val_;
        int map_x_;
        int map_y_;


        SDL_Rect frame_clip_[THREAT_FRAME_NUM];

        int width_frame_;
        int height_frame_;
        int frame_;
        int come_back_time_;
        
     
};

#endif
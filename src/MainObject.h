
#ifndef MAIN_OBJECT_H_
#define MAIN_OBJECT_H_

#include <memory>
#include <vector>
#include "CommonFunc.h"
#include "BaseObject.h"
#include "BulletObject.h"


#define GRAVITY_SPEED 0.8
#define MAX_FALL_SPEED 10
#define PLAYER_SPEED 8
#define PLAYER_JUMP_VAL 20
#define MAX_FRAME_PLAYER 6
#define POS_MAP_DATA_START_X_TO_WIN 79760            //79760


class MainObject : public BaseObject
{
public:
        using BulletList = std::vector<std::unique_ptr<BulletObject>>;

        MainObject();
        ~MainObject();
        enum WalkType
        {
                WALK_RIGHT = 0,
                WALK_LEFT = 1,
        };

        bool LoadImg(std::string path, SDL_Renderer* screen);
        void SetTextureRefs(SDL_Texture* left_texture, int left_width, int left_height,
                            SDL_Texture* right_texture, int right_width, int right_height);
        void SetBulletTextureRef(SDL_Texture* texture, int width, int height);
        void Show(SDL_Renderer* des, float frame_scale);
        void HandelInputAction(const SDL_Event& events, Mix_Chunk* gF);
        void set_clips();

        void DoPlayer(Map& map_data, Mix_Chunk *gEarn_Heart, float frame_scale);
        void CheckToMap(Map& map_data, Mix_Chunk *gEarn_Heart, float vertical_step);
        void SetMapXY(const int map_x, const int map_y) {map_x_ = map_x; map_y_ = map_y;}
        SDL_Rect GetRectFrame();

        const BulletList& get_bullet_list() const {return p_bullet_list_;}
        void HanleBullet(SDL_Renderer* rec, float frame_scale);
        void RemoveBullet(size_t idx);
        void ResetBulletList();
        void ClearBulletList();
        void PrepareRespawn(float comeback_time);
        void IncreaseMoney();
        bool GetIsMinusLive() {return is_minus_live;}
        void RetsetMinusLive() {is_minus_live = false;}
        int GetMoneyCount() const {return heart_count;}
        float GetXPos() const {return x_pos_;}
        void SetXPos(float x) {x_pos_ = x;}
        void HeartCount(int x) {heart_count=x;}
private:
        int heart_count;

        BulletList p_bullet_list_;
        BulletList bullet_pool_;
        float x_val_;
        float y_val_;
        
        float x_pos_;
        float y_pos_;

        int width_frame_;
        int height_frame_;

        SDL_Rect frame_clip_[MAX_FRAME_PLAYER];
        Input input_type_;
        int frame_;
        int status_;      
        bool on_ground_;

        int map_x_;
        int map_y_;

        float come_back_time_;
        float animation_ticks_;
        bool is_minus_live;

        bool check_x=false;

        SDL_Texture* left_texture_;
        SDL_Texture* right_texture_;
        SDL_Texture* bullet_texture_;
        int left_texture_width_;
        int left_texture_height_;
        int right_texture_width_;
        int right_texture_height_;
        int bullet_texture_width_;
        int bullet_texture_height_;
        int loaded_status_;
        void ApplyTextureForStatus(const int& status);
        void ResetMovementInput();
        std::unique_ptr<BulletObject> AcquireBullet();
        void RecycleBullet(size_t idx);
};


#endif

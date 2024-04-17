#ifndef BULLET_OBJECT_H_
#define BULLET_OBJECT_H_

#include "BaseObject.h"
#include "CommonFunc.h"

class BulletObject : public BaseObject
{
public:
       BulletObject();
       ~BulletObject();
       enum BulletDir
       {
              DIR_RIGHT = 20,
              DIR_LEFT = 21,
       };

       void set_x_val(const int &xVal) { x_val_ = xVal; }
       void set_y_val(const int &yVal) { y_val_ = yVal; }
       int get_x_val() const { return x_val_; }
       int get_y_val() const { return y_val_; }

       void set_is_move(const bool &isMove) { is_move_ = isMove; }
       bool get_is_move() const { return is_move_; }

       void set_bullet_dir(const unsigned int &bulletDir) { bullet_dir_ = bulletDir; }
       void HandleMove(const int &x_boder, const int &y_boder);

private:
       int x_val_;
       int y_val_;
       bool is_move_;
       unsigned int bullet_dir_;
};

#endif
#ifndef TURN_SERVICE_H_
#define TURN_SERVICE_H_

#include "Motor.h"
#include "ColorSensor.h"
#include "ColorLogic.h"
#include "Sapporo.h"


class TurnService{
public:
    TurnService(
        ev3api::Motor& leftWheel,
        ev3api::Motor& rightWheel,
        ev3api::Motor& armMotor,
        ev3api::ColorSensor& colorSensor
    );

    /** 
    * 90度旋回する
    * IN:旋回する方向(LEFT:1, RIGHT:2) 旋回する角度
    */
    void nice_turn(int direction, int turn_angle);

    /** 
    * 180度旋回する
    * IN: コースのフラグ(LEFT:1, RIGHT:2)
    */
    void fire_turn(int lr_flag);
    
    /** 
    * ジグザグ直線する
    * IN: 走行体の位置(LEFT:1, RIGHT:2)
    */
    void zig_zag(int position);

private:
    ev3api::Motor& mLeftWheel;
    ev3api::Motor& mRightWheel;
    ev3api::Motor& mArmMotor;
    ColorLogic mColorLogic;
    void init();
    void turn(int direction, int turn_angle);
    void add_turn(int direction);
    void recover_turn(int direction);
    void block_turn();

    double now_direction = 0;

};
#endif /* TURN_SERVICE_H_ */
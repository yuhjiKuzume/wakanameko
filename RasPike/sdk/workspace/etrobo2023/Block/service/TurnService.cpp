#include "TurnService.h"
#include "Clock.h"
#include "localizerTask.h"
#include "localizer.h"
#include "Sapporo.h"

using ev3api::Clock;
Clock mclock;

TurnService::TurnService(
    ev3api::Motor& leftWheel,
    ev3api::Motor& rightWheel,
    ev3api::Motor& armMotor,
    ev3api::ColorSensor& colorSensor)
     : mLeftWheel(leftWheel), mRightWheel(rightWheel), mArmMotor(armMotor), mColorLogic(colorSensor){
}

// 90度ターン
void TurnService::nice_turn(int direction, int turn_angle) {
    printf("nice_turn_start!\n");
    if(LR_FLAG == RIGHT && turn_angle == DENGER_ANGLE) {
        turn_angle = RDENGER_ANGLE;
    }
    turn(direction, turn_angle);

    // 90度のときは黒検知まで追加ターン
    if(turn_angle == RIGHT_ANGLE) {
         add_turn(direction);
    }

}

// ジグザグ直線
void TurnService::zig_zag(int position) {

    printf("zig_zag_start!\n");

    int zig_direction = LEFT;
    int zag_direction = RIGHT;

    if(position == RIGHT) {
        zig_direction = RIGHT;
        zag_direction = LEFT;
    }

    int zig = ZIG;
    int zag = ZAG;

    if(LR_FLAG == RIGHT) {
        zig = RZIG;
    }

    if(position == LEFT) {
        turn(zig_direction, zig+5);
    } else {
        turn(zig_direction, zag+3);
    }
    turn(zag_direction, zag);
    turn(zig_direction, zig);
    turn(zag_direction, zag);
}


// 初期化
void TurnService::init() {
    // printf("初期化\n");
    localizer_run();
    localizer_setDistance();

    // 現在の角度を取得する
    now_direction = localizer_getDirection();
    
    mLeftWheel.stop();
    mRightWheel.stop();
    mclock.sleep(50 * 1000);
}


// 決められた角度旋回する
void TurnService::turn(int direction, int turn_angle) {

    // printf("turn_start!!!!\n");

    init();
    long int start = mclock.now();
    int curv_speed = CURV_SPEED_HIGH;

    // 基本的には片方のタイヤだけを動かす
    // 指定された角度に曲がるまで角度検知
    //10msに一回旋回角度を確認する
    if(direction == RIGHT){    
        // 基準値になるまで右旋回
        while(now_direction - localizer_getDirection() < turn_angle){
            // printf("right direction %lf %lf \n", localizer_getDirection(), now_direction);
            mLeftWheel.setPWM(curv_speed);
            mRightWheel.setPWM(0);     
            mclock.sleep(10*1000);
            // ブースト処理
            // 止まった時に徐々にスピードを上げる
            if((mclock.now() - start) > 750*1000 ){
                // printf("booost turn \n");
                curv_speed = curv_speed + 10;
                start = mclock.now();
            }
            mclock.sleep(1*100);
        }
    }else if (direction == LEFT){
        // 基準値になるまで左旋回
        while(localizer_getDirection() - now_direction < turn_angle){
            // printf("left direction %lf %lf \n", localizer_getDirection(), now_direction);
            mLeftWheel.setPWM(0);
            mRightWheel.setPWM(curv_speed);          
            mclock.sleep(10*1000);
            // ブースト処理
            // 止まった時に徐々にスピードを上げる
            if((mclock.now() - start) > 750*1000 ){
                // printf("booost turn \n");
                curv_speed = curv_speed + 10;
                start = mclock.now();
            }
            mclock.sleep(1*100);
        }    
    }

    return;
}


// センサーが黒を検知していない場合
// もう少しだけ曲がる
void TurnService::add_turn(int direction) {
    printf("add_turn_start\n");

    init();

    long int start = mclock.now();
    // curv_speed = 50; //50だとブロックを持った時うまく曲がれなかったため、少しだけゆっくり曲がる
    int curv_speed = CURV_SPEED_LOW;

    while(mColorLogic.checkColor() != 1){
        if(direction == RIGHT){
            mLeftWheel.setPWM(curv_speed);
            mRightWheel.setPWM(0);   
        }else if (direction == LEFT){
            mLeftWheel.setPWM(0);
            mRightWheel.setPWM(curv_speed);  
        }     

        // ブースト処理
        // 止まった時に徐々にスピードを上げる
        if((mclock.now() - start) > 750*1000 ){
            // printf("booost turn \n");
            curv_speed = curv_speed + 10;
            start = mclock.now();
        }
        mclock.sleep(1*100);
    }

    return;
}

// 黒を超えてしまった場合にリカバーする
void TurnService::recover_turn(int direction) {
    printf("recover_turn_start\n");
    init();

    while(mColorLogic.checkColor() != 1){
        if(direction == RIGHT){
            mLeftWheel.setPWM(0);
            mRightWheel.setPWM(CURV_SPEED_LOW);   
        }else if (direction == LEFT){
            mLeftWheel.setPWM(CURV_SPEED_LOW);
            mRightWheel.setPWM(0);  
        }    
        mclock.sleep(1*100);
    }

    return;
}

// ブロックがある状態で右位置から右位置にまがるとき、ブロックが邪魔で理想位置より少し前でまがってしまう。
// ブロックがあるときにないときと同じ開始位置にするため、少し前進する
void TurnService::block_turn() {
    printf("block_turn_start\n");
    init();

    while(localizer_getDistance() - now_direction < 10){
        mLeftWheel.setPWM(40);
        mRightWheel.setPWM(40);
        mclock.sleep(5*1000);
    }

    mLeftWheel.setPWM(-40);
    mRightWheel.setPWM(-40);
    mclock.sleep(10*1000);

    return;
}

// 180度旋回する
void TurnService::fire_turn(int r_flag) {
    printf("fire_turn_start\n");
    init();

    // 左コースの時は左回転
    if(r_flag == LEFT){
        mLeftWheel.setPWM(-50);
        mRightWheel.setPWM(50);
        mclock.sleep(1000*1000);
    // 右コースの時は右回転
    }else if(r_flag == RIGHT){
        mLeftWheel.setPWM(50);
        mRightWheel.setPWM(-50);
        mclock.sleep(1000*1000);
    }

    // ブースト処理
    // 止まった時に徐々にスピードを上げる
    // printf("boost turn\n");
    while(mColorLogic.checkColor() != 1){
        if(r_flag == LEFT){   
            mLeftWheel.setPWM(-50);
            mRightWheel.setPWM(50);   
        }else if(r_flag == RIGHT){
            mLeftWheel.setPWM(50);
            mRightWheel.setPWM(-50);  
        }     
    }
    mLeftWheel.setPWM(-40);
    mRightWheel.setPWM(-40);
    mclock.sleep(10*1000);    

    return;
}

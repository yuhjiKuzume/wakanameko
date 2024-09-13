#include "LineTracerService.h"
#include "Clock.h"
#include "localizer.h"
#include "Sapporo.h"
using ev3api::Clock;
Clock mClock;
//キャリブレーション用のパラメータ
int mBLACK_BRIGHTNESS = BLACK_BRIGHTNESS;
int mWHITE_BRIGHTNESS = WHITE_BRIGHTNESS;
double CARCOFF = 1; //補正係数

int flag = 0;
double a1 = 0;
double a2 = 0;
double a3 = 0;
double a4 = 0;
double a5 = 0;

LineTracerService::LineTracerService(
    ev3api::Motor& leftWheel,
    ev3api::Motor& rightWheel,
    ev3api::ColorSensor& colorSensor)
     : mLeftWheel(leftWheel), mRightWheel(rightWheel),mColorSensor(colorSensor){
}

/**
 * 走行に必要なものをリセットする
 */
void LineTracerService::init() {
    // モータをリセットする
    mLeftWheel.reset();
    mRightWheel.reset();
}

/**
 * 周期実行/ライントレースの実行
 */

void LineTracerService::run(int lineposition, int base_speed) {
    /*
    * lineposition=1なら左回転
    * lineposition=2なら右回転
    */ 
    // flag ++;
    if(flag == 0){
        init();
        cariblation();
        flag++;
    }

    int EDGE = RIGHT_EDGE;
    if(lineposition == 2){
        //右回転ならspeedの値をマイナスにする
        EDGE = LEFT_EDGE;
        // printf("LEFT POSITION\n");
    }
    // else{
        // printf("RIGHT POSITION\n");
    // }

    double steering_amount = steering_amount_calculation();

    int left_motor_power, right_motor_power; /*左右モータ設定パワー*/
    /* 左右モータ駆動パワーの計算(走行エッジを右にする場合はRIGHT_EDGEに書き換えること) */
    left_motor_power  = (int)(base_speed + (steering_amount * EDGE));
    right_motor_power = (int)(base_speed - (steering_amount * EDGE));
    // printf("tmp_log,moter is called  %lu \n" , mClock.now());
    // printf("tmp_log,moterright:%d ,left:%d \n", right_motor_power,left_motor_power);
    mRightWheel.setPWM(right_motor_power);
    mLeftWheel.setPWM(left_motor_power);
}
 void LineTracerService::cariblation(){
    mBLACK_BRIGHTNESS = BLACK_BRIGHTNESS;//黒を基準にする
    mWHITE_BRIGHTNESS = mBLACK_BRIGHTNESS + 160;//白は黒+100
    CARCOFF = (double)mWHITE_BRIGHTNESS / (double)WHITE_BRIGHTNESS; //補正係数を求める
} 

int past[num];

int16_t LineTracerService::steering_amount_calculation(){
    
    uint16_t  target_brightness; /* 目標輝度値 */
    float32_t diff_brightness;   /* 目標輝度との差分値 */
    int16_t   steering_amount;   /* ステアリング操舵量 */
    rgb_raw_t rgb_val;           /* カラーセンサ取得値 */
    float32_t diff_aim;   /* 目標輝度との差分値 */

    /* 目標輝度値の計算 */ 
    //黒の線と青の線では目標値を変更する
    target_brightness = (mWHITE_BRIGHTNESS - mBLACK_BRIGHTNESS) / 2 ;
    /* カラーセンサ値の取得 */
    //mColorSensor.ev3_color_sensor_get_rgb_raw(mColorSensor, &rgb_val);
    mColorSensor.getRawColor(rgb_val);
    /* 目標輝度値とカラーセンサ値の差分を計算 */
    diff_brightness = (float32_t)(target_brightness - (CARCOFF)*rgb_val.g);
    /* ステアリング操舵量を計算 */
    diff_aim = (diff_brightness * STEERING_COEF);

    double daikei_nakami = 0;
    /*過去の値を計算*/
    for(int i = 0; i < num;i++){
        //printf("%d  : %d , ",i,past[i]);
        if(i == 0 || i == num-1){
            daikei_nakami += 3*past[i];
        }else {
            daikei_nakami += 2*past[i];
        } 
    }
    double sum_i = SUM_COEF* daikei_nakami * dt / 2  ;//+ a4 + a5  ;//台形公式
    /*直前の変化を負荷*/
    double der = DER_COEF*(diff_brightness - past[0]);///dt;
    //printf("WhilteBrightness : %d , mWhite %d\n",WHITE_BRIGHTNESS,mWHITE_BRIGHTNESS );
    //printf("coff %f target %d White %f  , diff_aim %f,  sum %f , der %f  \n", CARCOFF, target_brightness , (CARCOFF)*rgb_val.g - mBLACK_BRIGHTNESS,diff_aim, sum_i,der);
    /*初回呼び出しのときはderを実行しない*/
    if(flag < 2 ){
        der = 0;
        flag++;
    }
    for(int i = num - 1; i>=0 ;i--){
        if(i == 0){
            past[0] = diff_brightness;            
        }else{
            past[i] = past[i-1];
           // printf("bb \n");
        }
    }
    steering_amount = diff_aim + sum_i + der;
    //printf(" str : %d ",steering_amount);
    return steering_amount;
}

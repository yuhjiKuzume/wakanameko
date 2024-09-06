#include "app.h"
#include "LineTracer.h"
#include <stdio.h>
#define KP 0.36
#define KI 1.2
#define KD 0.027

float32_t math_limit(float32_t, float32_t, float32_t);

static long diff[2];
static float32_t integral;

/* 関数プロトタイプ宣言 */
static int16_t steering_amount_calculation(void);
static void motor_drive_control(int16_t);

/* ライントレースタスク(100msec周期で関数コールされる) */
void tracer_task(intptr_t unused) {

    int16_t steering_amount; /* ステアリング操舵量の計算 */
    
    /* ステアリング操舵量の計算 */
    steering_amount = steering_amount_calculation();

    /* 走行モータ制御 */
    motor_drive_control(steering_amount);

    /* タスク終了 */
    ext_tsk();
}

/* ステアリング操舵量の計算 */
static int16_t steering_amount_calculation(void){

    uint16_t  target_brightness; /* 目標輝度値 */
    float32_t diff_brightness;   /* 目標輝度との差分値 */
    int16_t   steering_amount;   /* ステアリング操舵量 */
    rgb_raw_t rgb_val;           /* カラーセンサ取得値 */
    
    float32_t p,i,d;

    diff[0] = diff[1];

    /* 目標輝度値の計算 */
    target_brightness = (WHITE_BRIGHTNESS - BLACK_BRIGHTNESS) / 2;

    /* カラーセンサ値の取得 */
    ev3_color_sensor_get_rgb_raw(color_sensor, &rgb_val);

    /* 目標輝度値とカラーセンサ値の差分を計算 */
    diff_brightness = (float32_t)(target_brightness - rgb_val.g);
    diff[1] = diff_brightness;
    integral += (diff[1] + diff[0])/ 2.0 * 100;
    
    p = KP * diff[0];
    i = KI * integral;
    d = KD *(diff[1] - diff[0]) / 100;
    
    steering_amount = math_limit(p+i+d,-100,100.0);

    /* ステアリング操舵量を計算 */
    //steering_amount = (int16_t)(diff_brightness * STEERING_COEF);
   
    // output RGB values
    printf("RGB values print >> R:%d  G:%d  B:%d\n",rgb_val.r,rgb_val.g,rgb_val.b);
    printf("%f\n",math_limit(p+i+d,-100.0,100.0));
    return steering_amount;
}

/* 走行モータ制御 */
static void motor_drive_control(int16_t steering_amount){

    int left_motor_power, right_motor_power; /*左右モータ設定パワー*/

    /* 左右モータ駆動パワーの計算(走行エッジを右にする場合はRIGHT_EDGEに書き換えること) */
    left_motor_power  = (int)(BASE_SPEED + (steering_amount * LEFT_EDGE));
    right_motor_power = (int)(BASE_SPEED - (steering_amount * LEFT_EDGE));

    /* 左右モータ駆動パワーの設定 */
    ev3_motor_set_power(left_motor, left_motor_power);
    ev3_motor_set_power(right_motor, right_motor_power);

    return;
}

float32_t math_limit(float32_t value, float32_t min, float32_t max)
{
    if (value < min) {
        return min;
    }
    else if (value > max) {
        return max;
    }
    return value;
}

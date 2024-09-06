#include "app.h"
#include "LineTracer.h"
#include "PidControl.h"
#include "ColorControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 関数プロトタイプ宣言 */
static void motor_drive_control(int16_t, int16_t, int16_t);

void pid_driver(rgb_raw_t rgb_val, int target_color, int16_t trace_edge, int16_t dynamic_base_speed, int selected_pid_parameter)
{
    int16_t steering_amount;
    steering_amount = steering_amount_pid_calculation(rgb_val, target_color, selected_pid_parameter); // PID制御

    /* 走行モータ制御 */
    motor_drive_control(steering_amount, trace_edge, dynamic_base_speed);
}

/* 走行モータ制御 */
static void motor_drive_control(int16_t steering_amount, int16_t trace_edge, int16_t dynamic_base_speed)
{

    int left_motor_power, right_motor_power; /*左右モータ設定パワー*/

    /* 左右モータ駆動パワーの計算(走行エッジを右にする場合はRIGHT_EDGEに書き換えること) */
    left_motor_power = (int)(dynamic_base_speed + (steering_amount * trace_edge));
    right_motor_power = (int)(dynamic_base_speed - (steering_amount * trace_edge));
    // left_motor_power = (int)(dynamic_base_speed);
    // right_motor_power = (int)(dynamic_base_speed);

    // if (left_motor_power > 100 || right_motor_power > 100)
    // {
    //     printf("motor_powerが100超えたので調節\n");
    //     if (left_motor_power > right_motor_power)
    //     {
    //         right_motor_power = 100 * (right_motor_power / left_motor_power);
    //         left_motor_power = 100;
    //     }
    //     else if (left_motor_power < right_motor_power)
    //     {
    //         left_motor_power = 100 * (left_motor_power / right_motor_power);
    //         right_motor_power = 100;
    //     }
    //     else
    //     {
    //         left_motor_power = 100;
    //         right_motor_power = 100;
    //     }
    // }

    /* 左右モータ駆動パワーの設定 */
    ev3_motor_set_power(left_motor, left_motor_power);
    ev3_motor_set_power(right_motor, right_motor_power);

    return;
}
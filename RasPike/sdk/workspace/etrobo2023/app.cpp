#include "app.h"
#include "Sapporo.h"
#include <stdio.h>

/* メインタスク(起動時にのみ関数コールされる) */
void main_task(intptr_t unused)
{

    /* センサー入力ポートの設定 */
    ev3_sensor_config(touch_sensor, TOUCH_SENSOR);
    ev3_sensor_config(color_sensor, COLOR_SENSOR);
    ev3_sensor_config(sonar_sensor, ULTRASONIC_SENSOR);
    ev3_sensor_config(gyro_sensor, GYRO_SENSOR);

    /* モーター出力ポートの設定 */
    ev3_motor_config(arm_motor, LARGE_MOTOR);
    ev3_motor_config(left_motor, MEDIUM_MOTOR);
    ev3_motor_config(right_motor, MEDIUM_MOTOR);

    printf("Start Line Trace!!\n");

    static bool_t is_start = false; // スタートフラグ
    static int block_course;        // 青と赤コースなら1,黄と緑コースなら2

    /*
        スタートボタン判定
            センターボタン、レフトボタン：1(青と赤コース)
            ライトボタン：2(黄と緑コース)
    */
    while (!is_start)
    {
        if (ev3_button_is_pressed(ENTER_BUTTON))
        {
            is_start = true;
            block_course = 1;
            break;
        }
        else if (ev3_button_is_pressed(LEFT_BUTTON))
        {
            is_start = true;
            block_course = 1;
            break;
        }
        else if (ev3_button_is_pressed(RIGHT_BUTTON))
        {
            is_start = true;
            block_course = 2;
            break;
        }
    }

    /* ライントレースタスクの起動 */
    if (is_start)
    {
        ev3_gyro_sensor_reset(gyro_sensor);

        sta_cyc(LINE_TRACER_TASK_CYC); // 左コース
        // sta_cyc(LINE_TRACER_TASK_CYC_R); // 右コース
        // slp_tsk();
    }

    /* ブロック運びの起動(例) */
    // printf("ブロックdeトレジャースタート\n");
    // //stp_cyc(LINE_TRACER_TASK_CYC);      //左コース
    // stp_cyc(LINE_TRACER_TASK_CYC_R);      // 右コース
    // sta_cyc(CYC_COLOR_CHECK);
    // sta_cyc(LCL_CYC);

    // user_system_create(block_course);
    // act_tsk(CARRY_BLOCK_TASK);
    // slp_tsk();

    /* タスク終了 */
    ext_tsk();
}

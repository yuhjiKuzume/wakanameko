#include "app.h"
#include "LineTracer.h"
#include "PidControl.h"
#include "ColorControl.h"
#include "ReturnLine.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int time;                   // 　制御時間
static int trace_edge = LEFT_EDGE; // RIGHT_EDGE
// static int trace_edge = RIGHT_EDGE;
// static int dynamic_base_speed = 60;
static int dynamic_base_speed = 45;
static int target_color = 170;
static int nowtime = 0;

/* ライントレースタスク(10msec周期で関数コールされる) */
void tracer_task(intptr_t unused)
{

    static int blue_line_count = 0; // 青ライン通過回数

    // ポイント通過フラグ
    static bool_t is_passing_through = false; // LAP通過をしたか
    static bool_t is_entering_double_loop = false;
    static bool_t is_entering_mini_circle = false;

    // モーター停止フラグ
    static bool_t is_motor_stop = false;
    static bool_t motor_impals = false;
    static bool_t end_of_linetrace = false;

    // 時間を計測
    time++;

    // 青→黒のライン変化取得
    if (is_change_line_color_hsv())
    {
        blue_line_count++;
        nowtime = time;
    }

    printf("blue line cont:%d\n", blue_line_count);
    printf("time:%d\n", time / 100);

    // カラー情報を取得
    set_colorsensor_value();
    convert_rgb_to_hsv();
    // ジャイロセンサーから角度を取得
    int16_t angle = ev3_gyro_sensor_get_angle(gyro_sensor);
    printf("angle:%d\n", angle);

    /*
        トレースラインに対する走行体の角度補正
    */
    // static bool_t serch_line_status = true;
    // static bool_t first_time = true;
    // if(serch_line_status){
    //     if(first_time){
    //         ReturnLine_init();
    //         first_time = false;
    //     }
    //     if(ReturnLine_do(R_EDGE)){
    //         serch_line_status = false;
    //         initialize_pid_value();
    //     }
    // }

    /*
        LAPゲート手前に到達
            ・トレースエッジを切替(左→右)
            ・IoT列車操作開始指示(LAPファイル作成)
    */
    // if(angle > 175 && !is_passing_through){
    //     printf("Lap手前に到達\n");

    //     start_video("/home/goriki/work_cs/RasPike/RaspberryPi/LAP");
    //     dynamic_base_speed = 45;
    //     target_color = 270; //210
    //     blue_line_count = 0;    // Lap過ぎたら、blue_line_countを初期化

    //     initialize_pid_value();

    //     is_passing_through = true;
    //     trace_edge = RIGHT_EDGE;
    //     ev3_gyro_sensor_reset(gyro_sensor);
    //     angle = ev3_gyro_sensor_get_angle(gyro_sensor);
    // }

    // /*
    //     ダブルループに進入
    //         ・ライントレースの目標値変更
    // */
    // if(blue_line_count == 1 && angle > 100 && !is_entering_double_loop){
    //     printf("ダブルループ開始\n");
    //     target_color = 180;
    //     is_entering_double_loop = true;
    //     initialize_pid_value();
    // }

    // /*
    //     ダブルループに進入(サークル交点付近)
    //         ・トレースエッジの変更(右→左)
    //         ・ミニフィグ撮影開始指示(START1ファイル作成)
    // */
    // if(is_passing_through && angle > 110 && blue_line_count == 1 && !is_entering_mini_circle){
    //     printf("小サークル\n");
    //     trace_edge = LEFT_EDGE;
    //     initialize_pid_value();
    //     ev3_gyro_sensor_reset(gyro_sensor);
    //     angle = ev3_gyro_sensor_get_angle(gyro_sensor);
    //     is_entering_mini_circle = true;
    //     start_video("/home/goriki/work_cs/RasPike/RaspberryPi/START1");
    // }

    // /*
    //     小サークルに進入
    //         ・走行体をサークル中心に向けて少し動かす
    //         ・トレースエッジを切替(左→右)
    // */
    // if((time - nowtime ) > 80 && nowtime > 0 && blue_line_count == 2){
    //     motor_impals = true;
    //     ev3_motor_set_power(left_motor, 20);
    //     ev3_motor_set_power(right_motor, 60);
    //     if((time - nowtime) > 110){
    //         nowtime = 0;
    //         motor_impals = false;
    //         trace_edge = RIGHT_EDGE;
    //         initialize_pid_value();
    //     }
    // }

    // /*
    //     大サークルのミニフィグ撮影開始
    // */
    // static bool_t take_movie_2 = true;
    // if(blue_line_count == 2 && check_Line_color_hsv(1) && take_movie_2){
    //     take_movie_2 = false;
    //     start_video("/home/goriki/work_cs/RasPike/RaspberryPi/START2");
    // }

    // /*
    //     大サークルに進入
    //         ・走行体をサークル中心に向けて少し動かす
    // */
    // if((time - nowtime ) > 80 && nowtime > 0 && blue_line_count == 3){
    //     motor_impals = true;
    //     ev3_motor_set_power(left_motor, 60);
    //     ev3_motor_set_power(right_motor, 20);
    //     if((time - nowtime) > 100){//90
    //         nowtime = 0;
    //         motor_impals = false;
    //         initialize_pid_value();
    //     }
    // }

    // /*
    //     ダブルループ脱出
    // */
    // static int exit_time = 0;
    // static bool_t is_loop_escape = false;
    // if(check_Line_color_hsv(1) && blue_line_count == 3){
    //     exit_time = time;
    // }

    // // 90
    // static bool_t is_loop_escape_edge_change = false;
    // if((time - exit_time) > 80 && exit_time > 0 && blue_line_count == 3 && !is_loop_escape_edge_change){
    //     trace_edge = LEFT_EDGE;
    //     is_loop_escape = true;
    //     initialize_pid_value();
    //     is_loop_escape_edge_change = true;
    // }

    // /*
    //     赤丸までダッシュ！
    // */
    // static int dash_time = 0;
    // static bool_t dash_start = false;
    // if(!dash_start && is_loop_escape && (time - exit_time) > 790 && exit_time > 0){
    //     dynamic_base_speed = 60;
    //     target_color = 270;
    //     initialize_pid_value();
    //     dash_time = time;
    //     dash_start = true;
    //     exit_time = 0;
    // }

    // if((time-dash_time) > 600 && dash_time > 0 && dash_start){
    //     dynamic_base_speed = 40;
    //     dash_time = 0;
    //     initialize_pid_value();
    // }

    // /*
    //     ブロックdeトレジャーに切替
    // */
    // if(check_Line_color_hsv(2) && blue_line_count >= 4){
    //     printf("赤検知\n");
    //     end_of_linetrace = true;
    //     ev3_motor_set_power(left_motor, 0);
    //     ev3_motor_set_power(right_motor, 0);

    //     wup_tsk(MAIN_TASK);
    // }

    /* ステアリング操舵量の計算 */
    if (!is_motor_stop && !motor_impals && !end_of_linetrace)
    {
        pid_driver(get_colorsensor_value(), target_color, trace_edge, dynamic_base_speed);
    }

    /* タスク終了 */
    ext_tsk();
}

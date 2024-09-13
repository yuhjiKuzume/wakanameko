#include "app.h"
#include "LineTracerR.h"
#include "PidControl.h"
#include "ColorControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int time; // 　制御時間
static int trace_edge = RIGHT_EDGE;
static int dynamic_base_speed = 60;
static int target_color = 170;
// LAP通過後を確認用
// static int trace_edge = LEFT_EDGE;
// // static int dynamic_base_speed = 30;
// static int dynamic_base_speed = 45;
// static int target_color = 180; // 270
static int latest_passed_blue_line_time = 0;

// 超えてから計測開始時刻
static int timeAtLapPoint = 0;

static int16_t angle;

/* ライントレースタスク(10msec周期で関数コールされる) */
void tracer_task_R(intptr_t unused)
{
    static int selected_pid_parameter = 0; // 0なら直線用pid　1なら楕円用pid(ベースカラーは270にする)
    static int blue_line_count = 0;        // 青ライン通過回数

    // ポイント通過フラグ
    static bool_t is_passing_through = false; // LAP通過をしたか

    // 角度が170度超えたかフラグ
    static bool_t angle170_through = false;
    static bool_t is_entering_double_loop = false;
    static bool_t is_entering_mini_circle = false;

    // モーター停止フラグ
    static bool_t is_motor_stop = false;
    static bool_t motor_impals = false;
    static bool_t end_of_linetrace = false;

    printf("【親】is_motor_stop%d\n", is_motor_stop);
    // 時間を計測
    time++;

    // 青→黒のライン変化取得
    // if (is_change_line_color_hsv())
    // {
    //     blue_line_count++;
    //     latest_passed_blue_line_time = time;
    // }
    // printf("blue line cont:%d\n", blue_line_count);
    // printf("time:%d\n", time / 100);

    // カラー情報を取得
    set_colorsensor_value();
    convert_rgb_to_hsv();
    // ジャイロセンサーから角度を取得
    angle = ev3_gyro_sensor_get_angle(gyro_sensor);
    printf("angle:%d\n", angle);

    takePhotoOfTrainAndLandscape(&motor_impals, &is_motor_stop);

    // 画像撮影and画像判定
    // int result = system("python etrobo2023/captureAndJudgeYolo.py");

    //----------------ポインタ理解用----------------------//
    // static int num = 0;
    // printf("=========================================================================\n");
    // printf("num:%d\n", num);
    // testMethod(&num);
    // static bool_t bool = true;
    // printf("bool:%d\n", bool);
    // testMethodBool1(&bool);
    // printf("=========================================================================");
    //---------------------------------------------------//

    // // 一回も170度を超えてない状態で、170度に到達した場合
    // if ((abs(angle) > 170) && !angle170_through)
    // {
    //     timeAtLapPoint = time;
    //     angle170_through = true;
    //     dynamic_base_speed = 45;
    // }

    // LAP付近にきたら、右側トレースに切り替える
    // 条件文
    // コーナーを曲がった後（=170度に到達）、100秒後にエッジを切り替える
    // -----// 去年(2023)の条件文 //-----
    // if ((angle < -177 || angle > 178) && !is_passing_through)
    // if ((abs(angle) > 177) && !is_passing_through)
    // ---------------------------------
    // if ((time - timeAtLapPoint) > 100 && !is_passing_through && angle170_through)
    // {

    //     printf("---------------------------------\n");
    //     printf("Lap手前に到達\n");
    //     // start_video("/home/goriki/work_cs/RasPike/RaspberryPi/LAP");

    //     dynamic_base_speed = 45;

    //     target_color = 270;
    //     blue_line_count = 0; // Lap過ぎたら、blue_line_countを初期化

    //     initialize_pid_value();

    //     is_passing_through = true;
    //     trace_edge = LEFT_EDGE;
    //     ev3_gyro_sensor_reset(gyro_sensor);
    //     angle = ev3_gyro_sensor_get_angle(gyro_sensor);
    // }

    /*
        ダブルループに進入
            ・ライントレースの目標値変更
    */
    // if (blue_line_count == 1 && angle < -100 && !is_entering_double_loop)
    // {
    //     printf("----------------------------------ダブルループ開始----------------------------------\n");
    //     target_color = 180;
    //     is_entering_double_loop = true;
    //     initialize_pid_value();
    // }

    /*
        ダブルループに進入(サークル交点付近)
            ・トレースエッジの変更(右→左)
            ・ミニフィグ撮影開始指示(START1ファイル作成)
    */
    // if (is_passing_through && angle < -110 && blue_line_count == 1 && !is_entering_mini_circle)
    // {
    //     printf("----------------------------------小サークル----------------------------------\n");
    //     trace_edge = RIGHT_EDGE;
    //     initialize_pid_value();
    //     ev3_gyro_sensor_reset(gyro_sensor);
    //     angle = ev3_gyro_sensor_get_angle(gyro_sensor);
    //     is_entering_mini_circle = true;
    //     // start_video("/home/goriki/work_cs/RasPike/RaspberryPi/START1");
    // }

    /*
        小サークルに進入
            ・走行体をサークル中心に向けて少し動かす
            ・(2023)トレースエッジを切替(右→左) 80 110
            ・(2024)トレースエッジそのまま 60 90
            フラグ
    */
    // static bool_t massuguGo_through = false;
    // if ((time - latest_passed_blue_line_time) > 40 && latest_passed_blue_line_time > 0 && blue_line_count == 2 && !massuguGo_through)
    // {
    //     printf("----------------------------------まっすぐGo----------------------------------");
    //     motor_impals = true;
    //     ev3_motor_set_power(left_motor, 65); // 60
    //     ev3_motor_set_power(right_motor, 20);
    //     if ((time - latest_passed_blue_line_time) > 60)
    //     {
    //         latest_passed_blue_line_time = 0; // エッジ切り替えを無効化する為に作成
    //         massuguGo_through = true;

    //         // ev3_motor_set_power(left_motor, 0);
    //         // ev3_motor_set_power(right_motor, 0);
    //         // is_motor_stop = true;

    //         motor_impals = false;
    //         // trace_edge = LEFT_EDGE;
    //         initialize_pid_value();
    //         // dynamic_base_speed = 30; // 追加2024
    //         selected_pid_parameter = 1;
    //         // target_color = 270;
    //     }
    // }

    /*
    まっすぐGo通過後、n秒後にラインを切り替える(青いラインを最後に通過した秒数と現在の秒数の差分で計算)

    変数　楕円でのエッジ切り替えフラグ
    */
    // static bool_t change_edge_in_ellipse = false;
    // if ((time - latest_passed_blue_line_time) > 700 && latest_passed_blue_line_time > 0 && blue_line_count == 2 && !change_edge_in_ellipse)
    // {
    //     printf("----------------------------------エッジ切り替えましたああああああああああああああああああああああああああああああ----------------------------------");
    //     change_edge_in_ellipse = true;
    //     initialize_pid_value();
    //     target_color = 180;
    //     latest_passed_blue_line_time = 0;
    //     // selected_pid_parameter = 0;
    //     trace_edge = LEFT_EDGE;
    // }

    // if (30 < angle < 120 && blue_line_count == 2 && !is_entering_mini_circle)
    // {
    //     dynamic_base_speed = 35;
    // }
    // else
    // {
    //     dynamic_base_speed = 45;
    // }

    /*
        大サークルのミニフィグ撮影開始
    */
    // static bool_t take_movie_2 = true;
    // if (blue_line_count == 2 && check_Line_color_hsv(1) && take_movie_2)
    // {
    //     take_movie_2 = false;
    //     start_video("/home/goriki/work_cs/RasPike/RaspberryPi/START2");
    // }

    /*
        大サークルに進入
            ・走行体をサークル中心に向けて少し動かす
            2023 : 80 100
    */
    // if ((time - latest_passed_blue_line_time) > 0 && latest_passed_blue_line_time > 0 && blue_line_count == 3)
    // {
    //     printf("----------------------------------２２まっすぐGo----------------------------------２２２２２２２２２２２２２２２----------------------------------");
    //     motor_impals = true;
    //     ev3_motor_set_power(left_motor, 50);
    //     ev3_motor_set_power(right_motor, 50);
    //     if ((time - latest_passed_blue_line_time) > 40)
    //     { // 90
    //         // trace_edge =
    //         selected_pid_parameter = 0;
    //         latest_passed_blue_line_time = 0;
    //         motor_impals = false;
    //         initialize_pid_value();
    //     }
    // }

    /*
        ダブルループ脱出
    */
    // static int exit_time = 0;
    // static bool_t is_loop_escape = false;
    // if (check_Line_color_hsv(1) && blue_line_count == 3)
    // {
    //     exit_time = time;
    // }

    // 90
    // static bool_t is_loop_escape_edge_change = false;
    // if ((time - exit_time) > 80 && exit_time > 0 && blue_line_count == 3 && !is_loop_escape_edge_change)
    // {
    //     trace_edge = RIGHT_EDGE;
    //     is_loop_escape = true;
    //     initialize_pid_value();
    //     is_loop_escape_edge_change = true;
    // }

    /*
        赤丸までダッシュ！
    */
    // static int dash_time = 0;
    // static bool_t dash_start = false;
    // if (!dash_start && is_loop_escape && (time - exit_time) > 790 && exit_time > 0)
    // {
    //     dynamic_base_speed = 60;
    //     // target_color = 270;
    //     initialize_pid_value();
    //     dash_time = time;
    //     dash_start = true;
    //     exit_time = 0;
    // }

    // if ((time - dash_time) > 600 && dash_time > 0 && dash_start)
    // {
    //     initialize_pid_value();
    //     dynamic_base_speed = 40;
    //     dash_time = 0;
    // }

    /*
        ブロックdeトレジャーに切替
    */
    // if (check_Line_color_hsv(2) && blue_line_count >= 4)
    // {
    //     printf("赤検知\n");
    //     end_of_linetrace = true;
    //     ev3_motor_set_power(left_motor, 0);
    //     ev3_motor_set_power(right_motor, 0);

    //     wup_tsk(MAIN_TASK);
    // }

    /* ステアリング操舵量の計算 */
    if (!is_motor_stop && !motor_impals && !end_of_linetrace)
    {
        // ここの引数5つ目を、0なら外に強い　1なら内に強い(ベースカラーは270にする)
        pid_driver(get_colorsensor_value(), target_color, trace_edge, dynamic_base_speed, selected_pid_parameter);
    }

    /* タスク終了 */
    ext_tsk();
}

start_video(char *filename)
{
    FILE *file;
    file = fopen(filename, "w");
    fclose(file);
}

//------------------------撮影タスク用メソッド------------------------//
static bool_t doTowardsCenterOfPerfectCircle = true;
static bool_t doBackToStartPointAtPerfectCircle = true;

// 走行体を正円の中央に向ける
int16_t towardsCenterOfPerfectCircle(bool_t *pointer_motor_impals, bool_t *is_motor_stop)
{
    printf("【正円での撮影タスク：行き】開始\n");
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    // 初期アングルから、到着角度を計算
    static int16_t startAngle;
    static int16_t arrivalAngle;
    static bool_t positionValueIsNull = true;
    if (positionValueIsNull)
    {
        startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);
        printf("startAngle : %d\n", startAngle);
        arrivalAngle = startAngle - 90;
        printf("arrivalAngle : %d\n", arrivalAngle);
        if (arrivalAngle < -179)
        {
            arrivalAngle = 181 - (abs(arrivalAngle) - 179);
        }
        positionValueIsNull = false;
    }

    // 正負の間際の場合角度に猶予をつける
    static bool_t isNeedAngleBuffer = false;
    if (-170 > arrivalAngle)
    {
        isNeedAngleBuffer = true;
    }

    // 到着角度になるまで車輪を動かす
    static bool_t doneTask = false;
    printf("【正円での撮影タスク：行き】撮影位置への移動開始\n");
    if ((angle > arrivalAngle) && !doneTask)
    {
        printf("【正円での撮影タスク：行き】移動中\n");
        // -170~-179のものは、正になった時点で動作を止める
        if (isNeedAngleBuffer && 0 < angle)
        {
            doneTask = true;
        }
        ev3_motor_set_power(right_motor, 60);
    }
    else
    {
        doneTask = true;
    }

    if (doneTask)
    {
        // 動作停止
        is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);

        // タスク終了フラグ更新
        doTowardsCenterOfPerfectCircle = false;
        doneTask = false;
        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;
        printf("【正円での撮影タスク：行き】終了\n");
        return startAngle;
    }
}

void backToStartPointAtPerfectCircle(int16_t startAngle, bool_t *pointer_motor_impals, bool_t *is_motor_stop)
{
    printf("【正円での撮影タスク：帰り】開始\n\n");
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    bool_t isNeedAngleBuffer = false;
    if (170 <= startAngle)
    {
        isNeedAngleBuffer = true;
    }
    // 復帰地点まで動かす
    static bool_t doneTask = false;
    printf("【正円での撮影タスク：帰り】撮影位置への移動開始\n");
    if (startAngle > angle && !doneTask)
    {
        printf("【正円での撮影タスク：帰り】移動中\n");
        ev3_motor_set_power(right_motor, -60);
        // 170~179のものは、負になった時点で動作を止める
        if (isNeedAngleBuffer)
        {
            if (0 > angle)
            {
                doneTask = true;
            }
        }
    }
    else
    {
        doneTask = true;
    }

    if (doneTask)
    {
        // 動作停止
        is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);

        // タスク終了フラグ更新
        doBackToStartPointAtPerfectCircle = false;
        doneTask = false;

        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;
        printf("【正円での撮影タスク：帰り】終了\n");
        return;
    }
}

// 写真がパーフェクトショットか判定する
// bool_t analyzaImage(型名 image){
//     bool_t isPerfectShot;
//     // 写真判定
//     return isPerfectShot;
// }

static int mSecond = 0;
bool_t wait10second(bool_t *is_motor_stop)
{
    mSecond++;
    if (mSecond > 1000)
    {
        *is_motor_stop = false;
        printf("10秒待機終了");
        return true;
    }
    else
    {
        printf("待機中・・・");
        printf("*is_motor_stop%d\n", *is_motor_stop);
        *is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);
    }
}

// プラレール＆背景撮影
void takePhotoOfTrainAndLandscape(bool_t *pointer_motor_impals, bool_t *is_motor_stop)
{
    // 動作を停止させる
    ev3_motor_set_power(left_motor, 0);
    ev3_motor_set_power(right_motor, 0);

    // 走行体を撮影方向に向ける
    static int16_t startAngle;
    printf("【外】startAngle : %d\n", startAngle);
    if (doTowardsCenterOfPerfectCircle)
    {
        startAngle = towardsCenterOfPerfectCircle(pointer_motor_impals, is_motor_stop);
    }
    // // 写真を撮って、パーフェクトショットか判定
    bool_t passShotTask = false;
    if (!passShotTask && !doTowardsCenterOfPerfectCircle)
    {
        passShotTask = wait10second(is_motor_stop);
        // passShotTask = analyzeImage(image);
    }

    // 画像送信

    // 元の位置に戻す
    if (doBackToStartPointAtPerfectCircle && !doTowardsCenterOfPerfectCircle && passShotTask)
    {
        backToStartPointAtPerfectCircle(startAngle, pointer_motor_impals, is_motor_stop);
    }
}

//-------------------------------------------------------------------//

//---------------------以下森岡君のテストコード-----------------------//
// 90°カメラ向けるテスト

// 角度初期段階を確保
// static bool_t kakudo_syoki_kakuho = false;
// static int16_t kakudo_syokidannkai;
// if (!kakudo_syoki_kakuho)
// {
//     kakudo_syokidannkai = angle;
//     kakudo_syoki_kakuho = true;
// }

// static int kamera_mukeru_time = 0;
// static bool_t kamera_mukeru1 = false;
// static bool_t kamera_mukeru2 = false;
// static bool_t kamera_mukeru3 = false;
// static bool_t kamera_mukeru4 = false;
// if ((kakudo_syokidannkai + 80) > angle && !kamera_mukeru1)
// {
//     printf("----------------------------------カメラ向ける----------------------------------");
//     motor_impals = true;
//     ev3_motor_set_power(left_motor, 60);
//     ev3_motor_set_power(right_motor, -10);
//     kamera_mukeru_time = time;
// }
// else if (200 > (time - kamera_mukeru_time) && (time - kamera_mukeru_time) > 0 && !kamera_mukeru2)
// {
//     printf("-------------とまるうううう---------------");
//     kamera_mukeru1 = true;
//     ev3_motor_set_power(left_motor, 0);
//     ev3_motor_set_power(right_motor, 0);
// }
// else if (kakudo_syokidannkai < angle && !kamera_mukeru3)
// {
//     printf("-----------もどるううう-----------------");
//     kamera_mukeru2 = true;
//     ev3_motor_set_power(left_motor, -60);
//     ev3_motor_set_power(right_motor, 10);
// }
// else if ((kakudo_syokidannkai > angle || kakudo_syokidannkai == angle) && kamera_mukeru2 && kamera_mukeru4)
// {
//     printf("----------おわるううう--------------");
//     kamera_mukeru3 = true;
//     motor_impals = false;
// }

//----------------ポインタ理解用----------------------//
void testMethod(int *num1)
{
    printf("*num1:%d\n", *num1);
    testMethod2(num1);
    (*num1)++;
}
void testMethod2(int *num2)
{
    printf("num2:%d\n", num2);
    printf("*num2:%d\n", *num2);
    (*num2)++;
}
void testMethodBool1(bool_t *bool1)
{
    printf("*bool1:%d\n", *bool1);
    testMethodBool2(bool1);
}
void testMethodBool2(bool_t *bool2)
{
    printf("*bool2:%d\n", *bool2);
    *bool2 = false;
    printf("【false後】*bool2:%d\n", *bool2);
}
//---------------------------------------------------//

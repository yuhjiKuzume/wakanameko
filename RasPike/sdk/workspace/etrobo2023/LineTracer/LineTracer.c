#include "app.h"
#include "LineTracer.h"
#include "PidControl.h"
#include "ColorControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

static int time; // 　制御時間
static int trace_edge = LEFT_EDGE;
static int dynamic_base_speed = 60;
static int target_color = 170;
static int latest_passed_blue_line_time = 0;

// 超えてから計測開始時刻
static int timeAtLapPoint = 0;

static int16_t angle;

/* ライントレースタスク(10msec周期で関数コールされる) */
void tracer_task(intptr_t unused)
{
    static int selected_pid_parameter = 0; // 0なら直線用pid　1なら楕円用pid(ベースカラーは270にする)
    static int blue_line_count = 0;        // 青ライン通過回数


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
        latest_passed_blue_line_time = time;
            printf("blue line cont:%d\n", blue_line_count);
    printf("time:%d\n", time / 100);
    }

    // カラー情報を取得
    set_colorsensor_value();
    convert_rgb_to_hsv();
    // ジャイロセンサーから角度を取得
    angle = ev3_gyro_sensor_get_angle(gyro_sensor);
    printf("angle:%d\n", angle);

    /*
    ２つめのカーブに到達
    */ 
    bool_t reachSecondCurve = false;
    if ((abs(angle) > 170) && !reachSecondCurve)
    {
        timeAtLapPoint = time;
        reachSecondCurve = true;
        dynamic_base_speed = 45;
    }

    /*
    LAP付近にきたら、右側トレースに切り替える
    */ 
    bool_t passLap = false;
    if ( reachSecondCurve && (time - timeAtLapPoint) > 100 && !passLap)
    {
        printf("------------------------------------Lap手前に到達------------------------------------\n");

        // 確実に青を検知できるよう、機体のスピードを落とし黒線からの距離を離す
        dynamic_base_speed = 45;
        target_color = 270;

        // 誤検知を考慮して、値をリセット
        blue_line_count = 0;
        initialize_pid_value();
        ev3_gyro_sensor_reset(gyro_sensor);
        angle = ev3_gyro_sensor_get_angle(gyro_sensor);

        trace_edge = RIGHT_EDGE;
        passLap = true;
    }

    /*
        ダブルループに進入
            ・ライントレースの目標値変更
    */
    bool_t enteringDoubleLoop = false;
    if (blue_line_count == 1 && angle > 30 && !enteringDoubleLoop)
    {
        printf("----------------------------------ダブルループ開始----------------------------------\n");
        target_color = 180;
        enteringDoubleLoop = true;
        // 誤検知を考慮して、値をリセット
        blue_line_count = 0;
        initialize_pid_value();
    }

    /*
    プラレール・風景攻略
    */
    static int16_t takeVideoAngle = 110; // ここを変更
    static bool_t passPerfectCercle = false;

    if (enteringDoubleLoop && !passPerfectCercle)
    {
        static bool_t loopedOnce = false;
        /*
        正円１周目
        */ 
        if (!loopedOnce)
        {
            printf("1周目");

            static bool_t arriveTakeVideoPosition = false;
            static bool_t doneTakeVideo = false;
            static bool_t passHarf = false;

            // 撮影位置まで移動
            if (!arriveTakeVideoPosition)
            {
                // ビデオ撮影箇所が正円前半の場合
                if (takeVideoAngle >= 0)
                {
                    if (angle > takeVideoAngle)
                    { // 機体ストップ
                        is_motor_stop = true;
                        ev3_motor_set_power(left_motor, 0);
                        ev3_motor_set_power(right_motor, 0);
                        arriveTakeVideoPosition = true;
                    }
                }
                // ビデオ撮影箇所が正円後半の場合
                else
                {
                    if (angle >= 180)
                    {
                        passHarf = true;
                        printf("passHarf");
                    }
                    if (passHarf && (angle > takeVideoAngle))
                    {
                        is_motor_stop = true;
                        ev3_motor_set_power(left_motor, 0);
                        ev3_motor_set_power(right_motor, 0);
                        arriveTakeVideoPosition = true;
                    }
                }
            }
            // 撮影位置到達後動画撮影
            if (!doneTakeVideo && arriveTakeVideoPosition)
            {
                // 90°曲げる、遠心力を考慮して14°手前で止める
                doneTakeVideo = takePhotoOfTrainAndLandscape(&motor_impals, &is_motor_stop, 90, 14); // ここを変更
            }
            if (doneTakeVideo)
            {
                if (angle > -10 && angle < 0)
                {
                    printf("loopedOndeをtrue");
                    printf("-time:%d\n", time);
                    is_motor_stop = false;
                    motor_impals = false;
                    loopedOnce = true;
                }
            }
        }

        // センサーが連続周回で壊れるため、一度静止＋値のリセット
        static bool_t resetedValue = false;
        static bool_t doneWait = false;
        static int waitTimer = 0;
        if(loopedOnce && !resetedValue){
            // 一度静止
            doneWait = waitMSecond(is_motor_stop, &waitTimer, 3);
            // 静止後、値のリセット
            if(doneWait){
                // センサー関係の値全リセット
                initialize_pid_value();
                ev3_gyro_sensor_reset(gyro_sensor);
                angle = ev3_gyro_sensor_get_angle(gyro_sensor);
                is_motor_stop = false;
                motor_impals = false;
                target_color = 180;

                resetedValue = true;
            }
        }

        /*
        正円２周目
        */ 
        if (loopedOnce && resetedValue)
        {
            printf("2周目");
            // 楕円交差点部分に向けてエッジ切り替え
            static bool_t changedEdge = false;
            if (angle > 110 && !changedEdge)
            {
                printf("【正円部】エッジ切り替え\n");
                is_motor_stop = false;
                motor_impals = false;
                initialize_pid_value();
                trace_edge = LEFT_EDGE;

                blue_line_count = 0;
                changedEdge = true;
                printf("time%d:\n", time);
            }
            // 交差点を突破
            if (changedEdge && blue_line_count == 1)
            {
                printf("----------------------------------【楕円突入】まっすぐGo----------------------------------");

                // 角度リセット
                static bool_t is_reset_angle = false;
                if (!is_reset_angle)
                {
                    ev3_gyro_sensor_reset(gyro_sensor);
                    angle = ev3_gyro_sensor_get_angle(gyro_sensor);
                    is_reset_angle = true;
                }

                motor_impals = true;
                ev3_motor_set_power(left_motor, 20); // 60
                ev3_motor_set_power(right_motor, 65);
                if ((time - latest_passed_blue_line_time) > 60)
                {
                    passPerfectCercle = true;

                    is_motor_stop = true;
                    ev3_motor_set_power(left_motor, 0); // 60
                    ev3_motor_set_power(right_motor, 0);

                    // motor_impals = false;
                    initialize_pid_value();
                    dynamic_base_speed = 45;
                    selected_pid_parameter = 1;
                    // target_color = 270;
                }
            }
        }
    }

    /*
     ミニフィグ撮影
    */
    // 撮影回数のフェーズを管理するフラグ
    static bool_t doFirstTask = true;
    static bool_t doneFirstTask = false;

    static bool_t doSecondTask = false;
    static bool_t doneSecondTask = false;

    static bool_t doThirdTask = false;
    static bool_t doneThirdTask = false;

    static bool_t doFourthTask = false;
    static bool_t doneFourthTask = false;

    static bool_t doFifthTask = false;
    static bool_t doneFifthTask = false;

    static bool_t doSixthTask = false;
    static bool_t doneSixthTask = false;

    //----------------------1回目：-50°---------------------------------------------//
    if (doFirstTask && passThePerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle < -50) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
        {
            // 安定性確保のため機体ストップ
            is_motor_stop = true;
            ev3_motor_set_power(left_motor, 0);
            ev3_motor_set_power(right_motor, 0);
            arrive = true;
        }

        // 撮影位置に到着すると撮影タスク開始
        if (arrive && !doneFirstTask)
        {
            printf("【楕円でのミニフィグ撮影】１回目");
            doneFirstTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 70, 12); // ここを変更
            if (doneFirstTask)
            {
                doSecondTask = true;
            }
        }
    }

    //----------------------2回目：-90°---------------------------------------------//
    if (doSecondTask && passThePerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle < -90) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
        {
            // 安定性確保のため機体ストップ
            is_motor_stop = true;
            ev3_motor_set_power(left_motor, 0);
            ev3_motor_set_power(right_motor, 0);
            arrive = true;
        }

        // 撮影位置に到着すると撮影タスク開始
        if (arrive && !doneSecondTask)
        {
            printf("【楕円でのミニフィグ撮影】２回目");
            doneSecondTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 82, 12); // ここを変更
            if (doneSecondTask)
            {
                doThirdTask = true;
            }
        }
    }

    //----------------------3回目：-100°---------------------------------------------//
    if (doThirdTask && passThePerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle < -110) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
        {
            // 安定性確保のため機体ストップ
            is_motor_stop = true;
            ev3_motor_set_power(left_motor, 0);
            ev3_motor_set_power(right_motor, 0);
            arrive = true;
        }

        // 撮影位置に到着すると撮影タスク開始
        if (arrive && !doneThirdTask)
        {
            printf("【楕円でのミニフィグ撮影】３回目");
            doneThirdTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 90, 12); // ここを変更
            if (doneThirdTask)
            {
                doFourthTask = true;
            }
        }
    }

    //----------------------4回目：-130°---------------------------------------------//
    if (doFourthTask && passThePerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle < -130) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
        {
            // 安定性確保のため機体ストップ
            is_motor_stop = true;
            ev3_motor_set_power(left_motor, 0);
            ev3_motor_set_power(right_motor, 0);
            arrive = true;
        }

        // 撮影位置に到着すると撮影タスク開始
        if (arrive && !doneFourthTask)
        {
            printf("【楕円でのミニフィグ撮影】４回目");
            doneFourthTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 140, 12); // ここを変更
            if (doneFourthTask)
            {
                doFifthTask = true;
            }
        }
    }

    //----------------------5回目：130°---------------------------------------------//
    static bool_t escapeEllipse = false;
    if (doFifthTask && passThePerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle < 130 && angle > 0) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
        {
            // 安定性確保のため機体ストップ
            is_motor_stop = true;
            ev3_motor_set_power(left_motor, 0);
            ev3_motor_set_power(right_motor, 0);
            arrive = true;
        }

        // 撮影位置に到着すると撮影タスク開始
        if (arrive && !doneFifthTask)
        {
            printf("【楕円でのミニフィグ撮影】５回目");
            doneFifthTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 60, 12); // ここを変更
            if (doneFifthTask)
            {
                doSixthTask = true;
            }
        }
    }

    //----------------------6回目：90°---------------------------------------------//
    if (doSixthTask && passThePerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle < 90 && angle > 0) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
        {
            // 安定性確保のため機体ストップ
            is_motor_stop = true;
            ev3_motor_set_power(left_motor, 0);
            ev3_motor_set_power(right_motor, 0);
            arrive = true;
        }

        // 撮影位置に到着すると撮影タスク開始
        if (arrive && !doneSixthTask)
        {
            printf("【楕円でのミニフィグ撮影】６回目");
            doneSixthTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 100, 12); // ここを変更
            if (doneSixthTask)
            {
                blue_line_count = 0;
                target_color = 280;
                escapeEllipse = true;
            }
        }
    }

    /*
        楕円を脱出し、正円に再侵入
    */
    static bool_t passEllipse = false;
    if (escapeEllipse && !passEllipse && blue_line_count == 1)
    {
        printf("----------------------------------【楕円脱出】まっすぐGo--------------------------------------------------------------------");
        motor_impals = true;
        ev3_motor_set_power(left_motor, 40);
        ev3_motor_set_power(right_motor, 60);
        if ((time - latest_passed_blue_line_time) > 40)
        { // 90
            selected_pid_parameter = 0;
            motor_impals = false;
            initialize_pid_value();
            passEllipse = true;
            blue_line_count = 0;
        }
    }

    /*
    ダブルループを脱出し、スマートキャリー攻略開始地点（青の〇）で止まる
    */
    if (passEllipse && check_Line_color_hsv(1))
    {
        is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);
    }

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

//------------------------プラレール・風景攻略用メソッド------------------------//
// メソッド全体のフラグ
static bool_t doTowardsCenterOfPerfectCircle = true;
static bool_t doBackToStartPointAtPerfectCircle = true;

static bool_t positionValueIsNull_backToStartPointAtPerfectCircle = true;
static bool_t isNeedAngleBuffer_backToStartPointAtPerfectCircle = false;
static bool_t doneTask_backToStartPointAtPerfectCircle = false;
static bool_t plarail_passShotTask = false;
// 境界を乗り越える場合か
static bool_t overcome_boundaries_backToStartPointAtPerfectCircle = false;

/* 正円にて、走行体を正円の中央に向ける */
int16_t towardsCenterOfPerfectCircle(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t moveAngle, int16_t inertiaAmount)
{
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    /*
    移動開始地点と到着地点の角度を設定
    */
    static bool_t positionValueIsNull = true;
    static int16_t startAngle;
    static int16_t arrivalAngle;
    if (positionValueIsNull)
    {
        printf("【正円での撮影タスク：行き】開始\n");
        startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);
        // 初期アングルから、到着角度を計算
        arrivalAngle = startAngle + (moveAngle - inertiaAmount);
        if (arrivalAngle > 180)
        {
            arrivalAngle = -181 + (abs(arrivalAngle) - 179);
        }
        positionValueIsNull = false;
        printf("startAngle : %d\n", startAngle);
        printf("arrivalAngle : %d\n", arrivalAngle);
    }

    /*
    開始地点と到着地点の角度に応じて、条件式を変更するためのフラグを調整
    */ 
    static bool_t isNeedAngleBuffer = false;
    static bool_t conditionIsInvert = false;
    // 到着角度が正負のギリギリ(ex. 179°など)の場合、少し過ぎて負に行っても止まるよう、猶予のフラグを立てる
    if (170 < arrivalAngle)
    {
        isNeedAngleBuffer = true;
    }
    // スタート位置が既に到着位置より大きい（スタート位置：正 到着位置：負）場合、移動継続の条件式を変えるよう、継続条件変更のフラグを立てる
    if (startAngle > arrivalAngle)
    {
        conditionIsInvert = true;
    }

    /*
    到着角度まで車輪を動かす
    */ 
    static bool_t arrive = false;
    if(!arrive){
        // 到着角度になるまで車輪を動かす
        if (!conditionIsInvert && (angle < arrivalAngle))
        {
            printf("【正円での撮影タスク：行き】移動中・・・\n");
            // 170~180のものは、負になった時点で動作を止める
            if (isNeedAngleBuffer && 0 > angle)
            {
                printf("【正円での撮影タスク：行き】撮影位置に到着");
                arrive = true;
            }
            ev3_motor_set_power(right_motor, -60);
        }
        // 到着角度になるまで車輪を動かす(正→負に動かす場合の条件式)
        else if (conditionIsInvert && (angle > 0 || angle < arrivalAngle))
        {
            printf("【正円での撮影タスク：行き】移動中・・・\n");
            ev3_motor_set_power(right_motor, -60);
        }
        // 撮影地点に到達した場合
        else
        {
            printf("【正円での撮影タスク：行き】撮影位置に到着");
            arrive = true;
        }
    }

    if (arrive)
    {
        // 動作停止
        is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);

        // このタスクのフラグをリセット
        positionValueIsNull = true;
        isNeedAngleBuffer = false;
        conditionIsInvert = false;
        arrive = false;

        // タスク終了フラグ更新
        doTowardsCenterOfPerfectCircle = false;
        
        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;
        printf("【正円での撮影タスク：行き】終了\n");
        return startAngle;
    }
}

/* 正円にて、撮影位置からライン上に復帰する */
bool_t backToStartPointAtPerfectCircle(int16_t startAngle, bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t inertiaAmount)
{
    printf("【正円での撮影タスク：帰り】開始\n");
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    // 帰り用の到着角度を計算
    static int16_t startAngle_backToStartPointAtPerfectCircle;
    static int16_t arrivalAngle_backToStartPointAtPerfectCircle;

    if (positionValueIsNull_backToStartPointAtPerfectCircle)
    {
        startAngle_backToStartPointAtPerfectCircle = ev3_gyro_sensor_get_angle(gyro_sensor);

        arrivalAngle_backToStartPointAtPerfectCircle = startAngle + inertiaAmount;

        if (arrivalAngle_backToStartPointAtPerfectCircle > 180)
        {
            arrivalAngle_backToStartPointAtPerfectCircle = -181 + (abs(arrivalAngle_backToStartPointAtPerfectCircle) - 179);
        }
        positionValueIsNull_backToStartPointAtPerfectCircle = false;
        printf("startAngle_backToStartPointAtPerfectCircle : %d\n", startAngle_backToStartPointAtPerfectCircle);
        printf("arrivalAngle_backToStartPointAtPerfectCircle : %d\n", arrivalAngle_backToStartPointAtPerfectCircle);
    }

    if (-170 > arrivalAngle_backToStartPointAtPerfectCircle && !isNeedAngleBuffer_backToStartPointAtPerfectCircle)
    {
        isNeedAngleBuffer_backToStartPointAtPerfectCircle = true;
    }

    // 境界を乗り越える場合 フラグをtrue
    if ((startAngle_backToStartPointAtPerfectCircle < 0 && arrivalAngle_backToStartPointAtPerfectCircle > 0) && !overcome_boundaries_backToStartPointAtPerfectCircle)
    {
        overcome_boundaries_backToStartPointAtPerfectCircle = true;
    }

    // 復帰地点まで動かす

    printf("【正円での撮影タスク：帰り】撮影位置への移動開始\n");
    if (arrivalAngle_backToStartPointAtPerfectCircle < angle && !doneTask_backToStartPointAtPerfectCircle && !overcome_boundaries_backToStartPointAtPerfectCircle)
    {
        printf("【正円での撮影タスク：帰り】移動中\n");
        // -170~-179のものは、正になった時点で動作を止める
        if (isNeedAngleBuffer_backToStartPointAtPerfectCircle && 0 < angle)
        {
            printf("バッファで止める");
            doneTask_backToStartPointAtPerfectCircle = true;
        }
        ev3_motor_set_power(right_motor, 60);
    }
    else if (overcome_boundaries_backToStartPointAtPerfectCircle && (angle < 0 || arrivalAngle_backToStartPointAtPerfectCircle < angle) && !doneTask_backToStartPointAtPerfectCircle)
    {
        printf("【正円での撮影タスク：帰り】移動中 【乗り越えver】\n");
        ev3_motor_set_power(right_motor, 60);
    }
    else
    {
        printf("正常系で止める");
        doneTask_backToStartPointAtPerfectCircle = true;
    }

    if (doneTask_backToStartPointAtPerfectCircle)
    {
        // 動作停止
        is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);

        // タスク終了フラグ更新
        doBackToStartPointAtPerfectCircle = false;
        doneTask_backToStartPointAtPerfectCircle = false;

        // pidパラメータリセット
        initialize_pid_value();

        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;
        printf("【正円での撮影タスク：帰り】終了\n");
        return true;
    }
}

/* プラレール・風景攻略の実行メソッド */
bool_t takePhotoOfTrainAndLandscape(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int moveAngle, int inertiaAmount)
{
    // 動作を停止させる
    ev3_motor_set_power(left_motor, 0);
    ev3_motor_set_power(right_motor, 0);

    // 走行体を撮影方向に向ける
    static int16_t startAngle;

    static bool_t doneTask = false;

    if (doTowardsCenterOfPerfectCircle)
    {
        startAngle = towardsCenterOfPerfectCircle(pointer_motor_impals, is_motor_stop, moveAngle, inertiaAmount);
    }

    // 写真を撮って、パーフェクトショットか判定
    if (!plarail_passShotTask && !doTowardsCenterOfPerfectCircle)
    {
        static int timeCount = 0;
        plarail_passShotTask = waitMSecond(is_motor_stop, &timeCount, 10);

        // 動画撮影のPythonの実行
        static bool_t executedPython = false;
        if (!executedPython)
        {
            char fileName[] = "etrobo2023/flagFolder/movie_flag.txt";
            FILE *file = fopen(fileName, "w");
            if (!(file == NULL))
            {
                printf("%sの作成に成功", fileName);
                executedPython = true;
            }
        }
    }

    // 元の位置に戻す
    if (doBackToStartPointAtPerfectCircle && !doTowardsCenterOfPerfectCircle && plarail_passShotTask)
    {
        doneTask = backToStartPointAtPerfectCircle(startAngle, pointer_motor_impals, is_motor_stop, inertiaAmount);
    }

    if (doneTask)
    {
        doneTask = false;
        return true;
    }
}

//-------------------------------------------------------------------//

//------------------------ミニフィグ攻略用メソッド------------------------//

// メソッド全体のフラグ
static bool_t doTowardsCenterOfEllipse = true;
static bool_t doBackToStartPointAtEllipse = true;

static bool_t positionValueIsNull_towardsCenterOfEllipse = true;
static bool_t isNeedAngleBuffer_towardsCenterOfEllipse = false;
static bool_t doneTask_towardsCenterOfEllipse = false;
static bool_t positionValueIsNull_backToStartPointAtEllipse = true;
static bool_t isNeedAngleBuffer_backToStartPointAtEllipse = false;
static bool_t doneTask_backToStartPointAtEllipse = false;
static bool_t minifig_passShotTask = false;
// 境界を乗り越える場合か
static bool_t overcome_boundaries_towardsCenterOfEllipse = false;
static bool_t overcome_boundaries_backToStartPointAtEllipse = false;

/* 楕円にて、走行体を楕円の中央に向ける */
int16_t towardsCenterOfEllipse(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t moveAngle, int16_t inertiaAmount)
{
    printf("【楕円での撮影タスク：行き】開始\n");
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    // 初期アングルから、到着角度を計算
    static int16_t startAngle;
    static int16_t arrivalAngle;

    if (positionValueIsNull_towardsCenterOfEllipse)
    {
        startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);

        arrivalAngle = startAngle - (moveAngle - inertiaAmount);

        if (arrivalAngle < -179)
        {
            arrivalAngle = 181 - (abs(arrivalAngle) - 179);
        }
        positionValueIsNull_towardsCenterOfEllipse = false;
        printf("startAngle : %d\n", startAngle);
        printf("arrivalAngle : %d\n", arrivalAngle);
    }

    // 正負の間際の場合角度に猶予をつける
    if (-170 > arrivalAngle && !isNeedAngleBuffer_towardsCenterOfEllipse)
    {
        isNeedAngleBuffer_towardsCenterOfEllipse = true;
    }

    // 境界を乗り越える場合 フラグをtrue
    if ((startAngle < 0 && arrivalAngle > 0) && !overcome_boundaries_towardsCenterOfEllipse)
    {
        overcome_boundaries_towardsCenterOfEllipse = true;
    }

    // 到着角度になるまで車輪を動かす
    printf("【楕円での撮影タスク：行き】撮影位置への移動開始\n");
    if ((angle > arrivalAngle) && !doneTask_towardsCenterOfEllipse && !overcome_boundaries_towardsCenterOfEllipse)
    {
        printf("【楕円での撮影タスク：行き】移動中\n");
        // -170~-179のものは、負になった時点で動作を止める
        if (isNeedAngleBuffer_towardsCenterOfEllipse && 0 < angle)
        {
            printf("バッファで止める");
            doneTask_towardsCenterOfEllipse = true;
        }
        ev3_motor_set_power(right_motor, 60);
    }
    else if (overcome_boundaries_towardsCenterOfEllipse && (angle < 0 || angle > arrivalAngle) && !doneTask_towardsCenterOfEllipse)
    {
        printf("【楕円での撮影タスク：行き】移動中 【乗り越えver】\n");
        ev3_motor_set_power(right_motor, 60);
    }
    else
    {
        printf("正常系で止める");
        doneTask_towardsCenterOfEllipse = true;
    }

    if (doneTask_towardsCenterOfEllipse)
    {
        // 動作停止
        is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);

        // タスク終了フラグ更新
        doTowardsCenterOfEllipse = false;
        doneTask_towardsCenterOfEllipse = false;
        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;
        printf("【楕円での撮影タスク：行き】終了\n");
        return startAngle;
    }
}

/* 楕円にて、撮影位置からライン上に復帰する */
bool_t backToStartPointAtEllipse(int16_t startAngle, bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t inertiaAmount)
{
    printf("【楕円での撮影タスク：帰り】開始\n");
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    // 帰り用の到着角度を計算
    static int16_t startAngle_backToStartPointAtEllipse;
    static int16_t arrivalAngle_backToStartPointAtEllipse;

    if (positionValueIsNull_backToStartPointAtEllipse)
    {
        startAngle_backToStartPointAtEllipse = ev3_gyro_sensor_get_angle(gyro_sensor);

        arrivalAngle_backToStartPointAtEllipse = startAngle - inertiaAmount;

        if (arrivalAngle_backToStartPointAtEllipse < -179)
        {
            arrivalAngle_backToStartPointAtEllipse = 181 - (abs(arrivalAngle_backToStartPointAtEllipse) - 179);
        }
        positionValueIsNull_backToStartPointAtEllipse = false;
        printf("startAngle_backToStartPointAtEllipse : %d\n", startAngle_backToStartPointAtEllipse);
        printf("arrivalAngle_backToStartPointAtEllipse : %d\n", arrivalAngle_backToStartPointAtEllipse);
    }

    if (170 < arrivalAngle_backToStartPointAtEllipse && !isNeedAngleBuffer_backToStartPointAtEllipse)
    {
        isNeedAngleBuffer_backToStartPointAtEllipse = true;
    }

    // 境界を乗り越える場合 フラグをtrue
    if ((startAngle_backToStartPointAtEllipse > 0 && arrivalAngle_backToStartPointAtEllipse < 0) && !overcome_boundaries_backToStartPointAtEllipse)
    {
        overcome_boundaries_backToStartPointAtEllipse = true;
    }

    // 復帰地点まで動かす
    printf("【楕円での撮影タスク：帰り】撮影位置への移動開始\n");
    if (arrivalAngle_backToStartPointAtEllipse > angle && !doneTask_backToStartPointAtEllipse && !overcome_boundaries_backToStartPointAtEllipse)
    {
        printf("【楕円での撮影タスク：帰り】移動中\n");
        // 170~180のものは、負になった時点で動作を止める
        if (isNeedAngleBuffer_backToStartPointAtEllipse && 0 > angle)
        {
            printf("バッファで止める");
            doneTask_backToStartPointAtEllipse = true;
        }
        ev3_motor_set_power(right_motor, -60);
    }
    else if (overcome_boundaries_backToStartPointAtEllipse && (angle > 0 || arrivalAngle_backToStartPointAtEllipse > angle) && !doneTask_backToStartPointAtEllipse)
    {
        printf("【楕円での撮影タスク：帰り】移動中 【乗り越えver】\n");
        ev3_motor_set_power(right_motor, -60);
    }
    else
    {
        printf("正常系で止める");
        doneTask_backToStartPointAtEllipse = true;
    }

    if (doneTask_backToStartPointAtEllipse)
    {
        // 動作停止
        is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);

        // タスク終了フラグ更新
        doBackToStartPointAtEllipse = false;
        doneTask_backToStartPointAtEllipse = false;

        // pidパラメータリセット
        initialize_pid_value();

        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;

        printf("【楕円での撮影タスク：帰り】終了\n");
        return true;
    }
}

/* ミニフィグ攻略の実行メソッド */
bool_t takePhotoOfMinifig(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int moveAngle, int inertiaAmount)
{

    static bool_t resetFlag;
    resetFlag = false;

    // 動作を停止させる
    ev3_motor_set_power(left_motor, 0);
    ev3_motor_set_power(right_motor, 0);

    // 走行体を撮影方向に向ける
    static int16_t startAngle;

    if (doTowardsCenterOfEllipse)
    {
        startAngle = towardsCenterOfEllipse(pointer_motor_impals, is_motor_stop, moveAngle, inertiaAmount);
    }

    // 写真を撮って、パーフェクトショットか判定
    static bool_t executedPython = false;
    static int timeCount = 0;
    if (!minifig_passShotTask && !doTowardsCenterOfEllipse)
    {
        minifig_passShotTask = waitMSecond(is_motor_stop, &timeCount, 5);
        static int minifigSnapNumber = 0;
        if (!executedPython)
        {
            printf("撮影実行\n");
            executedPython = makeMinifigFile(minifigSnapNumber);
            minifigSnapNumber++;
        }
    }

    // 画像送信

    // 元の位置に戻す
    if (doBackToStartPointAtEllipse && !doTowardsCenterOfEllipse && minifig_passShotTask)
    {
        resetFlag = backToStartPointAtEllipse(startAngle, pointer_motor_impals, is_motor_stop, inertiaAmount);
    }

    // このタスクのフラグをすべてリセット
    if (resetFlag)
    {
        doTowardsCenterOfEllipse = true;
        doBackToStartPointAtEllipse = true;
        positionValueIsNull_towardsCenterOfEllipse = true;
        isNeedAngleBuffer_towardsCenterOfEllipse = false;
        doneTask_towardsCenterOfEllipse = false;
        positionValueIsNull_backToStartPointAtEllipse = true;
        isNeedAngleBuffer_backToStartPointAtEllipse = false;
        doneTask_backToStartPointAtEllipse = false;
        minifig_passShotTask = false;
        overcome_boundaries_towardsCenterOfEllipse = false;
        overcome_boundaries_backToStartPointAtEllipse = false;
        timeCount = 0;
        executedPython = false;
        return true;
    }
}

//-------------------------------------------------------------------//

/* 指定された秒数停止する */
bool_t waitMSecond(bool_t *is_motor_stop, int *watingTime, int mSecond)
{
    (*watingTime)++;
    if (*watingTime > (mSecond * 100))
    {
        *is_motor_stop = false;
        printf("%d秒待機終了\n", mSecond);
        return true;
    }
    else
    {
        printf("待機中・・・\n");
        *is_motor_stop = true;
        ev3_motor_set_power(left_motor, 0);
        ev3_motor_set_power(right_motor, 0);
        return false;
    }
}

/* ファイルを作成する */
bool_t makeMinifigFile(int fileNum)
{
    bool_t result = false;
    char fileName[100];
    sprintf(fileName, "etrobo2023/flagFolder/capture_flag_%d.txt", fileNum);

    FILE *file = fopen(fileName, "w");
    if (!(file == NULL))
    {
        printf("%sの作成に成功", fileName);
        result = true;
    }
    return result;
}

// /* フォルダ内に画像があるか検索する */
// bool_t searchPicture(const char *directory, const char *filename)
// {
//     bool_t exist = false;

//     DIR *dir = opendir(directory);
//     if (dir == NULL)
//     {
//         return exist;
//     }

//     struct dirent *entry;

//     while ((entry = readdir(dir)) != NULL)
//     {
//         if (strcmp(entry->d_name, filename) == 0)
//         {
//             exist = true;
//             break;
//         }
//     }

//     closedir(dir);

//     if (exist)
//     {
//         printf("ファイル '%s' はディレクトリ '%s' に存在します。\n", filename, directory);
//     }
//     else
//     {
//         printf("ファイル '%s' はディレクトリ '%s' に存在しません。\n", filename, directory);
//     }

//     return exist;
// }

// 以下を、葛目君と相談して決定
//     const char *directory = "dirPath"; // 調べたいディレクトリのパス
//     char filename[256];
//     const char *filename = "fileName"; // 調べたいファイル名

//     searchPicture(directory, filename);
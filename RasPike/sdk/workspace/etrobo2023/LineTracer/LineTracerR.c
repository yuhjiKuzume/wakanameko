#include "app.h"
#include "LineTracerR.h"
#include "PidControl.h"
#include "ColorControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

static int time; // 　制御時間
static int trace_edge = RIGHT_EDGE;
static int dynamic_base_speed = 60;
static int target_color = 170;
static int latest_passed_blue_line_time = 0;

// 超えてから計測開始時刻
static int timeAtLapPoint = 0;

static int16_t angle;

/* ライントレースタスク(10msec周期で関数コールされる) */
void tracer_task_R(intptr_t unused)
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
    static bool_t reachSecondCurve = false;
    if (((abs(angle) > 170)|| time > 1500) && !reachSecondCurve)
    {
        timeAtLapPoint = time;
        reachSecondCurve = true;
        dynamic_base_speed = 45;
    }

    /*
    LAP付近にきたら、左側トレースに切り替える
    */
    static bool_t passLap = false;
    if (reachSecondCurve && (time - timeAtLapPoint) > 100 && !passLap)
    // if (!passLap) // ダブルループ実験用
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

        trace_edge = LEFT_EDGE;
        passLap = true;
    }

    /*
        ダブルループに進入
            ・ライントレースの目標値変更
    */
    static bool_t enteringDoubleLoop = false;
    if (blue_line_count == 1 && angle < -30 && !enteringDoubleLoop)
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
    static int16_t takeVideoAngle = 5;     // 110 // ここを変更 // 前半部分を撮影する場合、160°まで
    static bool_t takeVideoAtFirst = false; // 前半部分か後半部分を撮影するかを決定　// ここを変更

    static bool_t passPerfectCercle = false;
    static bool_t doneTakeVideo = false;

    static bool_t setDoneTakeVideoTime = false;
    static int doneTakeVideoTime;
    if (!setDoneTakeVideoTime && doneTakeVideo)
    {
        doneTakeVideoTime = time;
        setDoneTakeVideoTime = true;
    }

    // 前半部で撮影する場合
    if (takeVideoAtFirst)
    {
        if (enteringDoubleLoop && !passPerfectCercle)
        {
            static bool_t arriveTakeVideoPosition = false;
            static bool_t passHarf = false;

            // 撮影位置まで移動
            if (!arriveTakeVideoPosition)
            {
                // ビデオ撮影箇所が正円前半の場合
                if (takeVideoAngle <= 0)
                {
                    if (angle < takeVideoAngle)
                    { // 機体ストップ
                        is_motor_stop = true;
                        ev3_motor_set_power(left_motor, 0);
                        ev3_motor_set_power(right_motor, 0);
                        arriveTakeVideoPosition = true;
                    }
                    if (angle > 170)
                    {
                        is_motor_stop = true;
                        ev3_motor_set_power(left_motor, 0);
                        ev3_motor_set_power(right_motor, 0);
                        arriveTakeVideoPosition = true;
                    }
                }
                // ビデオ撮影箇所が正円後半の場合
                else
                {
                    if (!passHarf && angle > 170)
                    {
                        passHarf = true;
                        printf("passHarf");
                    }
                    if (passHarf && (angle < takeVideoAngle))
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
            // 楕円交差点部分に向けてエッジ切り替え
            static bool_t changedEdge = false;
            if (doneTakeVideo)
            {
                if (!changedEdge)
                {
                    if (setDoneTakeVideoTime && (time - doneTakeVideoTime) > 50)
                    {
                        printf("【正円部】エッジ切り替え\n");
                        is_motor_stop = false;
                        motor_impals = false;
                        initialize_pid_value();
                        trace_edge = RIGHT_EDGE;

                        blue_line_count = 0;
                        changedEdge = true;
                        printf("time:%d\n", time);
                    }
                }

                // 楕円突破に向けて、遅くするタイミング　
                static bool_t slowDown = false;
                if ((angle < -174 || angle > 0) && !slowDown && changedEdge)
                {
                    printf("------------------------------------楕円突入のため減速------------------------------------\n");
                    slowDown = true;
                    dynamic_base_speed = 35;
                    target_color = 270;
                }
            }

            // 交差点を突破
            static bool_t atCrossing = false;
            if (changedEdge && blue_line_count == 1)
            {
                printf("----------------------------------【楕円突入】まっすぐGo--------------------------------------------------------------------\n");
                if (!atCrossing)
                {
                    ev3_gyro_sensor_reset(gyro_sensor);
                    angle = ev3_gyro_sensor_get_angle(gyro_sensor);
                    atCrossing = true;
                }

                motor_impals = true;
                ev3_motor_set_power(left_motor, 34);
                ev3_motor_set_power(right_motor, 42);
                if ((time - latest_passed_blue_line_time) > 17)
                {
                    passPerfectCercle = true;
                    selected_pid_parameter = 1;
                    motor_impals = false;
                    initialize_pid_value();
                    dynamic_base_speed = 32;
                    printf("まっすぐGo終了");
                }
            }
        }
    }
    if (!takeVideoAtFirst)
    {
        if (enteringDoubleLoop && !passPerfectCercle)
        {
            // 楕円交差点部分に向けてエッジ切り替え
            static bool_t changedEdge = false;
            if (angle < -110 && !changedEdge) // ここを変更
            {
                printf("【正円部】エッジ切り替え\n");
                is_motor_stop = false;
                motor_impals = false;
                initialize_pid_value();
                trace_edge = RIGHT_EDGE;

                blue_line_count = 0;
                changedEdge = true;
                printf("time:%d\n", time);
            }
            // 楕円突破に向けて、遅くするタイミング　
            static bool_t slowDown = false;
            if (abs(angle) > 174 && !slowDown && changedEdge)
            {
                printf("------------------------------------楕円突入のため減速------------------------------------\n");
                slowDown = true;
                dynamic_base_speed = 35;
                target_color = 270;
            }
            // 交差点を突破
            static bool_t atCrossing = false;
            if (changedEdge && blue_line_count == 1)
            {
                printf("----------------------------------【楕円突入】まっすぐGo--------------------------------------------------------------------\n");
                if (!atCrossing)
                {
                    ev3_gyro_sensor_reset(gyro_sensor);
                    angle = ev3_gyro_sensor_get_angle(gyro_sensor);
                    atCrossing = true;
                }

                motor_impals = true;
                ev3_motor_set_power(left_motor, 34);
                ev3_motor_set_power(right_motor, 42);
                if ((time - latest_passed_blue_line_time) > 17)
                {
                    passPerfectCercle = true;
                    selected_pid_parameter = 1;
                    motor_impals = false;
                    initialize_pid_value();
                    dynamic_base_speed = 32;
                    printf("まっすぐGo終了");
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

    //----------------------1回目：50°---------------------------------------------//
    if (doFirstTask && passPerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle > 50) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
        {
            // 速度とベースカラーを戻す
            dynamic_base_speed = 45;
            target_color = 180;

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
            doneFirstTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 65, 12); // ここを変更
            if (doneFirstTask)
            {
                doSecondTask = true;
            }
        }
    }

    //----------------------2回目：-100°---------------------------------------------//
    if (doSecondTask && passPerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle > 100) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
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
            doneSecondTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 140, 12); // ここを変更
            if (doneSecondTask)
            {
                doThirdTask = true;
            }
        }
    }

    //----------------------3回目：-130°---------------------------------------------//
    if (doThirdTask && passPerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle > -130 && angle <= 0) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
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
            doneThirdTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 75, 12); // ここを変更
            if (doneThirdTask)
            {
                doFourthTask = true;
            }
        }
    }

    //----------------------4回目：-85°---------------------------------------------//
    static bool_t escapeEllipse = false;
    if (doFourthTask && passPerfectCercle)
    {
        // 撮影位置まで移動
        static bool_t arrive = false;
        if (!arrive && angle > -85 && angle <= 0) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
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
            doneFourthTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 135, 12); // ここを変更 // 120
            if (doneFourthTask)
            {
                blue_line_count = 0;
                dynamic_base_speed = 35;
                trace_edge = LEFT_EDGE;
                target_color = 180;
                escapeEllipse = true;
            }
        }
    }

    /*
        楕円を脱出し、正円に再侵入
    */
    static bool_t passEllipse = false;
    static bool_t atCrossing = false;

    static int doneMassuguGOTime;

    if (escapeEllipse && !passEllipse && blue_line_count == 1)
    {
        printf("----------------------------------【楕円脱出】まっすぐGo--------------------------------------------------------------------");
        if (!atCrossing)
        {
            ev3_gyro_sensor_reset(gyro_sensor);
            angle = ev3_gyro_sensor_get_angle(gyro_sensor);
            atCrossing = true;
        }

        motor_impals = true;
        ev3_motor_set_power(left_motor, 50);
        ev3_motor_set_power(right_motor, 35);
        if ((time - latest_passed_blue_line_time) > 10)
        {
            selected_pid_parameter = 1;
            motor_impals = false;
            initialize_pid_value();
            passEllipse = true;
            blue_line_count = 0;
            doneMassuguGOTime = time;
            dynamic_base_speed = 32;
            printf("まっすぐGo終了");
        }
    }

    // 楕円脱出後 一定期間経過後に速度40に設定
    static bool_t setSpeedPassEllipse = false;
    if (!setSpeedPassEllipse)
    {
        if ((passEllipse && (time - doneMassuguGOTime) > 200))
        {
            dynamic_base_speed = 40;
            setSpeedPassEllipse = true;
            printf("楕円脱出後 速度40に設定\n");
        }
    }

    // 後半部分で撮影する場合
    if (!takeVideoAtFirst)
    {

        if (passEllipse)
        {
            static bool_t arriveTakeVideoPosition = false;

            // 撮影位置まで移動
            if (!arriveTakeVideoPosition)
            {
                if (passEllipse && (time - doneMassuguGOTime) > 50)
                {
                    if (angle < takeVideoAngle)
                    { // 機体ストップ
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
        }
    }

    /*
    楕円脱出後 エッジ切り替え
    */
    // 楕円脱出まっすぐGOと(後半部でプラレール撮影がある場合を考慮して)プラレール撮影が両方終了していることを保証するフラグ
    static bool_t isAbleConditon = false;
    if (!isAbleConditon)
    {
        if ((passEllipse && (time - doneMassuguGOTime) > 200) && (setDoneTakeVideoTime && (time - doneTakeVideoTime) > 50))
        {
            isAbleConditon = true;
            printf("楕円脱出後 エッジ切り替えが可能になりました。\n");
        }
    }
    // 条件を満たしたので、楕円脱出後エッジ切り替え
    static bool_t chengedEdgeAfterEllipse = false;
    if (isAbleConditon && !chengedEdgeAfterEllipse)
    {
        printf("楕円脱出後 エッジ切り替え");
        selected_pid_parameter = 0;
        trace_edge = RIGHT_EDGE;
        chengedEdgeAfterEllipse = true;
        target_color = 150;
        blue_line_count = 0;
    }

    /*
    ダブルループを脱出し、スマートキャリー攻略開始地点（青の〇）で止まる
    */
    static bool_t startSmartCarry = false;
    if (chengedEdgeAfterEllipse && !startSmartCarry)
    {
        if (blue_line_count == 1)
        {
            dynamic_base_speed = 45;
            if ((time - latest_passed_blue_line_time) > 400)
            {
                printf("ダブルループを脱出し、スマートキャリー攻略開始地点（青の〇）で止まる");
                is_motor_stop = true;
                ev3_motor_set_power(left_motor, 0);
                ev3_motor_set_power(right_motor, 0);

                static int waitTimer = 0;
                startSmartCarry = true;
                // リセット
                blue_line_count = 0;
                initialize_pid_value();
                ev3_gyro_sensor_reset(gyro_sensor);
                angle = ev3_gyro_sensor_get_angle(gyro_sensor);
            }
        }
    }

    // スマートキャリー攻略のpythonを起動する為、フラグとなるtxtファイルを作成
    if (startSmartCarry)
    {
        printf("スマートキャリー攻略のフラグを作成");
        char fileName[] = "etrobo2023/flagFolder/startSmartCarry.txt";
        FILE *file = fopen(fileName, "w");
        if (!(file == NULL))
        {
            printf("%sの作成に成功", fileName);
            startSmartCarry = false;
        }
        fclose(file);
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

//------------------------プラレール・風景攻略用メソッド------------------------//
// 行き・帰りを管理するフラグ
static bool_t doTowardsCenterOfPerfectCircle = true;
static bool_t doBackToStartPointAtPerfectCircle = true;

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
        arrivalAngle = startAngle - (moveAngle - inertiaAmount);
        if (arrivalAngle < -180)
        {
            arrivalAngle = 180 - (abs(arrivalAngle) - 180);
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
    if (-170 > arrivalAngle)
    {
        isNeedAngleBuffer = true;
    }
    // スタート位置が既に到着位置より大きい（スタート位置：正 到着位置：負）場合、移動継続の条件式を変えるよう、継続条件変更のフラグを立てる
    if (startAngle < arrivalAngle)
    {
        conditionIsInvert = true;
    }

    /*
    到着角度まで車輪を動かす
    */
    static bool_t arrive = false;
    if (!arrive)
    {
        // 到着角度になるまで車輪を動かす
        if (!conditionIsInvert && (angle > arrivalAngle))
        {
            printf("【正円での撮影タスク：行き】移動中・・・\n");
            // 170~179のものは、負になった時点で動作を止める
            if (isNeedAngleBuffer && 0 < angle)
            {
                printf("【正円での撮影タスク：行き】撮影位置に到着");
                arrive = true;
            }
            ev3_motor_set_power(left_motor, -60);
        }
        // 到着角度になるまで車輪を動かす(正→負に動かす場合の条件式)
        else if (conditionIsInvert && (angle < 0 || angle > arrivalAngle))
        {
            printf("【正円での撮影タスク：行き】移動中・・・\n");
            ev3_motor_set_power(left_motor, -60);
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
bool_t backToStartPointAtPerfectCircle(int16_t arrivalAngle, bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t inertiaAmount)
{
    printf("【正円での撮影タスク：帰り】開始\n");
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    /*
    現在位置と復帰地点の角度を設定
    */
    static int16_t startAngle;
    static bool_t startAngleIsNull = true;
    if (startAngleIsNull)
    {
        startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);
        startAngleIsNull = false;
    }

    // 行きメソッドの開始地点から、復帰角度を逆算
    arrivalAngle = arrivalAngle - inertiaAmount;
    if (arrivalAngle < -180)
    {
        arrivalAngle = 180 - (abs(arrivalAngle) - 180);
    }

    /*
    現在地点と復帰地点の角度に応じて、条件式を変更するためのフラグを調整
    */
    static bool_t isNeedAngleBuffer = false;
    static bool_t conditionIsInvert = false;
    // 到着角度が正負のギリギリ(ex. -179°など)の場合、少し過ぎて正に行っても止まるよう、猶予のフラグを立てる
    if (170 < arrivalAngle && !isNeedAngleBuffer)
    {
        isNeedAngleBuffer = true;
    }
    // スタート位置が既に到着位置より小さい（スタート位置：負 到着位置：正）場合、移動継続の条件式を変えるよう、継続条件変更のフラグを立てる
    if (startAngle > arrivalAngle && !conditionIsInvert)
    {
        conditionIsInvert = true;
    }

    /*
    復帰地点角度まで車輪を動かす
    */
    bool_t arrive = false;
    if (!arrive)
    {
        // 到着角度になるまで車輪を動かす
        if (arrivalAngle > angle && !conditionIsInvert)
        {
            printf("【正円での撮影タスク：帰り】移動中・・・\n");
            // -170~-180のものは、正になった時点で動作を止める
            if (isNeedAngleBuffer && 0 > angle)
            {
                arrive = true;
            }
            ev3_motor_set_power(left_motor, 60);
        }
        // 到着角度になるまで車輪を動かす(負→正に動かす場合の条件式)
        else if (conditionIsInvert && (angle > 0 || arrivalAngle > angle))
        {
            printf("【正円での撮影タスク：帰り】移動中・・・\n");
            ev3_motor_set_power(left_motor, 60);
        }
        // 撮影地点に到達した場合
        else
        {
            printf("【正円での撮影タスク：帰り】元のライン上に到着");
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
        isNeedAngleBuffer = false;
        conditionIsInvert = false;
        arrive = false;

        // タスク終了フラグ更新
        doBackToStartPointAtPerfectCircle = false;

        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;
        printf("【正円での撮影タスク：帰り】終了\n");
        return true;
    }
}

/* プラレール・風景攻略の実行メソッド */
bool_t takePhotoOfTrainAndLandscape(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int moveAngle, int inertiaAmount)
{
    // プラレール・風景撮影完了フラグ
    static bool_t doneTask = false;

    // 動作を停止させる
    ev3_motor_set_power(left_motor, 0);
    ev3_motor_set_power(right_motor, 0);

    // 走行体を撮影方向に向ける
    static int16_t startAngle;
    if (doTowardsCenterOfPerfectCircle)
    {
        startAngle = towardsCenterOfPerfectCircle(pointer_motor_impals, is_motor_stop, moveAngle, inertiaAmount);
    }

    // 写真を撮って、パーフェクトショットか判定
    static bool_t snaped = false;
    if (!snaped && !doTowardsCenterOfPerfectCircle)
    {
        static int timeCount = 0;
        snaped = waitMSecond(is_motor_stop, &timeCount, 6);

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
    if (doBackToStartPointAtPerfectCircle && !doTowardsCenterOfPerfectCircle && snaped)
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

/* 楕円にて、走行体を楕円の中央に向ける */
int16_t towardsCenterOfEllipse(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t moveAngle, int16_t inertiaAmount)
{
    printf("【楕円での撮影タスク：行き】開始\n");
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
        startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);
        arrivalAngle = startAngle + (moveAngle - inertiaAmount);

        if (arrivalAngle > 179)
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
    // 到着角度が正負のギリギリ(ex. -179°など)の場合、少し過ぎて正に行っても止まるよう、猶予のフラグを立てる
    if (170 < arrivalAngle && !isNeedAngleBuffer)
    {
        isNeedAngleBuffer = true;
    }
    // スタート位置が既に到着位置より大きい（スタート位置：正 到着位置：負）場合、移動継続の条件式を変えるよう、継続条件変更のフラグを立てる
    if (startAngle > arrivalAngle && !conditionIsInvert)
    {
        conditionIsInvert = true;
    }

    /*
    到着角度まで車輪を動かす
    */
    static bool_t arrive = false;
    if (!arrive)
    {
        // 到着角度になるまで車輪を動かす
        if ((angle < arrivalAngle) && !conditionIsInvert)
        {
            printf("【楕円での撮影タスク：行き】移動中・・・\n");
            // 170~180のものは、負になった時点で動作を止める
            if (isNeedAngleBuffer && 0 > angle)
            {
                arrive = true;
            }
            ev3_motor_set_power(left_motor, 60);
        }
        // 到着角度になるまで車輪を動かす(正→負に動かす場合の条件式)
        else if (conditionIsInvert && (angle > 0 || angle < arrivalAngle))
        {
            printf("【楕円での撮影タスク：行き】移動中・・・\n");
            ev3_motor_set_power(left_motor, 60);
        }
        // 撮影地点に到達した場合
        else
        {
            printf("【楕円での撮影タスク：行き】撮影位置に到着");
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
        isNeedAngleBuffer = false;
        conditionIsInvert = false;
        positionValueIsNull = true;
        arrive = false;

        // タスク終了フラグ更新
        doTowardsCenterOfEllipse = false;

        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;
        printf("【楕円での撮影タスク：行き】終了\n");
        return startAngle;
    }
}

/* 楕円にて、撮影位置からライン上に復帰する */
bool_t backToStartPointAtEllipse(int16_t arrivalAngle, bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t inertiaAmount)
{
    printf("【楕円での撮影タスク：帰り】開始\n");
    // このタスク終了までライントレースを切る
    *pointer_motor_impals = true;

    /*
    現在位置と復帰地点の角度を設定
    */
    static int16_t startAngle;
    static bool_t startAngleIsNull = true;
    if (startAngleIsNull)
    {
        startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);
        startAngleIsNull = false;
    }
    // 行きメソッドの開始地点から、復帰角度を逆算
    arrivalAngle = arrivalAngle + inertiaAmount;
    if (arrivalAngle > 179)
    {
        arrivalAngle = -181 + (abs(arrivalAngle) - 179);
    }

    /*
    現在地点と復帰地点の角度に応じて、条件式を変更するためのフラグを調整
    */
    static bool_t isNeedAngleBuffer = false;
    static bool_t conditionIsInvert = false;
    // 到着角度が正負のギリギリ(ex. 179°など)の場合、少し過ぎて負に行っても止まるよう、猶予のフラグを立てる
    if (-170 > arrivalAngle && !isNeedAngleBuffer)
    {
        isNeedAngleBuffer = true;
    }
    // スタート位置が既に到着位置より小さい（スタート位置：負 到着位置：正）場合、移動継続の条件式を変えるよう、継続条件変更のフラグを立てる
    if ((startAngle < arrivalAngle) && !conditionIsInvert)
    {
        conditionIsInvert = true;
        printf("conditionIsInvertがtrueになった");
    }

    /*
    復帰地点角度まで車輪を動かす
    */
    static bool_t arrive = false;
    if (!arrive)
    {
        printf("arrivalAngle：%d", arrivalAngle);
        printf("angle：%d", angle);
        printf("conditionIsInvert：%d", conditionIsInvert);
        // 到着角度になるまで車輪を動かす
        if (!conditionIsInvert && arrivalAngle < angle)
        {
            printf("【楕円での撮影タスク：帰り】移動中・・・\n");
            // -170~-180のものは、正になった時点で動作を止める
            if (isNeedAngleBuffer && 0 < angle)
            {
                printf("バッファで止める");
                arrive = true;
            }
            ev3_motor_set_power(left_motor, -60);
        }
        // 到着角度になるまで車輪を動かす(正→負に動かす場合の条件式)
        else if (conditionIsInvert && (angle < 0 || arrivalAngle < angle))
        {
            printf("【楕円での撮影タスク：帰り】移動中・・・\n");
            ev3_motor_set_power(left_motor, -60);
        }
        // 撮影地点に到達した場合
        else
        {
            printf("【楕円での撮影タスク：帰り】元のライン上に到着");
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
        startAngleIsNull = true;
        isNeedAngleBuffer = false;
        conditionIsInvert = false;
        arrive = false;

        // タスク終了フラグ更新
        doBackToStartPointAtEllipse = false;

        // タスク終了したのでライントレースを再開
        *pointer_motor_impals = false;

        printf("【楕円での撮影タスク：帰り】終了\n");
        return true;
    }
}

/* ミニフィグ攻略の実行メソッド */
bool_t takePhotoOfMinifig(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int moveAngle, int inertiaAmount)
{
    // ミニフィグ撮影完了フラグ
    static bool_t doneTask = false;

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
    static bool_t snaped = false;
    static int timeCount = 0;
    static bool_t executedPython = false;
    if (!snaped && !doTowardsCenterOfEllipse)
    {
        snaped = waitMSecond(is_motor_stop, &timeCount, 2);

        // 写真撮影のPythonの実行
        static int minifigSnapNumber = 0; // n回目のミニフィグ撮影 かを管理
        if (!executedPython)
        {
            printf("ミニフィグ撮影実行【%d回目】\n", minifigSnapNumber);
            executedPython = makeMinifigFile(minifigSnapNumber);
            minifigSnapNumber++;
        }
    }

    // 元の位置に戻す
    if (doBackToStartPointAtEllipse && !doTowardsCenterOfEllipse && snaped)
    {
        doneTask = backToStartPointAtEllipse(startAngle, pointer_motor_impals, is_motor_stop, inertiaAmount);
    }

    if (doneTask)
    {
        // 次回のミニフィグ撮影用に、このタスクのフラグをすべてリセット
        doTowardsCenterOfEllipse = true;
        doBackToStartPointAtEllipse = true;
        snaped = false;
        timeCount = 0;
        executedPython = false;

        doneTask = false;
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
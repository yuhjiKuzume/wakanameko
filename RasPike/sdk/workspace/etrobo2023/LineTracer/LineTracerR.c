#include "app.h"
#include "LineTracerR.h"
#include "PidControl.h"
#include "ColorControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

// static int time; // 　制御時間
// static int trace_edge = RIGHT_EDGE;
// // static int trace_edge = LEFT_EDGE;
// static int dynamic_base_speed = 60;
// static int target_color = 170;
// static int latest_passed_blue_line_time = 0;

// // 超えてから計測開始時刻
// static int timeAtLapPoint = 0;

// static int16_t angle;

// /* ライントレースタスク(10msec周期で関数コールされる) */
// void tracer_task_R(intptr_t unused)
void tracer_task_R(intptr_t unused) {}
// {
//     static int selected_pid_parameter = 0; // 0なら直線用pid　1なら楕円用pid(ベースカラーは270にする)
//     static int blue_line_count = 0;        // 青ライン通過回数

//     // ポイント通過フラグ
//     static bool_t is_passing_through = false; // LAP通過をしたか
//     // static bool_t is_passing_through = true; // テスト用

//     // 角度が170度超えたかフラグ
//     static bool_t angle170_through = false;
//     static bool_t is_entering_double_loop = false;
//     static bool_t is_entering_mini_circle = false;

//     // モーター停止フラグ
//     static bool_t is_motor_stop = false;
//     static bool_t motor_impals = false;
//     static bool_t end_of_linetrace = false;

//     // 時間を計測
//     time++;

//     // 青→黒のライン変化取得
//     if (is_change_line_color_hsv())
//     {
//         blue_line_count++;
//         latest_passed_blue_line_time = time;
//     }
//     printf("blue line cont:%d\n", blue_line_count);
//     printf("time:%d\n", time / 100);

//     // カラー情報を取得
//     set_colorsensor_value();
//     convert_rgb_to_hsv();
//     // ジャイロセンサーから角度を取得
//     angle = ev3_gyro_sensor_get_angle(gyro_sensor);
//     printf("angle:%d\n", angle);

//     // // 一回も170度を超えてない状態で、170度に到達した場合
//     if ((abs(angle) > 170) && !angle170_through)
//     {
//         timeAtLapPoint = time;
//         angle170_through = true;
//         dynamic_base_speed = 45;
//     }

//     // LAP付近にきたら、右側トレースに切り替える
//     // コーナーを曲がった後（=170度に到達）、100秒後にエッジを切り替える
//     static bool_t passLap = false;
//     if ((time - timeAtLapPoint) > 100 && !is_passing_through && angle170_through)
//     {

//         printf("---------------------------------\n");
//         printf("Lap手前に到達\n");

//         dynamic_base_speed = 45;

//         target_color = 270;
//         blue_line_count = 0; // Lap過ぎたら、blue_line_countを初期化

//         initialize_pid_value();

//         is_passing_through = true;
//         trace_edge = LEFT_EDGE;
//         ev3_gyro_sensor_reset(gyro_sensor);
//         angle = ev3_gyro_sensor_get_angle(gyro_sensor);
//         passLap = true;
//     }
//     /*
//         ダブルループに進入
//             ・ライントレースの目標値変更
//     */
//     static int startDoubleLoop = 0;
//     if (blue_line_count == 1 && angle < -30 && !is_entering_double_loop)
//     {
//         printf("----------------------------------ダブルループ開始----------------------------------\n");
//         startDoubleLoop = time;
//         printf("startDoubleLoop:%d", startDoubleLoop);
//         target_color = 180;
//         is_entering_double_loop = true;
//         initialize_pid_value();
//         blue_line_count = 0;
//     }

//     /*
//      プラレール・風景攻略
//     */

//     /*
//     ※-10~0は設定しないでください(不具合が出ます)
//     */
//     static int16_t takeVideoAngle = 60; // ここを変更
//     static bool_t passPerfectCercle = false;

//     if (is_entering_double_loop && !passPerfectCercle)
//     {
//         static bool_t loopedOnce = false;
//         // 正円１周目
//         if (!loopedOnce)
//         {
//             static bool_t arriveTakeVideoPosition = false;
//             static bool_t doneTakeVideo = false;

//             // 撮影位置まで移動
//             if (!arriveTakeVideoPosition)
//             {
//                 if (takeVideoAngle > 0)
//                 {
//                     if (angle > takeVideoAngle)
//                     {
//                         arriveTakeVideoPosition = true;
//                         // 機体ストップ
//                         is_motor_stop = true;
//                         ev3_motor_set_power(left_motor, 0);
//                         ev3_motor_set_power(right_motor, 0);
//                     }
//                 }
//                 // 0°以上の場合は、ダブルループに入った時点でフラグが経ってしまう為、それを回避する為に設定
//                 else if (takeVideoAngle <= 0)
//                 {
//                     if (angle < -10 && angle > takeVideoAngle)
//                     {
//                         arriveTakeVideoPosition = true;
//                         // 機体ストップ
//                         is_motor_stop = true;
//                         ev3_motor_set_power(left_motor, 0);
//                         ev3_motor_set_power(right_motor, 0);
//                     }
//                 }
//             }
//             // 動画撮影
//             if (!doneTakeVideo && arriveTakeVideoPosition)
//             {
//                 // 90°曲げる、遠心力を考慮して14°手前で止める
//                 doneTakeVideo = takePhotoOfTrainAndLandscape(&motor_impals, &is_motor_stop, 90, 14); // ここを変更
//             }
//             if (doneTakeVideo && (time - startDoubleLoop) > 1600)
//             {
//                 printf("loopedOndeをtrue");
//                 loopedOnce = true;
//             }
//         }
//         // 正円２周目
//         if (loopedOnce)
//         {
//             // 楕円交差点部分に向けてエッジ切り替え
//             static bool_t changedEdge = false;
//             if (angle > 110 && !changedEdge)
//             {
//                 printf("【正円部】エッジ切り替え");
//                 trace_edge = LEFT_EDGE;
//                 initialize_pid_value();

//                 is_entering_mini_circle = true;
//                 blue_line_count = 0;
//                 changedEdge = true;
//             }
//             if (changedEdge && blue_line_count == 1)
//             {
//                 printf("----------------------------------【楕円突入】まっすぐGo----------------------------------");

//                 // 角度リセット
//                 static bool_t is_reset_angle = false;
//                 if (!is_reset_angle)
//                 {
//                     ev3_gyro_sensor_reset(gyro_sensor);
//                     angle = ev3_gyro_sensor_get_angle(gyro_sensor);
//                     is_reset_angle = true;
//                 }

//                 motor_impals = true;
//                 ev3_motor_set_power(left_motor, 20); // 60
//                 ev3_motor_set_power(right_motor, 65);
//                 if ((time - latest_passed_blue_line_time) > 60)
//                 {
//                     passPerfectCercle = true;

//                     motor_impals = false;
//                     initialize_pid_value();
//                     dynamic_base_speed = 45;
//                     selected_pid_parameter = 1;
//                     // target_color = 270;
//                 }
//             }
//         }
//     }

//     /*
//      ミニフィグ撮影
//     */
//     // 撮影回数のフェーズを管理するフラグ
//     static bool_t doFirstTask = true;
//     static bool_t doneFirstTask = false;

//     static bool_t doSecondTask = false;
//     static bool_t doneSecondTask = false;

//     static bool_t doThirdTask = false;
//     static bool_t doneThirdTask = false;

//     static bool_t doFourthTask = false;
//     static bool_t doneFourthTask = false;

//     static bool_t doFifthTask = false;
//     static bool_t doneFifthTask = false;

//     static bool_t doSixthTask = false;
//     static bool_t doneSixthTask = false;
//     //----------------------1回目：50°---------------------------------------------//
//     if (doFirstTask && passPerfectCercle)
//     {
//         // 撮影位置まで移動
//         static bool_t arrive = false;
//         if (!arrive && angle < -50) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
//         {
//             // 安定性確保のため機体ストップ
//             is_motor_stop = true;
//             ev3_motor_set_power(left_motor, 0);
//             ev3_motor_set_power(right_motor, 0);
//             arrive = true;
//         }

//         // 撮影位置に到着すると撮影タスク開始
//         if (arrive && !doneFirstTask)
//         {
//             printf("【楕円でのミニフィグ撮影】１回目");
//             doneFirstTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 70, 12); // ここを変更
//             if (doneFirstTask)
//             {
//                 doSecondTask = true;
//             }
//         }
//     }

//     //----------------------2回目：-90°---------------------------------------------//
//     if (doSecondTask && passPerfectCercle)
//     {
//         // 撮影位置まで移動
//         static bool_t arrive = false;
//         if (!arrive && angle < -80) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
//         {
//             // 安定性確保のため機体ストップ
//             is_motor_stop = true;
//             ev3_motor_set_power(left_motor, 0);
//             ev3_motor_set_power(right_motor, 0);
//             arrive = true;
//         }

//         // 撮影位置に到着すると撮影タスク開始
//         if (arrive && !doneSecondTask)
//         {
//             printf("【楕円でのミニフィグ撮影】２回目");
//             doneSecondTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 90, 12); // ここを変更
//             if (doneSecondTask)
//             {
//                 doThirdTask = true;
//             }
//         }
//     }
//     //----------------------3回目：-110°---------------------------------------------//
//     if (doThirdTask && passPerfectCercle)
//     {
//         // 撮影位置まで移動
//         static bool_t arrive = false;
//         if (!arrive && angle < -110) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
//         {
//             // 安定性確保のため機体ストップ
//             is_motor_stop = true;
//             ev3_motor_set_power(left_motor, 0);
//             ev3_motor_set_power(right_motor, 0);
//             arrive = true;
//         }

//         // 撮影位置に到着すると撮影タスク開始
//         if (arrive && !doneThirdTask)
//         {
//             printf("【楕円でのミニフィグ撮影】３回目");
//             doneThirdTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 120, 12); // ここを変更
//             if (doneThirdTask)
//             {
//                 doFourthTask = true;
//             }
//         }
//     }

//     //----------------------4回目：-130°---------------------------------------------//
//     if (doFourthTask && passPerfectCercle)
//     {
//         // 撮影位置まで移動
//         static bool_t arrive = false;
//         if (!arrive && angle < -130) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
//         {
//             // 安定性確保のため機体ストップ
//             is_motor_stop = true;
//             ev3_motor_set_power(left_motor, 0);
//             ev3_motor_set_power(right_motor, 0);
//             arrive = true;
//         }

//         // 撮影位置に到着すると撮影タスク開始
//         if (arrive && !doneFourthTask)
//         {
//             printf("【楕円でのミニフィグ撮影】４回目");
//             doneFourthTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 140, 12); // ここを変更
//             if (doneFourthTask)
//             {
//                 doFifthTask = true;
//             }
//         }
//     }

//     //----------------------5回目：130°---------------------------------------------//
//     if (doFifthTask && passPerfectCercle)
//     {
//         // 撮影位置まで移動
//         static bool_t arrive = false;
//         if (!arrive && angle < 130 && angle > 0) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
//         {
//             // 安定性確保のため機体ストップ
//             is_motor_stop = true;
//             ev3_motor_set_power(left_motor, 0);
//             ev3_motor_set_power(right_motor, 0);
//             arrive = true;
//         }

//         // 撮影位置に到着すると撮影タスク開始
//         if (arrive && !doneFifthTask)
//         {
//             printf("【楕円でのミニフィグ撮影】５回目");
//             doneFifthTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 60, 12); // ここを変更
//             if (doneFifthTask)
//             {
//                 doSixthTask = true;
//             }
//         }
//     }

//     //----------------------6回目：90°---------------------------------------------//
//     // static bool_t escapeEllipse = false;
//     static bool_t headToGoal = false;
//     static int sixthSnapfinishedTime = 0;
//     if (doSixthTask && passPerfectCercle)
//     {
//         // 撮影位置まで移動
//         static bool_t arrive = false;
//         if (!arrive && angle < 90 && angle > 0) // 110°などの場合、angle < 110 && angle > 0 // ここを変更
//         {
//             // 安定性確保のため機体ストップ
//             is_motor_stop = true;
//             ev3_motor_set_power(left_motor, 0);
//             ev3_motor_set_power(right_motor, 0);
//             arrive = true;
//         }

//         // 撮影位置に到着すると撮影タスク開始
//         if (arrive && !doneSixthTask)
//         {
//             printf("【楕円でのミニフィグ撮影】６回目");
//             doneSixthTask = takePhotoOfMinifig(&motor_impals, &is_motor_stop, 90, 12); // ここを変更
//             if (doneSixthTask)
//             {
//                 blue_line_count = 0;
//                 // escapeEllipse = true;
//                 sixthSnapfinishedTime = time;
//                 headToGoal = true;
//             }
//         }
//     }

//     /*
//     楕円を1.5周した後、直線と楕円の接点で直線に復帰し、ゴールを目指す
//     */
//     if (headToGoal)
//     {
//         // ミニフィグの撮影から3.5秒後にエッジを切り替える
//         if (time - sixthSnapfinishedTime > 3500)
//         {
//             trace_edge = RIGHT_EDGE;
//             selected_pid_parameter = 0;
//             initialize_pid_value();
//         }
//         // ゴールした後に止まるコードを入れる
//     }

//     /*
//         楕円を脱出し、正円に再侵入
//     */
//     // static bool_t passEllipse = false;
//     // if (escapeEllipse && !passEllipse && blue_line_count == 1)
//     // {
//     //     printf("----------------------------------【楕円脱出】まっすぐGo--------------------------------------------------------------------");
//     //     is_motor_stop = true;
//     //     motor_impals = true;
//     //     ev3_motor_set_power(left_motor, 50);
//     //     ev3_motor_set_power(right_motor, 50);
//     //     if ((time - latest_passed_blue_line_time) > 40)
//     //     { // 90
//     //         selected_pid_parameter = 0;
//     //         motor_impals = false;
//     //         initialize_pid_value();
//     //         passEllipse = true;
//     //         blue_line_count = 0;
//     //     }
//     // }

//     /*
//     ダブルループを脱出し、スマートキャリー攻略開始地点（青の〇）で止まる
//     */
//     // if (passEllipse && blue_line_count == 1)
//     // {
//     //     is_motor_stop = true;
//     //     ev3_motor_set_power(left_motor, 0);
//     //     ev3_motor_set_power(right_motor, 0);
//     // }

//     /* ステアリング操舵量の計算 */
//     if (!is_motor_stop && !motor_impals && !end_of_linetrace)
//     {
//         // ここの引数5つ目を、0なら外に強い　1なら内に強い(ベースカラーは270にする)
//         pid_driver(get_colorsensor_value(), target_color, trace_edge, dynamic_base_speed, selected_pid_parameter);
//     }

//     /* タスク終了 */
//     ext_tsk();
// }

// start_video(char *filename)
// {
//     FILE *file;
//     file = fopen(filename, "w");
//     fclose(file);
// }

// //------------------------プラレール・風景攻略用メソッド------------------------//

// // メソッド全体のフラグ
// static bool_t doTowardsCenterOfPerfectCircle = true;
// static bool_t doBackToStartPointAtPerfectCircle = true;

// static bool_t positionValueIsNull_towardsCenterOfPerfectCircle = true;
// static bool_t isNeedAngleBuffer_towardsCenterOfPerfectCircle = false;
// static bool_t doneTask_towardsCenterOfPerfectCircle = false;
// static bool_t positionValueIsNull_backToStartPointAtPerfectCircle = true;
// static bool_t isNeedAngleBuffer_backToStartPointAtPerfectCircle = false;
// static bool_t doneTask_backToStartPointAtPerfectCircle = false;
// static bool_t plarail_passShotTask = false;
// // 境界を乗り越える場合か
// static bool_t overcome_boundaries_towardsCenterOfPerfectCircle = false;
// static bool_t overcome_boundaries_backToStartPointAtPerfectCircle = false;

// /* 正円にて、走行体を正円の中央に向ける */
// int16_t towardsCenterOfPerfectCircle(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t moveAngle, int16_t inertiaAmount)
// {
//     printf("【正円での撮影タスク：行き】開始\n");
//     // このタスク終了までライントレースを切る
//     *pointer_motor_impals = true;

//     // 初期アングルから、到着角度を計算
//     static int16_t startAngle;
//     static int16_t arrivalAngle;

//     if (positionValueIsNull_towardsCenterOfPerfectCircle)
//     {
//         startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);

//         arrivalAngle = startAngle - (moveAngle - inertiaAmount);

//         if (arrivalAngle < -179)
//         {
//             arrivalAngle = 181 - (abs(arrivalAngle) - 179);
//         }
//         positionValueIsNull_towardsCenterOfPerfectCircle = false;
//         printf("startAngle : %d\n", startAngle);
//         printf("arrivalAngle : %d\n", arrivalAngle);
//     }

//     // 正負の間際の場合角度に猶予をつける
//     if (-170 > arrivalAngle && !isNeedAngleBuffer_towardsCenterOfPerfectCircle)
//     {
//         isNeedAngleBuffer_towardsCenterOfPerfectCircle = true;
//     }

//     // 境界を乗り越える場合 フラグをtrue
//     if ((startAngle < 0 && arrivalAngle > 0) && !overcome_boundaries_towardsCenterOfPerfectCircle)
//     {
//         overcome_boundaries_towardsCenterOfPerfectCircle = true;
//     }

//     // 到着角度になるまで車輪を動かす

//     printf("【正円での撮影タスク：行き】撮影位置への移動開始\n");
//     if ((angle > arrivalAngle) && !doneTask_towardsCenterOfPerfectCircle && !overcome_boundaries_towardsCenterOfPerfectCircle)
//     {
//         printf("【正円での撮影タスク：行き】移動中\n");
//         // -170~-179のものは、正になった時点で動作を止める
//         if (isNeedAngleBuffer_towardsCenterOfPerfectCircle && 0 < angle)
//         {
//             printf("バッファで止める");
//             doneTask_towardsCenterOfPerfectCircle = true;
//         }
//         ev3_motor_set_power(left_motor, -60);
//     }
//     else if (overcome_boundaries_towardsCenterOfPerfectCircle && (angle < 0 || angle > arrivalAngle) && !doneTask_towardsCenterOfPerfectCircle)
//     {
//         printf("【正円での撮影タスク：行き】移動中 【乗り越えver】\n");
//         ev3_motor_set_power(left_motor, -60);
//     }
//     else
//     {
//         printf("正常系で止める");
//         doneTask_towardsCenterOfPerfectCircle = true;
//     }

//     if (doneTask_towardsCenterOfPerfectCircle)
//     {
//         // 動作停止
//         is_motor_stop = true;
//         ev3_motor_set_power(left_motor, 0);
//         ev3_motor_set_power(right_motor, 0);

//         // タスク終了フラグ更新
//         doTowardsCenterOfPerfectCircle = false;
//         doneTask_towardsCenterOfPerfectCircle = false;
//         // タスク終了したのでライントレースを再開
//         *pointer_motor_impals = false;
//         printf("【正円での撮影タスク：行き】終了\n");
//         return startAngle;
//     }
// }

// /* 正円にて、撮影位置からライン上に復帰する */
// bool_t backToStartPointAtPerfectCircle(int16_t startAngle, bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t inertiaAmount)
// {
//     printf("【正円での撮影タスク：帰り】開始\n");
//     // このタスク終了までライントレースを切る
//     *pointer_motor_impals = true;

//     // 帰り用の到着角度を計算
//     static int16_t startAngle_backToStartPointAtPerfectCircle;
//     static int16_t arrivalAngle_backToStartPointAtPerfectCircle;

//     if (positionValueIsNull_backToStartPointAtPerfectCircle)
//     {
//         startAngle_backToStartPointAtPerfectCircle = ev3_gyro_sensor_get_angle(gyro_sensor);

//         arrivalAngle_backToStartPointAtPerfectCircle = startAngle - inertiaAmount;

//         if (arrivalAngle_backToStartPointAtPerfectCircle < -179)
//         {
//             arrivalAngle_backToStartPointAtPerfectCircle = 181 - (abs(arrivalAngle_backToStartPointAtPerfectCircle) - 179);
//         }
//         positionValueIsNull_backToStartPointAtPerfectCircle = false;
//         printf("startAngle_backToStartPointAtPerfectCircle : %d\n", startAngle_backToStartPointAtPerfectCircle);
//         printf("arrivalAngle_backToStartPointAtPerfectCircle : %d\n", arrivalAngle_backToStartPointAtPerfectCircle);
//     }

//     if (170 < arrivalAngle_backToStartPointAtPerfectCircle && !isNeedAngleBuffer_backToStartPointAtPerfectCircle)
//     {
//         isNeedAngleBuffer_backToStartPointAtPerfectCircle = true;
//     }

//     // 境界を乗り越える場合 フラグをtrue
//     if ((startAngle_backToStartPointAtPerfectCircle > 0 && arrivalAngle_backToStartPointAtPerfectCircle < 0) && !overcome_boundaries_backToStartPointAtPerfectCircle)
//     {
//         overcome_boundaries_backToStartPointAtPerfectCircle = true;
//     }

//     // 復帰地点まで動かす

//     printf("【正円での撮影タスク：帰り】撮影位置への移動開始\n");
//     if (arrivalAngle_backToStartPointAtPerfectCircle > angle && !doneTask_backToStartPointAtPerfectCircle && !overcome_boundaries_backToStartPointAtPerfectCircle)
//     {
//         printf("【正円での撮影タスク：帰り】移動中\n");
//         // 170~179のものは、負になった時点で動作を止める
//         if (isNeedAngleBuffer_backToStartPointAtPerfectCircle && 0 > angle)
//         {
//             printf("バッファで止める");
//             doneTask_backToStartPointAtPerfectCircle = true;
//         }
//         ev3_motor_set_power(left_motor, 60);
//     }
//     else if (overcome_boundaries_backToStartPointAtPerfectCircle && (angle > 0 || arrivalAngle_backToStartPointAtPerfectCircle > angle) && !doneTask_backToStartPointAtPerfectCircle)
//     {
//         printf("【正円での撮影タスク：帰り】移動中 【乗り越えver】\n");
//         ev3_motor_set_power(left_motor, 60);
//     }
//     else
//     {
//         printf("正常系で止める");
//         doneTask_backToStartPointAtPerfectCircle = true;
//     }

//     if (doneTask_backToStartPointAtPerfectCircle)
//     {
//         // 動作停止
//         is_motor_stop = true;
//         ev3_motor_set_power(left_motor, 0);
//         ev3_motor_set_power(right_motor, 0);

//         // タスク終了フラグ更新
//         doBackToStartPointAtPerfectCircle = false;
//         doneTask_backToStartPointAtPerfectCircle = false;

//         // pidパラメータリセット
//         initialize_pid_value();

//         // タスク終了したのでライントレースを再開
//         *pointer_motor_impals = false;
//         printf("【正円での撮影タスク：帰り】終了\n");
//         return true;
//     }
// }

// /* プラレール・風景攻略の実行メソッド */
// bool_t takePhotoOfTrainAndLandscape(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int moveAngle, int inertiaAmount)
// {
//     static bool_t doneTask = false;

//     // 動作を停止させる
//     ev3_motor_set_power(left_motor, 0);
//     ev3_motor_set_power(right_motor, 0);

//     // 走行体を撮影方向に向ける
//     static int16_t startAngle;

//     if (doTowardsCenterOfPerfectCircle)
//     {
//         startAngle = towardsCenterOfPerfectCircle(pointer_motor_impals, is_motor_stop, moveAngle, inertiaAmount);
//     }

//     // 写真を撮って、パーフェクトショットか判定
//     if (!plarail_passShotTask && !doTowardsCenterOfPerfectCircle)
//     {
//         static int timeCount = 0;
//         plarail_passShotTask = waitMSecond(is_motor_stop, &timeCount, 3);

//         // 動画撮影のPythonの実行
//         static bool_t executedPython = false;
//         if (!executedPython)
//         {
//             char fileName[] = "etrobo2023/flagFolder/movie_flag.txt";
//             FILE *file = fopen(fileName, "w");
//             if (!(file == NULL))
//             {
//                 printf("%sの作成に成功", fileName);
//                 executedPython = true;
//             }
//         }
//     }

//     // 元の位置に戻す
//     if (doBackToStartPointAtPerfectCircle && !doTowardsCenterOfPerfectCircle && plarail_passShotTask)
//     {
//         doneTask = backToStartPointAtPerfectCircle(startAngle, pointer_motor_impals, is_motor_stop, inertiaAmount);
//     }

//     if (doneTask)
//     {
//         pointer_motor_impals = false;
//         is_motor_stop = false;
//         return true;
//     }
// }

// //----------------------------------------------------------------------------//

// //------------------------ミニフィグ攻略用メソッド------------------------//

// // メソッド全体のフラグ
// static bool_t doTowardsCenterOfEllipse = true;
// static bool_t doBackToStartPointAtEllipse = true;

// static bool_t positionValueIsNull_towardsCenterOfEllipse = true;
// static bool_t isNeedAngleBuffer_towardsCenterOfEllipse = false;
// static bool_t doneTask_towardsCenterOfEllipse = false;
// static bool_t positionValueIsNull_backToStartPointAtEllipse = true;
// static bool_t isNeedAngleBuffer_backToStartPointAtEllipse = false;
// static bool_t doneTask_backToStartPointAtEllipse = false;
// static bool_t minifig_passShotTask = false;
// // 境界を乗り越える場合か
// static bool_t overcome_boundaries_towardsCenterOfEllipse = false;
// static bool_t overcome_boundaries_backToStartPointAtEllipse = false;

// /* 楕円にて、走行体を楕円の中央に向ける */
// int16_t towardsCenterOfEllipse(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t moveAngle, int16_t inertiaAmount)
// {
//     printf("【楕円での撮影タスク：行き】開始\n");
//     // このタスク終了までライントレースを切る
//     *pointer_motor_impals = true;

//     // 初期アングルから、到着角度を計算
//     static int16_t startAngle;
//     static int16_t arrivalAngle;

//     if (positionValueIsNull_towardsCenterOfEllipse)
//     {
//         startAngle = ev3_gyro_sensor_get_angle(gyro_sensor);

//         arrivalAngle = startAngle + (moveAngle - inertiaAmount);

//         if (arrivalAngle > 180)
//         {
//             arrivalAngle = -181 + (abs(arrivalAngle) - 179);
//         }
//         positionValueIsNull_towardsCenterOfEllipse = false;
//         printf("startAngle : %d\n", startAngle);
//         printf("arrivalAngle : %d\n", arrivalAngle);
//     }

//     // 正負の間際の場合角度に猶予をつける
//     if (170 < arrivalAngle && !isNeedAngleBuffer_towardsCenterOfEllipse)
//     {
//         isNeedAngleBuffer_towardsCenterOfEllipse = true;
//     }

//     // 境界を乗り越える場合 フラグをtrue
//     if ((startAngle > 0 && arrivalAngle < 0) && !overcome_boundaries_towardsCenterOfEllipse)
//     {
//         overcome_boundaries_towardsCenterOfEllipse = true;
//     }

//     // 到着角度になるまで車輪を動かす
//     printf("【楕円での撮影タスク：行き】撮影位置への移動開始\n");
//     if ((angle < arrivalAngle) && !doneTask_towardsCenterOfEllipse && !overcome_boundaries_towardsCenterOfEllipse)
//     {
//         printf("【楕円での撮影タスク：行き】移動中\n");
//         // 170~180のものは、負になった時点で動作を止める
//         if (isNeedAngleBuffer_towardsCenterOfEllipse && 0 > angle)
//         {
//             printf("バッファで止める");
//             doneTask_towardsCenterOfEllipse = true;
//         }
//         ev3_motor_set_power(left_motor, 60);
//     }
//     else if (overcome_boundaries_towardsCenterOfEllipse && (angle > 0 || angle < arrivalAngle) && !doneTask_towardsCenterOfEllipse)
//     {
//         printf("【楕円での撮影タスク：行き】移動中 【乗り越えver】\n");
//         ev3_motor_set_power(left_motor, 60);
//     }
//     else
//     {
//         printf("正常系で止める");
//         doneTask_towardsCenterOfEllipse = true;
//     }

//     if (doneTask_towardsCenterOfEllipse)
//     {
//         // 動作停止
//         is_motor_stop = true;
//         ev3_motor_set_power(left_motor, 0);
//         ev3_motor_set_power(right_motor, 0);

//         // タスク終了フラグ更新
//         doTowardsCenterOfEllipse = false;
//         doneTask_towardsCenterOfEllipse = false;
//         // タスク終了したのでライントレースを再開
//         *pointer_motor_impals = false;
//         printf("【楕円での撮影タスク：行き】終了\n");
//         return startAngle;
//     }
// }

// /* 楕円にて、撮影位置からライン上に復帰する */
// bool_t backToStartPointAtEllipse(int16_t startAngle, bool_t *pointer_motor_impals, bool_t *is_motor_stop, int16_t inertiaAmount)
// {
//     printf("【楕円での撮影タスク：帰り】開始\n\n");
//     // このタスク終了までライントレースを切る
//     *pointer_motor_impals = true;

//     // 帰り用の到着角度を計算
//     static int16_t startAngle_backToStartPointAtEllipse;
//     static int16_t arrivalAngle_backToStartPointAtEllipse;

//     if (positionValueIsNull_backToStartPointAtEllipse)
//     {
//         startAngle_backToStartPointAtEllipse = ev3_gyro_sensor_get_angle(gyro_sensor);

//         arrivalAngle_backToStartPointAtEllipse = startAngle + inertiaAmount;

//         if (arrivalAngle_backToStartPointAtEllipse > 180)
//         {
//             arrivalAngle_backToStartPointAtEllipse = -181 + (abs(arrivalAngle_backToStartPointAtEllipse) - 179);
//         }
//         positionValueIsNull_backToStartPointAtEllipse = false;
//         printf("startAngle_backToStartPointAtEllipse : %d\n", startAngle_backToStartPointAtEllipse);
//         printf("arrivalAngle_backToStartPointAtEllipse : %d\n", arrivalAngle_backToStartPointAtEllipse);
//     }

//     if (-170 > arrivalAngle_backToStartPointAtEllipse && !isNeedAngleBuffer_backToStartPointAtEllipse)
//     {
//         isNeedAngleBuffer_backToStartPointAtEllipse = true;
//     }

//     // 境界を乗り越える場合 フラグをtrue
//     if ((startAngle_backToStartPointAtEllipse < 0 && arrivalAngle_backToStartPointAtEllipse > 0) && !overcome_boundaries_backToStartPointAtEllipse)
//     {
//         overcome_boundaries_backToStartPointAtEllipse = true;
//     }

//     // 復帰地点まで動かす
//     printf("【楕円での撮影タスク：帰り】撮影位置への移動開始\n");
//     if (arrivalAngle_backToStartPointAtEllipse < angle && !doneTask_backToStartPointAtEllipse && !overcome_boundaries_backToStartPointAtEllipse)
//     {
//         printf("【楕円での撮影タスク：帰り】移動中\n");
//         // -170~-179のものは、負になった時点で動作を止める
//         if (isNeedAngleBuffer_backToStartPointAtEllipse && 0 < angle)
//         {
//             printf("バッファで止める");
//             doneTask_backToStartPointAtEllipse = true;
//         }
//         ev3_motor_set_power(left_motor, -60);
//     }
//     else if (overcome_boundaries_backToStartPointAtEllipse && (angle < 0 || arrivalAngle_backToStartPointAtEllipse < angle) && !doneTask_backToStartPointAtEllipse)
//     {
//         printf("【楕円での撮影タスク：帰り】移動中 【乗り越えver】\n");
//         ev3_motor_set_power(left_motor, -60);
//     }
//     else
//     {
//         printf("正常系で止める");
//         doneTask_backToStartPointAtEllipse = true;
//     }

//     if (doneTask_backToStartPointAtEllipse)
//     {
//         // 動作停止
//         is_motor_stop = true;
//         ev3_motor_set_power(left_motor, 0);
//         ev3_motor_set_power(right_motor, 0);

//         // タスク終了フラグ更新
//         doBackToStartPointAtEllipse = false;
//         doneTask_backToStartPointAtEllipse = false;

//         // pidパラメータリセット
//         initialize_pid_value();

//         // タスク終了したのでライントレースを再開
//         *pointer_motor_impals = false;

//         printf("【楕円での撮影タスク：帰り】終了\n");
//         return true;
//     }
// }

// /* ミニフィグ攻略の実行メソッド */
// bool_t takePhotoOfMinifig(bool_t *pointer_motor_impals, bool_t *is_motor_stop, int moveAngle, int inertiaAmount)
// {

//     static bool_t doneTask = false;

//     // 動作を停止させる
//     ev3_motor_set_power(left_motor, 0);
//     ev3_motor_set_power(right_motor, 0);

//     // 走行体を撮影方向に向ける
//     static int16_t startAngle;

//     if (doTowardsCenterOfEllipse)
//     {
//         startAngle = towardsCenterOfEllipse(pointer_motor_impals, is_motor_stop, moveAngle, inertiaAmount);
//     }

//     // 写真を撮って、パーフェクトショットか判定
//     static bool_t executedPython = false;
//     static int timeCount = 0;
//     if (!minifig_passShotTask && !doTowardsCenterOfEllipse)
//     {
//         minifig_passShotTask = waitMSecond(is_motor_stop, &timeCount, 5);
//         static int minifigSnapNumber = 0;
//         if (!executedPython)
//         {
//             printf("撮影実行\n");
//             executedPython = makeMinifigFile(minifigSnapNumber);
//             minifigSnapNumber++;
//         }
//     }

//     // 画像送信

//     // 元の位置に戻す
//     if (doBackToStartPointAtEllipse && !doTowardsCenterOfEllipse && minifig_passShotTask)
//     {
//         doneTask = backToStartPointAtEllipse(startAngle, pointer_motor_impals, is_motor_stop, inertiaAmount);
//     }

//     // このタスクのフラグをすべてリセット
//     if (doneTask)
//     {
//         pointer_motor_impals = false;
//         is_motor_stop = false;

//         doTowardsCenterOfEllipse = true;
//         doBackToStartPointAtEllipse = true;
//         positionValueIsNull_towardsCenterOfEllipse = true;
//         isNeedAngleBuffer_towardsCenterOfEllipse = false;
//         doneTask_towardsCenterOfEllipse = false;
//         positionValueIsNull_backToStartPointAtEllipse = true;
//         isNeedAngleBuffer_backToStartPointAtEllipse = false;
//         doneTask_backToStartPointAtEllipse = false;
//         minifig_passShotTask = false;
//         overcome_boundaries_towardsCenterOfEllipse = false;
//         overcome_boundaries_backToStartPointAtEllipse = false;
//         timeCount = 0;
//         executedPython = false;
//         return true;
//     }
// }

// //-----------------------------------------------------------------------------//

// /* 指定された秒数停止する */
// bool_t waitMSecond(bool_t *is_motor_stop, int *watingTime, int mSecond)
// {
//     (*watingTime)++;
//     if (*watingTime > (mSecond * 100))
//     {
//         *is_motor_stop = false;
//         printf("%d秒待機終了\n", mSecond);
//         return true;
//     }
//     else
//     {
//         printf("待機中・・・\n");
//         *is_motor_stop = true;
//         ev3_motor_set_power(left_motor, 0);
//         ev3_motor_set_power(right_motor, 0);
//         return false;
//     }
// }

// /* ファイルを作成する */
// bool_t makeMinifigFile(int fileNum)
// {
//     bool_t result = false;
//     char fileName[100];
//     sprintf(fileName, "etrobo2023/flagFolder/capture_flag_%d.txt", fileNum);

//     FILE *file = fopen(fileName, "w");
//     if (!(file == NULL))
//     {
//         printf("%sの作成に成功", fileName);
//         result = true;
//     }
//     return result;
// }

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
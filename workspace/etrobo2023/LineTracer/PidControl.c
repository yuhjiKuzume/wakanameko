#include "app.h"
#include "LineTracer.h"
#include "PidControl.h"
#include <stdio.h>
#include <stdlib.h>

// 高速での外側トレース
// #define KP 0.11
// #define KI 0.06
// #define KD 0.015

// #define KP 0.043
// #define KI 0.06
// #define KD 0.015
// #define KP 0.047
// #define KI 0.007
// #define KD 0.017

#define DELTA_T (LINE_TRACER_PERIOD / 1000.0 / 1000.0) // 処理周期

static float32_t integral = 0.0; // 偏差の積分
static float32_t diff_before;    // 1つ前の偏差
static float32_t diff_after;     // 現在の偏差

//
int32_t KP_SOTO = 0.047;
int32_t KI_SOTO = 0.007;
int32_t KD_SOTO = 0.017;

int32_t KP_UTI = 0.043;
int32_t KI_UTI = 0.06;
int32_t KD_UTI = 0.015;

// PIDの構造体
typedef struct
{
    float32_t KP, KI, KD;
} TARGET_PID;

// 各色のpid閾値テーブル定義
static TARGET_PID target_pid[] = {
    // {0.07, 0.024, 0.019}, // 直線用PID
    {0.07, 0.024, 0.019},  // 直線用PID
    {0.13, 0.035, 0.005}}; // 楕円用PID

// PID制御による操作量の計算
/*
    引数(selected_pid_parameter)：
        0：外に強いPID
        1：内に強いPID
*/
int16_t steering_amount_pid_calculation(rgb_raw_t rgb_val, int target_color, int selected_pid_parameter)
{

    float32_t p, i, d; // PIDの計算値

    // RGBの合計値を計算
    int32_t sumcolor = rgb_val.r + rgb_val.g + rgb_val.b;

    /* 目標輝度値とカラーセンサ値の差分を計算 */
    diff_before = diff_after;
    diff_after = (float32_t)(target_color - sumcolor);
    integral += (diff_after + diff_before) / 2.0 * DELTA_T;

    p = target_pid[selected_pid_parameter].KP * diff_after;
    i = target_pid[selected_pid_parameter].KI * integral;
    d = (target_pid[selected_pid_parameter].KD * (diff_after - diff_before)) / DELTA_T;

    // printf("\nPID,%d,%d,%d\n", p, i, d);

    return p + i + d;
}

void initialize_pid_value()
{
    integral = 0.0;
    diff_before = 0.0;
    diff_after = 0.0;

    return;
}

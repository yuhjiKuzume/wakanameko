#include "app.h"
#include "LineTracer.h"
#include "ColorControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// SHVの構造体
typedef struct
{
    float32_t h, s, v;
} HSV;

// 閾値の構造体
typedef struct
{
    char color_name[32];
    float32_t target_h, target_s, target_v;
    float32_t margin;
} TARGET_HSV;

static rgb_raw_t rgb_val; // カラーセンサー値(RGB)
static HSV hsv;           // HSV変換後の値
// 各色のSHV閾値テーブル定義
static TARGET_HSV target_hsv[] = {
    {"black", 205.0, 30.0, 15.0, 10.0},
    {"blue", 210.0, 83.0, 60.0, 20.0},
    {"red", 355.0, 80.0, 130.0, 15.0}};

/*
    カラーセンサーの値を取得
*/
void set_colorsensor_value()
{
    // カラーセンサー値を取得
    ev3_color_sensor_get_rgb_raw(color_sensor, &rgb_val);

    printf("RGB values print >> R:%d  G:%d  B:%d\n", rgb_val.r, rgb_val.g, rgb_val.b);
    printf("%d,%d,%d,%d\n", rgb_val.r, rgb_val.g, rgb_val.b, rgb_val.r + rgb_val.g + rgb_val.b);
}

/*
    カラーセンサーの値を返す
*/
rgb_raw_t get_colorsensor_value()
{
    return rgb_val;
}

/*
    RGBからSHVへ変換
*/
void convert_rgb_to_hsv()
{

    // RGBのうち、最大・最小を取得
    float32_t max = fmax(fmax(rgb_val.r, rgb_val.g), rgb_val.b);
    float32_t min = fmin(fmin(rgb_val.r, rgb_val.g), rgb_val.b);
    float32_t delta = max - min;

    // 色相(h)を求める
    if (delta == 0)
    {
        hsv.h = 0.0;
    }
    else if (max == rgb_val.r)
    {
        hsv.h = 60 * ((rgb_val.g - rgb_val.b) / delta);
    }
    else if (max == rgb_val.g)
    {
        hsv.h = 60 * ((rgb_val.b - rgb_val.r) / delta) + 120;
    }
    else
    {
        hsv.h = 60 * ((rgb_val.r - rgb_val.g) / delta) + 240;
    }

    if (hsv.h < 0)
    {
        hsv.h += 360;
    }

    // 彩度(s)を求める
    if (delta == 0)
    {
        hsv.s = 0.0;
    }
    else
    {
        hsv.s = (delta / max) * 100;
    }

    // 明度(v)を求める
    hsv.v = max;

    printf("HSV values print >> H:%f  S:%f  V:%f\n", hsv.h, hsv.s, hsv.v);
}

/*
    ラインの色判定(HSV)
    引数(target_color_number)：
        0：黒
        1：青
*/
bool_t check_Line_color_hsv(int16_t target_color_number)
{

    // 判定対象カラーの閾値を取得
    float32_t target_h = target_hsv[target_color_number].target_h;
    float32_t target_s = target_hsv[target_color_number].target_s;
    float32_t target_v = target_hsv[target_color_number].target_v;
    float32_t margin = target_hsv[target_color_number].margin;

    bool_t check_h_value = ((target_h - margin) <= hsv.h && hsv.h <= (target_h + margin));
    bool_t check_s_value = ((target_s - margin) <= hsv.s && hsv.s <= (target_s + margin));
    bool_t check_v_value = ((target_v - margin) <= hsv.v && hsv.v <= (target_v + margin));

    // printf("ライン色判定(HSV):%d:%d\n", target_color_number, check_h_value && check_s_value && check_v_value);

    return check_h_value && check_s_value && check_v_value;
}

/*
    青→黒のライン変化を判定(HSV)
*/
static int16_t previous_line_color = 0; // 　前回のラインカラー(0:黒、1:青)

int16_t is_change_line_color_hsv()
{

    // 現在の色を取得
    bool_t is_black = check_Line_color_hsv(0);
    bool_t is_blue = check_Line_color_hsv(1);
    // 青→黒のライン変化を判定
    if (previous_line_color == 1 && is_black)
    {
        previous_line_color = 0;
        return true;
    }

    if (is_black)
    {
        previous_line_color = 0;
    }
    else if (is_blue)
    {
        previous_line_color = 1;
    }

    return false;
}

void reset_previous_color()
{
    previous_line_color = 0;
}

/*
  青ライン判定関数
    戻り値：青の場合はtrue、青以外の場合はfalse
*/
bool_t is_bule_Line(rgb_raw_t rgb_val)
{
    bool_t check_r_value = 10 <= rgb_val.r && rgb_val.r <= 30;
    bool_t check_g_value = 30 <= rgb_val.g && rgb_val.g <= 50;
    bool_t check_b_value = 55 <= rgb_val.b && rgb_val.b <= 85;
    // printf("isbuleline:%d\n",check_r_value && check_r_value && check_b_value);

    // 試走会で新規追加
    // bool_t check_r_value = 5 <= rgb_val.r && rgb_val.r <= 18;
    // bool_t check_g_value = 25 <= rgb_val.g && rgb_val.g <= 40;
    // bool_t check_b_value = 50 <= rgb_val.b && rgb_val.b <= 70;

    return check_r_value && check_r_value && check_b_value;
}

/*
  黒ライン判定関数
    戻り値：黒の場合はtrue、黒以外の場合はfalse
*/
bool_t is_black_Line(rgb_raw_t rgb_val)
{
    bool_t check_r_value = (rgb_val.r <= 10);
    bool_t check_g_value = (rgb_val.g <= 10);
    bool_t check_b_value = (rgb_val.b <= 10);
    // printf("isblackline:%d\n",check_r_value && check_r_value && check_b_value);

    return check_r_value && check_r_value && check_b_value;
}

/*
  ラインの色の変化を検知する関数
　 戻り値：
        黒 → 青の場合   0
　      青 → 黒の場合   1
        同色の場合     -1
*/
int16_t is_change_line_color(rgb_raw_t rgb_val)
{

    static int16_t previous_line_color = 0; // 　前回のラインカラー(0:黒、1:青)

    bool_t is_blue = is_bule_Line(rgb_val);
    bool_t is_black = is_black_Line(rgb_val);

    if (previous_line_color == 0 && is_blue)
    {
        // 黒 → 青
        previous_line_color = 1;
        return 0;
    }
    else if (previous_line_color == 1 && is_black)
    {
        // 青 → 黒
        previous_line_color = 0;
        return 1;
    }
    else if (is_black)
    {
        previous_line_color = 0;
    }
    else if (is_blue)
    {
        previous_line_color = 1;
    }

    return -1;
}

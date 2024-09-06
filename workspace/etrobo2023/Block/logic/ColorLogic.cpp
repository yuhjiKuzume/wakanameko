#include "ColorLogic.h"
#include <stdlib.h>
#include <math.h>
#include "Sapporo.h"

ColorLogic::ColorLogic(ev3api::ColorSensor& colorSensor)
    : mColorSensor(colorSensor){}

int ColorLogic::checkColor(){
    // RGB値を取得
    mColorSensor.getRawColor(rgb_val);

    // HSV値を計算
    double h, s, v;
    ColorLogic::rgb_to_hsv(rgb_val.r, rgb_val.g, rgb_val.b, h, s, v);
    int result;

    // 色を判定
    if (s < 0.4 && v > 0.7) {
        result = WHITE;
    } else if (h >= 180 && h <= 250 && s >= 0.4 &&v >= 0.2) {
        result = BLUE;
    } else if (h >= 90 && h <= 155 && s >= 0.4 &&v >= 0.2) {
        result = GREEN;
    } else if (v < 0.2) {
        result = BLACK;
    } else if ((h >= 0 && h <= 30) || (h >= 330 && h <= 360) && s >= 0.5) {
        result = RED;
    } else if ((h >= 30 && h <= 80) && s >= 0.2) {
        result = YELLOW;
    } else {
        result = -1; //その他
    }
    // printf(" color is %d, rgb is %d,%d,%d, hsv = is %lf,%lf,%lf\n", result, rgb_val.r, rgb_val.g, rgb_val.b, h, s, v);
    return result;
}

void ColorLogic::color_sensor_value_log(){
    FILE *outputfile;
    outputfile = fopen("color_sensor_log.txt", "a");
    if (outputfile == NULL) {
        printf("cannot open\n");
    }

    int brightness = mColorSensor.getBrightness();
    fprintf(outputfile, "brightness is %d\n", brightness);
    printf("brightness is %d\n", brightness);

    // 環境光
    // int ambient = mColorSensor.getAmbient();
    // fprintf(outputfile, "ambient is %d\n", ambient);
    // printf("ambient is %d\n", ambient);

    mColorSensor.getRawColor(rgb_val);
    fprintf(outputfile, "rgb r is %d\n", rgb_val.r);
    fprintf(outputfile, "rgb g is %d\n", rgb_val.g);
    fprintf(outputfile, "rgb b is %d\n", rgb_val.b);
    printf("r is %d\n", rgb_val.r);
    printf("g is %d\n", rgb_val.g);
    printf("b is %d\n", rgb_val.b);

    // HSV値の計算と出力
    double h, s, v;
    rgb_to_hsv(rgb_val.r, rgb_val.g, rgb_val.b, h, s, v);
    fprintf(outputfile, "h is %f\n", h);
    fprintf(outputfile, "s is %f\n", s);
    fprintf(outputfile, "v is %f\n", v);
    printf("h is %f\n", h);
    printf("s is %f\n", s);
    printf("v is %f\n", v);

    fclose(outputfile);          // ファイルをクローズ(閉じる)
}

// RGB値をHSV値に変換する関数
void ColorLogic:: rgb_to_hsv(int r, int g, int b, double &h, double &s, double &v) {
    double min_val = fmin(r, fmin(g, b));
    double max_val = fmax(r, fmax(g, b));
    double delta = max_val - min_val;

    // 色相（Hue）の計算
    if (delta == 0) {
        h = 0; // グレースケールの場合、色相は0とする
    } else if (max_val == r) {
        h = fmod((g - b) / delta + 6, 6);
    } else if (max_val == g) {
        h = (b - r) / delta + 2;
    } else if (max_val == b) {
        h = (r - g) / delta + 4;
    }
    h *= 60;

    // 彩度（Saturation）の計算
    if (max_val == 0) {
        s = 0;
    } else {
        s = delta / max_val;
    }

    // 明度（Value）の計算
    v = max_val / 255;
}

int ColorLogic::colorGetBrightness(){
    return mColorSensor.getBrightness();
}

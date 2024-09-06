#include "ColorCheckService.h"
#include "Sapporo.h"

ColorCheckService::ColorCheckService(ev3api::ColorSensor& colorSensor)
     : mColorLogic(colorSensor){
}

int colorList[COLOR_CHECK_NUM];

// 色判定をする
void ColorCheckService::executeColorCheck(){

    // 連続同じ値を判定した場合セットする
    for(int i = COLOR_CHECK_NUM - 1; i>=0 ;i--){
        if(i == 0){
            colorList[0] = mColorLogic.checkColor();         
        }else{
            colorList[i] = colorList[i-1];
        }
    }
    int flag = 0;
    for(int i = 0; i < COLOR_CHECK_NUM - 1; i++) {
        if(colorList[i] == colorList[i + 1]) {
            flag++;
        } else {
            break;
        }
        if(flag == COLOR_CHECK_NUM - 1) {
            setColor(colorList[i]);
        }
    }

    // printf("color check service val is %d\n", getColor());
    // mColorLogic.color_sensor_value_log();
}

int ColorCheckService::getColor() {
    return this->color;
}
void ColorCheckService::setColor(int color) {
    this->color = color;
}

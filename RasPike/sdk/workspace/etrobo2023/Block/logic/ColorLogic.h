#ifndef COLORLOGIC_H_
#define COLORLOGIC_H_

#include "ColorSensor.h"

class ColorLogic {
public:
    ColorLogic(
        ev3api::ColorSensor& colorSensor
    );
    int checkColor();
    int colorGetBrightness();
    void color_sensor_value_log();
    void rgb_to_hsv(int r, int g, int b, double &h, double &s, double &v);

private:
    rgb_raw_t rgb_val;
    ev3api::ColorSensor& mColorSensor;
};
#endif /* COLORLOGIC_H_ */
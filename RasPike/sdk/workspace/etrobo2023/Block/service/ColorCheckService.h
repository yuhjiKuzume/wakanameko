#ifndef COLORCHECKSERVICE_H_
#define COLORCHECKSERVICE_H_

#include "ColorLogic.h"
#include "ColorSensor.h"

class ColorCheckService{
public:
    ColorCheckService(
        ev3api::ColorSensor& colorSensor
    );

    void executeColorCheck();
    int getColor();
    void setColor(int color);

private:
    int color = -1;
    ColorLogic mColorLogic;
};
#endif /* COLORCHECKSERVICE_H_ */
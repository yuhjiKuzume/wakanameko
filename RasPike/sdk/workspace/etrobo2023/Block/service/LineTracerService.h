#ifndef EV3_APP_LINETRACER_H_
#define EV3_APP_LINETRACER_H_

#include "Motor.h"
#include "ColorSensor.h"

/* ステアリング操舵量の係数 
#define STEERING_COEF     (0.25F) 
#define DER_COEF (7.5F)
#define SUM_COEF (2.5F)
#define dt (0.005F)

#define BASE_SPEED        (50) */
#define STEERING_COEF     (0.175F) 
#define DER_COEF (9.5F)
#define SUM_COEF (2.5F)
// #define STEERING_COEF     (0.25F) 
// #define DER_COEF (2.2F)
// #define SUM_COEF (2.5F)
#define dt (0.005F)

#define num (5)
/* ライントレースエッジ切り替え */
#define LEFT_EDGE         (-1) 
#define RIGHT_EDGE        (1) 

class LineTracerService {
public:
    LineTracerService(
        ev3api::Motor& leftWheel,
        ev3api::Motor& rightWheel,
        ev3api::ColorSensor& colorSensor
    );
    void run(int lineposition, int beas_spped);
    
private:
    ev3api::Motor& mLeftWheel;
    ev3api::Motor& mRightWheel;
    ev3api::ColorSensor& mColorSensor;
    double pid();
    void init();
    void cariblation();
    int16_t steering_amount_calculation();
};

#endif  // EV3_APP_LINETRACER_H_

/*
 * calibrator.h
 *
 *  Created on: 2011/09/27
 *      Author: tdohshi
 */

#ifndef CALIBRATOR_H_
#define CALIBRATOR_H_

//#include "traceSide.h"

#define CALIBRATOR_GYRO_OFFSET			(600)
#define CALIBRATOR_TYRE_RADIUS			(40.1)		/* タイヤ半径(mm) */
#define CALIBRATOR_AXLE_LENGTH			(55.0)		/* 65行き過ぎ/64不足:車軸長の半分(mm) */
#define CALIBRATOR_GEAR_RATIO			(1.0)		/* ギアレシオ(ギア比 1[1:1], 3[1:3], 5[1:5]) */
#define CALIBRATOR_TYRE_R_OFFSET		(0)			/* 右タイヤ速度オフセット（本番用）*/
#define CALIBRATOR_STEERING_ANGLE_MAX	(90)		/* ステアリング最大角 */
//#define CALIBRATOR_TRACE_SIDE			(TRACE_SIDE_RIGHT)	/* スタート時にトレースする側 */

typedef enum _CalibrateStat {
	CALIBRATE_STAT_WHITE = 0,
	CALIBRATE_STAT_BLACK,
	CALIBRATE_STAT_GRAY,
	CALIBRATE_STAT_END,
} CalibrateStat;

typedef enum _LightThresholdType {
	LIGHT_THRESHOLD_TYPE_DEFAULT,
	LIGHT_THRESHOLD_TYPE_HIGHSPEED,
	LIGHT_THRESHOLD_TYPE_NEAR_BLACK,
	LIGHT_THRESHOLD_TYPE_NEAR_WHITE,
	LIGHT_THRESHOLD_TYPE_MANUAL,
} LightThresholdType;

void calibrator_setLightThresholdType(LightThresholdType type);
LightThresholdType calibrator_getLightThresholdType(void);
void calibrator_setLightThresholdOfManual(int threshold);
float calibrator_getLightThreshold(void);
int calibrator_calibrateBrightness(void);

#endif /* CALIBRATOR_H_ */
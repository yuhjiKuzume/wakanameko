/*
 * localizer.h
 *
 *  Created on: 2011/07/14
 *      Author: tdohshi
 */

#ifndef LOCALIZER_H_
#define LOCALIZER_H_

#include "position.h"
#include "odometory.h"
#include "localizerTask.h"
//#include "lpf.h"
#include "Motor.h"

#define LOCALIZER_CYCLIC	TASK_CYCLE_LOCALIZER	/* 自己位置推定が呼ばれる周期 */

typedef struct _Localizer {
	Odometory odometory;
	Position localPos;
	double distance;
	double speed;
	double ud_speed;
	//LPF lpf;
} Localizer;

void localizer_init(const Position* startPos);		/* 初期化 */
void localizer_run(void);							/* 自己位置推定実行 */
const Position* localizer_getPosition(void);		/* 自己位置取得 */
void localizer_setPosition(const Position* pos);	/* 自己位置設定 */
double localizer_getDistance(void);					/* 移動距離取得 */
double localizer_getSpeed(void);					/* 速度取得 */
double localizer_getDirection(void);				/* 方位取得 */
void localizer_setDistance();
void localizer_setDirection();

#endif /* LOCALIZER_H_ */
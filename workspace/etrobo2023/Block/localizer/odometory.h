/*
 * odometory.h
 *
 *  Created on: 2011/07/09
 *      Author: tdohshi
 */

#ifndef ODOMETORY_H_
#define ODOMETORY_H_

#include "calibrator.h"
#include "position.h"

typedef struct _Odometory {
	/** 基準位置 */
	Position base;
	/** 基準位置での左モータ回転角 */
	int leftAngle;
	/** 基準位置での右モータ回転角 */
	int rightAngle;
	/** 移動距離 */
	double distance;
} Odometory;

void odometory_init(Odometory* self, const Position* base);
void odometory_setBasePosition(Odometory* self, const Position* basePos);
double odometory_getDistance(Odometory* self);
void odometory_getNextPosition(Odometory* self, int leftAngle, int rightAngle, Position* pos);

#endif /* ODOMETORY_H_ */
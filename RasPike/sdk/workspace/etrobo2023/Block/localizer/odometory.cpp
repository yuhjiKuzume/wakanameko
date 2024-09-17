/*
 * odometory.c
 *
 *  Created on: 2011/07/09
 *      Author: tdohshi
 */

#include <float.h>
#include <math.h>
#include "etrobo_inc.h"
#include "odometory.h"
#include "position.h"
#include "math.h"


/**
 * 初期化
 * @param Position 基準位置
 */
void odometory_init(Odometory* self, const Position* base) {
	self->base = *base;
	self->leftAngle = 0;
	self->rightAngle = 0;
	self->distance = 0;
}

/**
 * 基準位置を設定
 */
void odometory_setBasePosition(Odometory* self, const Position* basePos) {
	self->base = *basePos;
}

/**
 * 移動距離取得
 */
double odometory_getDistance(Odometory* self) {
	return self->distance;
}

/**
 * オドメトリ手法による計算
 * @param leftAngle	現在の左モータの回転角
 * @param rightAngle 現在の右モータの回転角
 * @return 現在位置
 */
void odometory_getNextPosition(Odometory* self, int leftAngle, int rightAngle, Position* pos) {
	/* 各タイヤの回転角度算出(単位：度) */
	/* 回転角度 = (現在の回転角度 - 前回の回転角度) * ギア比 */
	double theta_R = ((double)(rightAngle - self->rightAngle)) * CALIBRATOR_GEAR_RATIO;
	double theta_L = ((double)(leftAngle - self->leftAngle)) * CALIBRATOR_GEAR_RATIO;
	//printf("theta_R: %lf\n", theta_R);
	//printf("rightAngle: %d\n", rightAngle);

//	DBG_LOG("theta_R : %d, theta_L : %d", theta_R, theta_L);

	/* 各タイヤの進んだ距離を算出(単位:mm) */
	/* 距離 = 2 * PI * タイヤの半径 * (回転角度 / 360) */
	double distance_R = (2 * M_PI * CALIBRATOR_TYRE_RADIUS) * theta_R / 360;
	double distance_L = (2 * M_PI * CALIBRATOR_TYRE_RADIUS) * theta_L / 360;
	//printf("distance_R: %lf\n", distance_R);

//	DBG_LOG("distance_R : %d, distance_L : %d", (int)distance_R, (int)distance_L);

	/* 走行体の進んだ距離を算出(単位:mm) */
	/* 距離 = (右タイヤの進んだ距離 + 左タイヤの進んだ距離) / 2 */
	double distance = (distance_R + distance_L) / 2;

//	DBG_LOG("distance : %d\n", (int)distance);

	/* 走行体の旋回角度を算出(単位：radian) */
	/* 旋回角度 = (右タイヤの進んだ距離 - 左タイヤの進んだ距離) / (2 * タイヤ半径) */
	double theta = (distance_R - distance_L) / (2 * CALIBRATOR_AXLE_LENGTH);

//	DBG_LOG("theta : %d\n", (int)theta);

	/* 旋回半径算出 */
	double turn_r = 0;
	if (DBL_EPSILON < fabs(theta)) {
		turn_r = distance / theta;
	}

//	DBG_LOG("turn_r : %d\n", (int)turn_r);

	/* 正確な移動距離??を算出 */
	double distance2 = distance;
	if (DBL_EPSILON < fabs(turn_r)) {
		distance2 = 2 * turn_r * sin(theta / 2);
	}

//	DBG_LOG("distance2 : %d\n", (int)distance2);

	/* 現在位置算出 */
	pos->x = position_getX(&self->base) +
			distance2 * cos(TO_RADIANS(position_getW(&self->base)) + theta / 2);
	pos->y = position_getY(&self->base) +
			distance2 * sin(TO_RADIANS(position_getW(&self->base)) + theta / 2);
	pos->w = position_getW(&self->base) + TO_DEGREES(theta);

	/* 現在位置を基準に設定 */
	self->leftAngle = leftAngle;
	self->rightAngle = rightAngle;
	self->distance = distance2;
	self->base = *pos;
}
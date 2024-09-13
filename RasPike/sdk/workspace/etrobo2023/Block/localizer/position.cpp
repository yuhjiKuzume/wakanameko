/*
 * position.c
 *
 *  Created on: 2011/07/09
 *      Author: tdohshi
 */

#include "etrobo_inc.h"
#include "position.h"
#include "math.h"

/**
 * 引数付き初期化
 * @param x	位置x方向
 * @param y	位置y方向
 * @param w	向いている方向
 */
void position_init(Position* self, double x, double y, double w) {
	self->x = x;
	self->y = y;
	self->w = w;
}

/**
 * 2点間の距離を算出
 * @param this
 * @param target
 * @return
 */
double position_getDistance(const Position* self, const Position* target) {
	return hypot(self->x - target->x, self->y - target->y);
}
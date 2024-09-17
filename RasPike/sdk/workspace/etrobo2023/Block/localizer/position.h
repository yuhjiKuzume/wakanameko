/*
 * position.h
 *
 *  Created on: 2011/07/09
 *      Author: tdohshi
 */

#ifndef POSITION_H_
#define POSITION_H_
#define TO_RADIANS(angle) ((angle) * M_PI / 180.0)
#define TO_DEGREES(angle) ((angle) * 180.0 / M_PI)

typedef struct _Position {
	/** 現在位置x方向*/
	double x;
	/** 現在位置y方向 */
	double y;
	/** 現在向いている方向 */
	double w;
} Position;


void position_init(Position* self, double x, double y, double w);
#define position_getX(self)		((self)->x)
#define position_getY(self)		((self)->y)
#define position_getW(self)		((self)->w)
double position_getDistance(const Position* self, const Position* target);

#endif /* POSITION_H_ */
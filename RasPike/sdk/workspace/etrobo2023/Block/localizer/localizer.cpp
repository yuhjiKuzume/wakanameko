/*
 * localizer.cpp
 *
 *  Created on: 2011/07/14
 *      Author: tdohshi
 */

#include <stddef.h>
#include "etrobo_inc.h"
#include "localizer.h"
//#include "virtualDeviceDebug.h"
//#include "Motor.h"
//#include "Tracer.h"

//#define B0	(0.110825663)
//#define B1	(0.221651325)
//#define B2	(0.110825663)
//#define A1	(0.46463719)
//#define A2	(0.092060159)

static Localizer self;
double total_distance = 0;
/**
 * 移動距離取得
 */
double localizer_getDistance(void) {
	return self.distance;
}
void localizer_setDistance() {
	self.distance = 0.0;
}

/**
 * 速度取得

double localizer_getSpeed(void) {
	return self.speed;
}
 */
/**
 * 方位角度取得

 */
double localizer_getDirection(void) {
	return self.localPos.w;
}
void localizer_setDirection(){
	self.localPos.w = 0.0;
}
/**
 * 自己位置取得

const Position* localizer_getPosition(void) {
	return &self.localPos;
}
 */
/**
 * 自己位置設定

void localizer_setPosition(const Position* pos) {
	self.localPos = *pos;
}
 */
/**
 * 自己位置推定実行
 */
void localizer_run(void) {
	/* 自己位置推定 */
	Position tmpPos;
	// odometory_setBasePosition(&self.odometory, &self.localPos);
	// odometory_getNextPosition(&self.odometory,
	// 		Motor leftWheel(PORT_C),
	// 		Motor rightWheel(PORT_B),
	// 		&tmpPos);
	odometory_setBasePosition(&self.odometory, &self.localPos);
	// Motor leftWheel(PORT_C);
	// Motor rightWheel(PORT_B);
	odometory_getNextPosition(&self.odometory,
			ev3_motor_get_counts(EV3_PORT_C),
			ev3_motor_get_counts(EV3_PORT_B),
			&tmpPos);
			//printf("getcount_PORTC: %ld\n", ev3_motor_get_counts(EV3_PORT_C));
			//printf("getcount_PORTB: %ld\n", ev3_motor_get_counts(EV3_PORT_B));

	/* 移動距離取得 */
	double distance = odometory_getDistance(&self.odometory);
	//printf("distance: %lf\n", distance);
	self.distance += distance;
	
//	this.speed = 1000 * distance / LOCALIZER_CYCLIC;
//	this.speed = lpf_get(&this.lpf, 1000 * distance / LOCALIZER_CYCLIC);
//	self.speed = self.ud_speed * 0.85 + (1000 * distance / LOCALIZER_CYCLIC) * 0.15;
//	self.ud_speed = self.speed;
	/* 現在地保存 */
	self.localPos = tmpPos;

//	DBG_LOG("[LOCALIZER] x=%d, y=%d, w=%d, d=%d, s=%d",
//			(int)this.localPos.x, (int)this.localPos.y, (int)this.localPos.w, (int)this.distance, (int)this.speed);
}

/**
 * 初期化
 * @param startPos 初期位置
 */
void localizer_init(const Position* startPos) {
	self.distance = 0;
	self.localPos = *startPos;
	self.ud_speed = 0;
	odometory_init(&self.odometory, startPos);
//	lpf_init(&this.lpf, A1, A2, B0, B1, B2);
}
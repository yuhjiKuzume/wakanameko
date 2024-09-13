// /*
//  * calibrator.c
//  *
//  *  Created on: 2011/09/27
//  *      Author: tdohshi
//  */

// #include "calibrator.h"
// #include "light.h"
// #include "etrobo_inc.h"
// #include "starterDebug.h"

// //static LightThresholdType lightThresholdType = LIGHT_THRESHOLD_TYPE_DEFAULT;
// static LightThresholdType lightThresholdType = LIGHT_THRESHOLD_TYPE_MANUAL;
// int calibrate_state = CALIBRATE_STAT_WHITE;

// static float lightDefaultTarget = 50;
// static float lightManualTarget = 50;

// static float whiteBrightness = 100;	/* 白色ライン輝度値 */
// static float grayBrightness = 50;	/* 灰色ライン輝度値 */
// static float blackBrightness = 0;	/* 黒色ライン輝度値 */

// static float gaugeBrightness();

// /**
//  * 光センサ閾値タイプ設定
//  */
// void calibrator_setLightThresholdType(LightThresholdType type) {
// 	lightThresholdType = type;
// }

// /**
//  * 光センサ閾値タイプ取得
//  */
// LightThresholdType calibrator_getLightThresholdType(void) {
// 	return lightThresholdType;
// }

// /**
//  * 光センサ閾値(マニュアル)設定
//  */
// void calibrator_setLightThresholdOfManual(int threshold) {
// 	lightManualTarget = (float)threshold;
// }

// /**
//  * 光センサ閾値取得
//  */
// float calibrator_getLightThreshold(void) {
// 	float shreshold = lightDefaultTarget;

// 	switch (lightThresholdType) {
// 	case LIGHT_THRESHOLD_TYPE_DEFAULT:
// 		shreshold = lightDefaultTarget;
// 		break;
// 	case LIGHT_THRESHOLD_TYPE_HIGHSPEED:
// 		shreshold = (blackBrightness + whiteBrightness) / 2;
// 		break;
// 	case LIGHT_THRESHOLD_TYPE_NEAR_BLACK:
// 		shreshold = (blackBrightness + grayBrightness) / 2;
// 		break;
// 	case LIGHT_THRESHOLD_TYPE_NEAR_WHITE:
// 		shreshold = (whiteBrightness + grayBrightness) / 2;
// 		break;
// 	case LIGHT_THRESHOLD_TYPE_MANUAL:
// 		shreshold = lightManualTarget;
// 		break;
// 	default:
// 		break;
// 	}

// 	DBG_LOG("[CALIBRATOR] shreshold %d", (int)(shreshold*1000));
// 	return shreshold;
// }

// /**
//  * キャリブレーション
//  */
// int calibrator_calibrateBrightness(void) {
// 	int dlyTime = 120;
// 	static LightMethod curMethod = LIGHT_METHOD_NORMALIZE;

// 	switch (calibrate_state) {
// 	case CALIBRATE_STAT_WHITE:
// 		curMethod = light_getMethod();
// 		if (curMethod == LIGHT_METHOD_NORMALIZE) {
// 			// 正規化している場合は、生の光センサ値を取得できるようにする
// 			light_setMethod(LIGHT_METHOD_DEFAULT);
// 		}
// 		else if (curMethod == LIGHT_METHOD_COLOR_RGB_NORMALIZE){
// 			// 正規化している場合は、生のカラーセンサ値を取得できるようにする
// 			light_setMethod(LIGHT_METHOD_COLOR_RGB);
// 		}
// 		dly_tsk(dlyTime);
// 		whiteBrightness = gaugeBrightness();
// 		//sound_play(500, 700);
// 		calibrate_state = CALIBRATE_STAT_BLACK;
// //		APP_LOG("[CALIBRATOR] white %d", (int)(whiteBrightness));
// 		return calibrate_state;
// 	case CALIBRATE_STAT_BLACK:
// 		dly_tsk(dlyTime);
// 		blackBrightness = gaugeBrightness();
// 		//sound_play(1000, 700);
// 		light_setSaturation(whiteBrightness, blackBrightness);
// 		//calibrate_state = CALIBRATE_STAT_GRAY;
// 		//return calibrate_state;
// 		calibrate_state = CALIBRATE_STAT_WHITE;
// //		APP_LOG("[CALIBRATOR] black %d", (int)(blackBrightness));
// 		light_setMethod(curMethod);
// 		return CALIBRATE_STAT_END;
// #if 0
// 	case CALIBRATE_STAT_GRAY:
// 		dly_tsk(dlyTime);
// 		grayBrightness = gaugeBrightness();
// 		calibrate_state++;
// 		sound_play(1500, 20);

// 		DBG_LOG("[CALIBRATOR] white %d, black %d, gray %d", (int)(whiteBrightness * 1000), (int)(blackBrightness * 1000), (int)(grayBrightness * 1000));
// 		calibrate_state = CALIBRATE_STAT_WHITE;

// 		// 元の方法で光センサ値を取得できるように戻す
// 		light_setMethod(curMethod);
// 		return CALIBRATE_STAT_END;
// #endif
// 	default:
// 		return 0;
// 	}

// 	return 0;
// }

// /**
//  * ラインの輝度を計る
//  */
// static float gaugeBrightness() {
// 	int cnt = 10;
// 	float brightness = 0;

// 	while (cnt--) {
// 		brightness += light_getBrightness();
// 		dly_tsk(24);
// 	}

// 	brightness = brightness / 10;

// 	return brightness;
// }
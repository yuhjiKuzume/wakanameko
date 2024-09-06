#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ev3api.h"
#include "Distance.h"
#include "Direction.h"

/* ライントレースのエッジ定義 */
#define R_EDGE 0 // 左エッジ
#define L_EDGE 1 // 右エッジ

/* カラーセンサの反射光の白黒判断基準値 */
#define WHITE 40 //白値判断基準閾値
#define BLACK 10 //黒値判断基準閾値

#define TASK_INTERVAL 10.0 // タスク周期

/* ライン復帰の状態遷移 */
typedef enum {
    // SERCH_LINE_INIT,
    // SERCH_LINE,
    SET_DIRECTION_INIT,
    SET_DIRECTION
} ReturnLine_enum;

/* ライン探索の状態遷移 */
typedef enum {
    SERCH_L,
    BACK_L,
    SERCH_R,
    BACK_R
} serchLine_enum;

/* 車体方位調整の状態遷移 */
typedef enum {
    TURN_L,
    TURN_R
} setDirection_enum;

/* ライン検知の状態遷移 */
typedef enum {
    DISCERN_WHITE1,
    DISCERN_BLACK,
    DISCERN_WHITE2
} discernLine_enum;

/* 公開関数 */
bool_t ReturnLine_do(bool_t edge); // ライン復帰関数
void ReturnLine_init(void); // 初期化関数

#ifdef __cplusplus
}
#endif

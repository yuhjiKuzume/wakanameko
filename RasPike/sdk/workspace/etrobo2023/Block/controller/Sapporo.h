#ifdef SAPPORO
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "ev3api.h"

/*
 *  各タスクの優先度の定義
 */
// #define MAIN_PRIORITY           TMIN_APP_TPRI   /* メインタスクの優先度=0 */
// #define W_LOOP_PRIORITY         TMIN_APP_TPRI + 1
// #define CARRY_BLOCK_PRIORITY    TMIN_APP_TPRI + 1
// #define TRACER_PRIORITY         TMIN_APP_TPRI + 2
// #define LOCALIZER_PRIORITY      TMIN_APP_TPRI + 2
// #define COLOR_CHECK_PRIORITY    TMIN_APP_TPRI + 2


/*
 *  全体に関わる定数の定義
 */
////////////////////////////////////////////////////////////////////
#define LR_FLAG  (2)     /* LRコースかの分岐、Lコースは1, Rコースは2 */
#define WHITE_BRIGHTNESS    (180)   /* カラーセンサの輝度設定 */
#define BLACK_BRIGHTNESS    (10)    /* カラーセンサの輝度設定 */
////////////////////////////////////////////////////////////////////

#define LEFT     (1)     /* 左 */
#define RIGHT    (2)     /* 右 */
#define WHITE    (0)     /* 白 */
#define BLACK    (1)     /* 黒 */
#define BLUE     (2)     /* 青 */
#define RED      (3)     /* 赤 */
#define GREEN    (4)     /* 緑 */
#define YELLOW   (5)     /* 黄 */

#define SLP_TIME       (100)   /* シーンの終了判定の周期(マイクロ秒) */ 

/*
 *  直線のライントレースのスピードを調整するフラグ
 */
#define STORE_SPEED (-1)
#define BOOST_SPEED (0)
#define INIT_SPEED (1)

/*
 *  ライントレース,スピードに依存する定数の定義
 */
#define BASE_SPEED          (48)    /* ダブルループ中のベース速度(ダブルループ中の速度) */
#define LOW                 (40)    /* 低速、ブロック運びでブロックを持っていないときの速度 */
#define NORMAL              (52)    /* 中速、LAPまでのカーブの速度 */
#define HIGH                (70)    /* 高速、LAPまでの直線の速度 */
#define BLOCK_BASE_SPEED    (50)    /* ブロック保持中のベーススピード */

/*
 *  色判定サービスに依存する定数の定義
 */
#define COLOR_CHECK_NUM     (2)    /* 連続で同じ値を何回ロジックから得た場合を判定結果とする */

/*
 *  ブロック運びに依存する定数の定義
 */
#define END_LCL_NO_LINE         (60)    /* 線のない箇所を直進する距離 */
#define END_LCL_NO_LINE_CENTER  (150)   /* 線のない箇所を直進する距離(ブロックから黒線までの途中くらい) */
#define END_LCL_IN_CIRCLE       (60)    /* 〇の中を直進する距離 */
#define END_LCL_ONE_SIDE        (150)   /* 1辺をライントレースする距離 */
#define END_LCL_BLUE_BLOCK      (150)   /* 青の時の距離 */
#define END_LCL_FIRE_BLUE       (50)    /* 180旋回して青の排他距離 */
#define RIGHT_ANGLE             (42)    /* 直角 */
#define BLOCK_ANGLE             (43)    /* 直角 左位置→左旋回、右位置→右旋回のときに、ブロックがある場合に指定する*/
#define FIRST_ANGLE             (48)    /* 赤丸からブロックエリアまで曲がるときの角度 */
#define LAST_ANGLE              (60)    /* 左折から直線に入る際の曲がるときの角度 */

#define DENGER_ANGLE            (75)    /* 急遽 */
#define RDENGER_ANGLE           (80)    /* 急遽 */
#define DENGER_ANGLE2           (35)    /* 急遽 */

/*
 *  旋回に関するパラメータ
 */
#define CURV_SPEED_HIGH     (70)
#define CURV_SPEED_LOW      (55)
#define ZIG                 (8)
#define RZIG                (8)
#define ZAG                 (8)

void user_system_create(int cose);
void user_system_destroy();

#ifdef SAPPORO
}


#endif

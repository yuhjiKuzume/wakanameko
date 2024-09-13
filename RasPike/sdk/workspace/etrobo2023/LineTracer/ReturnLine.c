#include "ReturnLine.h"
#include "app.h"

/* 各関数の状態遷移変数を宣言 */
ReturnLine_enum ReturnLine_state;

/*********** ラインを見つける動作をする関数 ***********/
/* 定義 */
#define SERCH_LINE_TURN_DIR 45.0  // 探索時左右旋回角度
#define SERCH_LINE_MOVE_POWER 10  // 動作速度
#define SERCH_LINE_MOVE_DIS 100.0 // 探索距離
/* 関数宣言 */
bool_t ReturnLine_serchLine(void);    // ライン探索する関数
/* 構造体定義 */
typedef enum {  // ライン探索時の動作状態
    SERCH_TURN, // 探索方向に旋回
    SERCH_MOVE  // 目標位置まで探索しながら移動
} serchLine_Action_enum;
/* 変数宣言 */
serchLine_enum serchLine_state; // 動作の状態遷移用変数
serchLine_Action_enum serchLine_Action_state; // ライン探索の動作状態
int serchLine_count = 1;        // 探索試行回数（1からカウント）

/******** 車体の向きを調整する動作をする関数 **********/
/* 定義 */
#define  SET_DIRECTION_MOVE_POWER 45  // 動作速度
#define SET_DIRECTION_dirDiff 90     // 許容される最大旋回範囲
/* 関数宣言 */
bool_t ReturnLine_setDirection(bool_t edge); // 方位を調整する関数
void ReturnLine_setDirection_init(void);     // 初期化関数
/* 変数宣言 */
setDirection_enum setDirection_state;
static float setDirection_Ldir = SET_DIRECTION_dirDiff; //左右の旋回量
static float setDirection_Rdir = SET_DIRECTION_dirDiff; //初回時に越えないように閾値を代入
static float setDirection_predir = 0.0; // 方位過去値(旋回開始時方位)

/*********** ライン見つけたか判断する関数 ***********/
/* 関数宣言 */
bool_t ReturnLine_discernLine(void);    // ラインの検知を行う関数
void ReturnLine_discernLine_init(void); // 初期化関数
/* 変数宣言 */
discernLine_enum discernLine_state; // 状態遷移関数




/************************************
ライン復帰初期化関数
*************************************/
void ReturnLine_init(void) {
    /* ライン復帰関数 */
    ReturnLine_state = SET_DIRECTION_INIT;
    //ReturnLine_state = SERCH_LINE_INIT;
    /* ラインを探索する関数 */
    serchLine_state = SERCH_L;
    serchLine_Action_state = SERCH_TURN;
    serchLine_count = 1;
    /* 方位を調整する関数 */
    ReturnLine_setDirection_init();
    /* ラインを検知する関数 */
    ReturnLine_discernLine_init();
    /* 計測器初期化 */
    Direction_init();
    Distance_init();
}

/************************************
ライン復帰関数
false：ライン復帰中
true:ライン復帰完了
*************************************/
bool_t ReturnLine_do(bool_t edge) {
    switch(ReturnLine_state) {
    // case SERCH_LINE_INIT: // ライン探索前の初期化処理
    //     ReturnLine_init();
    //     ReturnLine_state = SERCH_LINE;
    //     break;
    // case SERCH_LINE: // ライン探索実行
    //     if(ReturnLine_serchLine() == true) {
    //         ReturnLine_state = SET_DIRECTION_INIT;
    //     }
    //     break;
    case SET_DIRECTION_INIT: // 車体方位調整前の初期化処理
        printf("初期化処理\n");
        ReturnLine_setDirection_init();
        ReturnLine_discernLine_init();
        ReturnLine_state = SET_DIRECTION;
        break;
    case SET_DIRECTION: // 方位調整実行
        if(ReturnLine_setDirection(edge) == true) {
            ReturnLine_state = SET_DIRECTION_INIT;
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

/******************************************
ラインを探索する関数（非公開処理）
false：ライン探索中
true：ライン探索完了
*******************************************/
bool_t ReturnLine_serchLine(void) {
    float cur_dir =  Direction_getDirection();
    float cur_dis =  Distance_getDistance();

    switch(serchLine_state) {
    case SERCH_L: /************** 左側方向を探索する **************/
        switch(serchLine_Action_state) {
        case SERCH_TURN: /* 探索位置に車体方位を向ける（マイナス角度方位） */
            // 左側に旋回
            ev3_motor_set_power(left_motor,  (SERCH_LINE_MOVE_POWER*-1));
            ev3_motor_set_power(right_motor,  SERCH_LINE_MOVE_POWER);
            // 左側の指定方位まで旋回したら、前進動作実行
            if(cur_dir <= (SERCH_LINE_TURN_DIR*-1.0)) {
                ReturnLine_discernLine_init();
                serchLine_Action_state = SERCH_MOVE;
            }
            break;
        case SERCH_MOVE: /* 探索位置まで移動する */
            // 直進
            ev3_motor_set_power(left_motor,  SERCH_LINE_MOVE_POWER);
            ev3_motor_set_power(right_motor,  SERCH_LINE_MOVE_POWER);
            // 指定位置まで移動したら、初期位置に戻る状態に遷移
            if(cur_dis >= SERCH_LINE_MOVE_DIS*serchLine_count) {
                ReturnLine_discernLine_init();
                /* 次はバックしてから旋回するため，移動状態から */
                serchLine_Action_state = SERCH_MOVE;
                /* 次の状態に遷移 */
                serchLine_state = BACK_L;
            }
            break;
        }
        break;
    case BACK_L: /********** 左側探索位置から探索位置に戻る **********/
        switch(serchLine_Action_state) {
        case SERCH_MOVE: /* スタート位置まで後ろにバックする */
            // 後進
            ev3_motor_set_power(left_motor,  (SERCH_LINE_MOVE_POWER*-1));
            ev3_motor_set_power(right_motor,  (SERCH_LINE_MOVE_POWER*-1));
            // 初期位置まで移動
            if(cur_dis <= 0.0) {
                ReturnLine_discernLine_init();
                serchLine_Action_state = SERCH_TURN;
            }
            break;
        case SERCH_TURN: /* 車体を探索開始時の向きに戻す（左（-角度）から右（+角度）） */
            // 右向きに旋回
            ev3_motor_set_power(left_motor,  SERCH_LINE_MOVE_POWER);
            ev3_motor_set_power(right_motor,  (SERCH_LINE_MOVE_POWER*-1));
            // 初期方位に戻るまで旋回（次は右側探索）
            if(cur_dir >= 0.0) {
                ReturnLine_discernLine_init();
                /* 次は旋回してから進むため */
                serchLine_Action_state = SERCH_TURN;
                /* 次の状態に遷移 */
                serchLine_state =  SERCH_R;
            }
            break;
        }
        break;
    case SERCH_R: /************** 右側方向を探索する **************/
        switch(serchLine_Action_state) {
        case SERCH_TURN: /* 探索位置に車体方位を向ける(+方向) */
            ev3_motor_set_power(left_motor,  SERCH_LINE_MOVE_POWER);
            ev3_motor_set_power(right_motor,  (SERCH_LINE_MOVE_POWER*-1));
            if(cur_dir >= SERCH_LINE_TURN_DIR) {
                ReturnLine_discernLine_init();
                serchLine_Action_state = SERCH_MOVE;
            }
            break;
        case SERCH_MOVE: /* 探索位置まで移動する */
            ev3_motor_set_power(left_motor,  SERCH_LINE_MOVE_POWER);
            ev3_motor_set_power(right_motor,  SERCH_LINE_MOVE_POWER);
            if(cur_dis >= SERCH_LINE_MOVE_DIS*serchLine_count) {
                ReturnLine_discernLine_init();
                /* 次はバックしてから旋回するため，移動状態から */
                serchLine_Action_state = SERCH_MOVE;
                /* 次の状態に遷移 */
                serchLine_state = BACK_R;
            }
            break;
        }
        break;
    case BACK_R: /********** 右側探索位置から探索位置に戻る **********/
        switch(serchLine_Action_state) {
        case SERCH_MOVE: /* スタート位置まで後ろにバックする */
            ev3_motor_set_power(left_motor,  (SERCH_LINE_MOVE_POWER*-1));
            ev3_motor_set_power(right_motor,  (SERCH_LINE_MOVE_POWER*-1));
            if(cur_dis <= 0.0) {
                ReturnLine_discernLine_init();
                serchLine_Action_state = SERCH_TURN;
            }
            break;
        case SERCH_TURN: /* 車体を探索開始時の向きに戻す（右（+方向）から左（-方向）） */
            // 
            ev3_motor_set_power(left_motor,  (SERCH_LINE_MOVE_POWER*-1));
            ev3_motor_set_power(right_motor,  SERCH_LINE_MOVE_POWER);
            if(cur_dir <= 0.0) {
                ReturnLine_discernLine_init();
                /* 次は旋回してから進むため */
                serchLine_Action_state = SERCH_TURN;
                /* また左側探索の状態に遷移 */
                serchLine_state =  SERCH_L;
                serchLine_count++;
            }
            break;
        }
        break;
    default:
        break;
    }

    // ラインを検知したのが、右側か左側かで
    // 次の車体方位の調整を行う際の左右旋回の方向を決定
    if(ReturnLine_discernLine() == true) {
        if(serchLine_state == SERCH_L) {
            setDirection_state = TURN_R;
        } else
            if(serchLine_state == SERCH_R) {
                setDirection_state = TURN_L;
            }
        return true;
    } else {
        return false;
    }
}


/**************************************
車体の方位を調整する関数
[引数]bool_t edge：ライントレースのエッジ
false方位の調整中
true：方位の調整終了
***************************************/
bool_t ReturnLine_setDirection(bool_t edge) {
    float cur_dir =  Direction_getDirection();
    float check_dir = 0.0;

    switch(setDirection_state) {
    case TURN_R: // 右側方向に旋回しながらライン検知
        // 右側旋回
        ev3_motor_set_power(left_motor, SET_DIRECTION_MOVE_POWER);
        ev3_motor_set_power(right_motor, (SET_DIRECTION_MOVE_POWER*-1));
        // ラインを検知したら，旋回角度計算
        if(ReturnLine_discernLine() == true) {
            // 右側旋回時の旋回角度を格納
            setDirection_Rdir = cur_dir - setDirection_predir;
            // 次の左側旋回時角度計算のために，現在方位を過去値として格納
            setDirection_predir = cur_dir;
            // ライン検知を初期化
            ReturnLine_discernLine_init();
            setDirection_state = TURN_L;
        }
        break;
    case TURN_L: // 左側方向に旋回しながらライン検知
        // 左側旋回
        ev3_motor_set_power(left_motor, (SET_DIRECTION_MOVE_POWER*-1));
        ev3_motor_set_power(right_motor, SET_DIRECTION_MOVE_POWER);
        // ラインを検知したら，旋回角度計算
        if(ReturnLine_discernLine() == true) {
            // 左側旋回時の旋回角度を格納
            setDirection_Ldir = setDirection_predir - cur_dir;
            // 次の右側旋回時角度計算のために，現在方位を過去値として格納
            setDirection_predir = cur_dir;
            // ライン検知を初期化
            ReturnLine_discernLine_init();
            setDirection_state = TURN_R;
        }
        break;
    default:
        break;
    }

    // ライントレース時のエッジがどちらかで
    // 右側旋回で終わるか，左側旋回で終わるかを決定
    if(edge == L_EDGE) {
        if(setDirection_Ldir <  SET_DIRECTION_dirDiff) {
            return true;
        } 
    } else
        if(edge == R_EDGE) {
            if(setDirection_Rdir <  SET_DIRECTION_dirDiff) {
                return true;
            } 
        }

    return false;

}
/*****************************************
車体の方位を調整する　初期化関数
******************************************/
void ReturnLine_setDirection_init(void) {
    setDirection_predir = Direction_getDirection();
    setDirection_Ldir = SET_DIRECTION_dirDiff;
    setDirection_Rdir = SET_DIRECTION_dirDiff;
}

/*****************************************
ラインを検知する関数
false：ライン未検知
true：ライン検知済み
******************************************/
bool_t ReturnLine_discernLine(void) {
    int color_sensor_reflect = ev3_color_sensor_get_reflect(color_sensor);

    switch (discernLine_state) {
    case DISCERN_WHITE1: // 白を検知
        if(color_sensor_reflect >= WHITE) {
            discernLine_state = DISCERN_BLACK;
        }
        break;
    case DISCERN_BLACK: // 黒を検知
        if(color_sensor_reflect <= BLACK ) {
            discernLine_state = DISCERN_WHITE2;
        }
        break;
    case DISCERN_WHITE2: // 白を検知
        if(color_sensor_reflect >= WHITE) {
            discernLine_state = DISCERN_WHITE1;
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}
/***************************************
ラインを検知する 初期化関数
****************************************/
void ReturnLine_discernLine_init(void) {
    discernLine_state = DISCERN_WHITE1;
}
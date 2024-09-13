#ifndef PATTERN_H_
#define PATTERN_H_

#include "Sapporo.h"
#include "SceneInformation.h"

// { 実行する動作, 開始時の走行体の位置, 終了条件, 終了条件詳細-走行距離, その他の情報(ライントレース時のスピードなど) }
// ダブルループのシーン
static SceneInformation gScenes[] = {
    // { LINE_TRACE, LEFT_POSITION,  LOCALIZER,       END_LCL_SCENE0,       HIGH },            // シーン0 最初のカーブまで
    // { LINE_TRACE, LEFT_POSITION,  LOCALIZER,       END_LCL_SCENE1,       NORMAL },          // シーン1 カーブ中
    // { LINE_TRACE, LEFT_POSITION,  LOCALIZER,       END_LCL_SCENE2,       HIGH },            // シーン2 2つ目のカーブまで
    // { LINE_TRACE, LEFT_POSITION,  END_BLUE,        END_LCL_BLUE_SCENE3,  BASE_SPEED },      // シーン3 カーブを曲がって青まで
    // { CROSS_LINE, LEFT_POSITION,  CROSS_LINE_END,  0,                    0 },               // シーン4
    // { LINE_TRACE, RIGHT_POSITION, LOCALIZER,       END_LCL_SHORT,        BASE_SPEED },      // シーン5
    // { CROSS_LINE, RIGHT_POSITION, CROSS_LINE_END,  0,                    0 },               // シーン6
    // { LINE_TRACE, LEFT_POSITION,  END_BLUE,        END_LCL_BLUE_SCENE7,  BASE_SPEED },      // シーン7 大きい円の青まで
    // { CROSS_LINE, LEFT_POSITION,  CROSS_LINE_END,  0,                    0 },               // シーン8
    // { LINE_TRACE, RIGHT_POSITION, LOCALIZER,       0,                    BASE_SPEED },      // シーン9
    // { LINE_TRACE, RIGHT_POSITION, END_BLUE,        END_LCL_BLUE_SCENE10, BASE_SPEED },      // シーン10 大きい円を1週し青
    // { LINE_TRACE, RIGHT_POSITION, END_BLACK,       0,                    BASE_SPEED },      // シーン11 青から、黒
    // { CROSS_LINE, RIGHT_POSITION, CROSS_LINE_END,  0,                    0 },               // シーン12
    // { LINE_TRACE, LEFT_POSITION,  END_BLUE,        END_LCL_BLUE_SCENE13, BASE_SPEED },      // シーン13(小さい円を1週) ！！
    // { LINE_TRACE, LEFT_POSITION,  END_BLACK,       0,                    BASE_SPEED },      // シーン14 青から、黒
    // { CROSS_LINE, LEFT_POSITION,  CROSS_LINE_END,  0,                    0 },               // シーン15
    // { LINE_TRACE, RIGHT_POSITION, LOCALIZER,       END_LCL_SHORT,        BASE_SPEED },      // シーン16
    // { CROSS_LINE, RIGHT_POSITION, CROSS_LINE_END,  0,                    0 },               // シーン17
    // { LINE_TRACE, LEFT_POSITION,  LOCALIZER,       END_LCL_SHORT,        BASE_SPEED },      // シーン18
    // { CROSS_LINE, LEFT_POSITION,  CROSS_LINE_END,  0,                    0 },               // シーン19
    // { LINE_TRACE, RIGHT_POSITION, LOCALIZER,       END_LCL_LONG,         BASE_SPEED },      // シーン20 内側走行やめる
    // { CROSS_LINE, RIGHT_POSITION, CROSS_LINE_END,  0,                    0 },               // シーン21
    // { LINE_TRACE, LEFT_POSITION,  END_BLUE,        END_LCL_BLUE_SCENE22, NORMAL },          // シーン22 カーブして青まで
    { LINE_TRACE, LEFT_POSITION,  END_RED,         0,                    35 },          // シーン23 赤丸まで
};


// ブロック運びのシーン
// 全てのサークルを通るルート
static SceneInformation gBlockScenes_all_circle[] = {

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          DENGER_ANGLE },     // シーン0 赤丸からほぼ90度に旋回する
    { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,  END_LCL_NO_LINE_CENTER+100, LOW-5 },            // シーン1 何もないところを直進途中まで
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          DENGER_ANGLE2 },    // シーン2 少し旋回
    { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,  END_LCL_NO_LINE,            LOW-5 },            // シーン3 何もないところを直進
    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          FIRST_ANGLE },      // シーン4 曲がる
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   0,                          LOW-5 },            // シーン5 青丸まで

    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン6 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          LOW-5 },            // シーン7 赤丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン8 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    END_LCL_ONE_SIDE,           LOW },              // シーン9 赤丸まで【1辺】

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン10 右折
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          LOW },              // シーン11 赤丸まで

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン12 右折
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          LOW },              // シーン13 赤丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン14 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   END_LCL_ONE_SIDE,           LOW },              // シーン15 青丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン16 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   END_LCL_ONE_SIDE,           LOW },              // シーン17 青丸まで【1辺】

    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン18 左折
    { LINE_TRACE,   RIGHT_POSITION, END_GREEN,  0,                          LOW },              // シーン19 緑丸まで

    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          RIGHT_ANGLE },      // シーン20 左折
    { LINE_TRACE,   RIGHT_POSITION, END_GREEN,  0,                          LOW },              // シーン21 緑丸まで
    { GO_ZIG_ZAG,   RIGHT_POSITION, TURN_END,   0,                          0 },                // シーン22 ジグザグ直進
    { LINE_TRACE,   RIGHT_POSITION, END_YELLOW, END_LCL_ONE_SIDE,           LOW },              // シーン23 黄丸まで
    { GO_ZIG_ZAG,   RIGHT_POSITION, TURN_END,   0,                          0 },                // シーン24 ジグザグ直進
    { LINE_TRACE,   RIGHT_POSITION, END_YELLOW, END_LCL_ONE_SIDE,           LOW },              // シーン25 黄丸まで【1辺】

    { TURN_RIGHT,   RIGHT_POSITION, TURN_END,   0,                          RIGHT_ANGLE },      // シーン26 右折
    { LINE_TRACE,   LEFT_POSITION,  END_YELLOW, END_LCL_ONE_SIDE,           LOW },              // シーン27 黄丸まで

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン28 右折
    { LINE_TRACE,   LEFT_POSITION,  END_YELLOW, END_LCL_ONE_SIDE,           LOW },              // シーン29 黄丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン30 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_GREEN,  END_LCL_ONE_SIDE,           LOW },              // シーン31 緑丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン32 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_GREEN,  END_LCL_ONE_SIDE,           LOW },              // シーン33 緑丸まで

    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          LAST_ANGLE },       // シーン34 左折して直線なのでLAST_ANGLE
    { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,  END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED }, // シーン35 何もないところを直進途中まで
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          FIRST_ANGLE },      // シーン36 少し旋回
    { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,  END_LCL_NO_LINE,            BLOCK_BASE_SPEED }, // シーン37 黒線まで何もないところを直進
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          FIRST_ANGLE },      // シーン38 左折
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   0,                          BLOCK_BASE_SPEED }, // シーン39 ゴールまで
};

// 赤と青のサークルを通るルート
static SceneInformation gBlockScenes_red_and_blue_circle[] = {

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          DENGER_ANGLE },     // シーン0 赤丸からほぼ90度に旋回する
    { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,  END_LCL_NO_LINE_CENTER+100, LOW },            // シーン1 何もないところを直進途中まで
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          DENGER_ANGLE2 },    // シーン2 少し旋回
    { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,  END_LCL_NO_LINE,            LOW },            // シーン3 何もないところを直進
    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          FIRST_ANGLE-3 },      // シーン4 曲がる
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   0,                          LOW },            // シーン5 青丸まで

    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン6 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          LOW },            // シーン7 赤丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン8 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    END_LCL_ONE_SIDE,           BASE_SPEED },              // シーン9 赤丸まで【1辺】

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン10 右折
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          BASE_SPEED },              // シーン11 赤丸まで

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン12 右折
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          BASE_SPEED },              // シーン13 赤丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン14 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   END_LCL_ONE_SIDE,           BASE_SPEED },              // シーン15 青丸まで
    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン16 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   END_LCL_ONE_SIDE,           BASE_SPEED },              // シーン17 青丸まで【1辺】

    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン18 左折
    { LINE_TRACE,   RIGHT_POSITION, END_GREEN,  0,                          BASE_SPEED },              // シーン19 緑丸まで

    { GO_ZIG_ZAG,   RIGHT_POSITION,  TURN_END,   0,                         0 },                // シーン20 ジグザグ直進
    { LINE_TRACE,   RIGHT_POSITION,  END_GREEN,  0,                         BASE_SPEED },              // シーン21 緑丸まで

    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン22 ジグザグ直進
    { GO_NO_LINE,   LEFT_POSITION,  LOCALIZER,  END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED }, // シーン23 何もないところを直進途中まで
    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          FIRST_ANGLE-5 },      // シーン24 少し旋回
    { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,  END_LCL_NO_LINE,            BLOCK_BASE_SPEED }, // シーン25 黒線まで何もないところを直進
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          FIRST_ANGLE },      // シーン26 左折
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   0,                          BLOCK_BASE_SPEED }, // シーン27 ゴールまで
};

// 黄と緑のサークルを通るルート
static SceneInformation gBlockScenes_yellow_and_green_circle[] = {

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          DENGER_ANGLE },     // シーン0 赤丸からほぼ90度に旋回する
    { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,  END_LCL_NO_LINE_CENTER+100, LOW },            // シーン1 何もないところを直進途中まで
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          DENGER_ANGLE2 },    // シーン2 少し旋回
    { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,  END_LCL_NO_LINE,            LOW },            // シーン3 何もないところを直進
    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          FIRST_ANGLE-3 },      // シーン4 曲がる
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   0,                          LOW },            // シーン5 青丸まで

    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン6 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          LOW },            // シーン7 赤丸まで

    { TURN_RIGHT,   RIGHT_POSITION, TURN_END,   0,                          RIGHT_ANGLE },      // シーン8 右折
    { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                          BASE_SPEED },              // シーン9 赤丸まで

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン10 右折
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   END_LCL_ONE_SIDE,           BASE_SPEED },              // シーン11 青丸まで

    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン12 左折
    { LINE_TRACE,   RIGHT_POSITION, END_GREEN,  0,                          BASE_SPEED },              // シーン13 緑丸まで

    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          RIGHT_ANGLE },      // シーン14 左折
    { LINE_TRACE,   RIGHT_POSITION, END_YELLOW, 0,                          BASE_SPEED },              // シーン15 黄丸まで

    { GO_ZIG_ZAG,   RIGHT_POSITION, TURN_END,   0,                          0 },                // シーン16 ジグザグ直進
    { LINE_TRACE,   RIGHT_POSITION, END_YELLOW, END_LCL_ONE_SIDE,           BASE_SPEED },              // シーン17 黄丸まで

    { TURN_RIGHT,   RIGHT_POSITION, TURN_END,   0,                          RIGHT_ANGLE },      // シーン18 右折
    { LINE_TRACE,   LEFT_POSITION,  END_YELLOW, 0,                          BASE_SPEED },              // シーン19 黄丸まで

    { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                          RIGHT_ANGLE },      // シーン20 右折
    { LINE_TRACE,   LEFT_POSITION,  END_YELLOW, 0,                          BASE_SPEED },              // シーン21 黄丸まで

    { GO_ZIG_ZAG,   LEFT_POSITION,  TURN_END,   0,                          0 },                // シーン22 ジグザグ直進
    { LINE_TRACE,   LEFT_POSITION,  END_GREEN,  0,                          BASE_SPEED },              // シーン23 緑丸まで

    { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                          LAST_ANGLE },       // シーン24 左折して直線なのでLAST_ANGLE
    { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,  END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED }, // シーン25 何もないところを直進途中まで
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          FIRST_ANGLE },      // シーン26 少し旋回
    { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,  END_LCL_NO_LINE,            BLOCK_BASE_SPEED }, // シーン27 黒線まで何もないところを直進
    { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                          FIRST_ANGLE+3 },      // シーン28 左折
    { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   0,                          BLOCK_BASE_SPEED }, // シーン29 ゴールまで
};


// 1-1 緑ダイヤパターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          FIRST_ANGLE },     // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,   END_LCL_NO_LINE,            LOW },             // シーン1 何もないところを直進
//     { TURN_LEFT,    LEFT_POSITION,  TURN_END,    0,                          FIRST_ANGLE },     // シーン2 曲がる
//     { LINE_TRACE,   LEFT_POSITION,  END_RED,     0,                          LOW },             // シーン3 赤丸まで
//     { TURN_LEFT,    LEFT_POSITION,  TURN_END,    0,                          GO_ANGLE },        // シーン4 直進のために左折
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          GO_ANGLE },        // シーン5 直進のための右折
//     { LINE_TRACE,   LEFT_POSITION,  END_RED,     END_LCL_ONE_SIDE,           LOW },             // シーン6 赤丸まで 
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          BLOCK_ANGLE },     // シーン7 右折 【ブロック】
//     { LINE_TRACE,   LEFT_POSITION,  LOCALIZER,   END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン8
//     { GO_TO_CIRCLE, LEFT_POSITION,  END_RED,     0,                          BLOCK_BASE_SPEED },      // シーン9 赤丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,  LOCALIZER,   END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },      // シーン10 丸の中を直進
//     { LINE_TRACE,   LEFT_POSITION,  END_YELLOW,  END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン11 黄丸まで
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          BLOCK_BASE_SPEED },      // シーン12 右折
//     { LINE_TRACE,   LEFT_POSITION,  END_YELLOW,  END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン13
//     { TURN_LEFT,    LEFT_POSITION,  TURN_END,    0,                          RIGHT_ANGLE },     // シーン14 左折
//     { LINE_TRACE,   RIGHT_POSITION, LOCALIZER,   END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン15
//     { GO_TO_CIRCLE, RIGHT_POSITION, END_YELLOW,  END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン16 黄丸まで 
//     { GO_IN_CIRCLE, RIGHT_POSITION, LOCALIZER,   END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED},       // シーン17 丸の中を直進
//     { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,   END_LCL_NO_LINE_CENTER+50,  BLOCK_BASE_SPEED },      // シーン18 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION, TURN_END,    0,                          FIRST_ANGLE },     // シーン19 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,   END_LCL_NO_LINE,            BLOCK_BASE_SPEED },      // シーン20 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION, TURN_END,    0,                          FIRST_ANGLE },     // シーン21
//     { LINE_TRACE,   LEFT_POSITION,  END_RED,     0,                          BLOCK_BASE_SPEED },      // シーン22 ゴールまで
// };

// 1-2 青ダイヤパターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                       FIRST_ANGLE },     // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,         LOW },             // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                       FIRST_ANGLE },     // シーン2
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                       LOW },             // シーン3 赤丸からスタート
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                       RIGHT_ANGLE },     // シーン4 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                       LOW },             // シーン5 赤丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                       RIGHT_ANGLE },     // シーン6 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_BLUE,  END_LCL_BLUE_BLOCK,      LOW },             // シーン7 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                       BLOCK_ANGLE },     // シーン8 左折【ブロック】
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,        BLOCK_BASE_SPEED },      // シーン9 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                       BLOCK_BASE_SPEED },      // シーン10 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,       BLOCK_BASE_SPEED },      // シーン11 丸の中を直進 
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,        BLOCK_BASE_SPEED },      // シーン12
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                       BLOCK_BASE_SPEED },      // シーン13 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,       BLOCK_BASE_SPEED },      // シーン14 丸の中を直進
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER, END_LCL_NO_LINE_CENTER,  BLOCK_BASE_SPEED },      // シーン15 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                       FIRST_ANGLE },     // シーン16 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,         BLOCK_BASE_SPEED },      // シーン17 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                       FIRST_ANGLE },     // シーン18
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                       BLOCK_BASE_SPEED },      // シーン19 ゴールまで
// };

// 1-3パターン
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,      0,                      FIRST_ANGLE },  // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,     END_LCL_NO_LINE,        LOW },          // シーン1 何もないところを直進
//     { TURN_LEFT,    LEFT_POSITION,  TURN_END,      0,                       FIRST_ANGLE },  // シーン2
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,       0,                      LOW },          // シーン3 赤丸からスタート
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,      0,                      RIGHT_ANGLE },  // シーン4 右折
//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,     END_LCL_ONE_SIDE,       LOW },          // シーン5 
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_RED,       0,                      LOW },          // シーン6 赤丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,     END_LCL_IN_CIRCLE,      LOW },          // シーン7 丸の中を直進
//     { LINE_TRACE,   LEFT_POSITION,   END_YELLOW,    END_LCL_ONE_SIDE,       LOW },          // シーン8 黄丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,      0,                      RIGHT_ANGLE },  // シーン9 左折
//     { LINE_TRACE,   RIGHT_POSITION,  END_YELLOW,    END_LCL_ONE_SIDE,       LOW },          // シーン10 黄丸まで
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,      0,                      RIGHT_ANGLE },  // シーン11 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_YELLOW,    END_LCL_ONE_SIDE-50,    LOW },          // シーン12 黄丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,      0,                      BLOCK_ANGLE },  // シーン13 右折【ブロック】
//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,     END_LCL_ONE_SIDE,       BLOCK_BASE_SPEED },   // シーン14 
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_YELLOW,    END_LCL_ONE_SIDE,       BLOCK_BASE_SPEED },   // シーン15 黄丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,     END_LCL_IN_CIRCLE,      BLOCK_BASE_SPEED },   // シーン16 丸の中を直進
//     { LINE_TRACE,   LEFT_POSITION,   END_GREEN,     0,                      BLOCK_BASE_SPEED },   // シーン17 緑丸まで
//     { TURN_LEFT,    RIGHT_POSITION,   TURN_END,      0,                     LAST_ANGLE },   // シーン18 左折
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER,     END_LCL_NO_LINE_CENTER, BLOCK_BASE_SPEED },   // シーン19 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,      0,                      FIRST_ANGLE },  // シーン20 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,     END_LCL_NO_LINE,        BLOCK_BASE_SPEED },   // シーン21 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,      0,                      FIRST_ANGLE},   // シーン22
//     { LINE_TRACE,   RIGHT_POSITION,  END_RED,       0,                      BLOCK_BASE_SPEED },   // シーン23 ゴールまで
// };

// 2-1 青ダイヤパターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          FIRST_ANGLE },  // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            LOW },          // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },  // シーン2
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          LOW },          // シーン3 赤丸からスタート
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          RIGHT_ANGLE },  // シーン4 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          LOW },          // シーン5 赤丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          RIGHT_ANGLE},   // シーン6 右折　　　　　【ブロック】
//     { LINE_TRACE,   LEFT_POSITION,   END_BLUE,  END_LCL_BLUE_BLOCK,         LOW },   // シーン7 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                          BLOCK_ANGLE },  // シーン8 左折
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },   // シーン9 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                          BLOCK_BASE_SPEED },   // シーン10 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },   // シーン11 丸の中を直進 
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },   // シーン12
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                          BLOCK_BASE_SPEED },   // シーン13 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },   // シーン14 丸の中を直進
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER, END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED },   // シーン15 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },  // シーン16 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            BLOCK_BASE_SPEED },   // シーン17 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },  // シーン18
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          BLOCK_BASE_SPEED },   // シーン19 ゴールまで
// };

// 2-2パターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                         FIRST_ANGLE },      // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,           LOW },              // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                         FIRST_ANGLE },      // シーン2  
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                         LOW },              // シーン3 赤丸まで
//     { FIRE_TURN,    LEFT_POSITION,   TURN_END,  0,                         FIRE_ANGLE },       // シーン4 180度旋回する
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_FIRE_BLUE,         LOW },              // シーン5 青丸まで
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,  0,                         GO_ANGLE },         // シーン6 
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                         GO_ANGLE },         // シーン7 
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_BLUE_BLOCK,        BLOCK_BASE_SPEED },       // シーン8 青丸まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                         BLOCK_ANGLE },      // シーン9 青丸左折【ブロック】
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },       // シーン10 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_BLUE,  0,                         BLOCK_BASE_SPEED },       // シーン11 青丸まで直進 
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },       // シーン12 丸の中を直進 
//     { LINE_TRACE,   RIGHT_POSITION,  END_GREEN, 0,                         BLOCK_BASE_SPEED },       // シーン13 緑丸まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                         RIGHT_ANGLE },      // シーン14 緑丸左折
//     { LINE_TRACE,   RIGHT_POSITION,  END_GREEN, 0,                         BLOCK_BASE_SPEED },       // シーン15 緑丸まで
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,  0,                         RIGHT_ANGLE },      // シーン16 緑丸右折
//     { LINE_TRACE,   LEFT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },       // シーン17 緑丸まで
//     { GO_TO_CIRCLE, LEFT_POSITION,  END_GREEN, 0,                         BLOCK_BASE_SPEED },       // シーン18 
//     { GO_IN_CIRCLE, LEFT_POSITION,  TURN_END,  0,                         BLOCK_BASE_SPEED },       // シーン19 丸の中を直進
//     { GO_NO_LINE,   LEFT_POSITION,   LOCALIZER, END_LCL_NO_LINE_CENTER,    BLOCK_BASE_SPEED },       // シーン20 何もないところを直進途中まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                         FIRST_ANGLE },      // シーン21 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,   END_BLACK, END_LCL_NO_LINE,           BLOCK_BASE_SPEED },       // シーン22 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                         FIRST_ANGLE },      // シーン23
//     { LINE_TRACE,   RIGHT_POSITION,  END_RED,   0,                         BLOCK_BLOCK_BASE_SPEED },           // シーン24 ゴールまで
// };

// 2-3 パターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          FIRST_ANGLE },      // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            LOW },              // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン2  
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          LOW },              // シーン3 赤丸まで
//     { FIRE_TURN,    LEFT_POSITION,   TURN_END,  0,                          FIRE_ANGLE },       // シーン4 180度旋回する
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_FIRE_BLUE,          LOW },              // シーン5 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                          RIGHT_ANGLE },      // シーン4 青丸からスタート左折
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           LOW },              // シーン5 青丸まで
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_BLUE,  0,                          LOW },              // シーン6 
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          LOW },              // シーン7 丸の中を直進
//     { LINE_TRACE,   RIGHT_POSITION,  END_GREEN, END_LCL_ONE_SIDE,           LOW },              // シーン8 緑丸まで
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,  0,                          RIGHT_ANGLE },      // シーン9 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_GREEN, END_LCL_ONE_SIDE,           LOW },              // シーン10 緑丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                          RIGHT_ANGLE },      // シーン11 左折
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           LOW },              // シーン12 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                          LOW },              // シーン13 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },       // シーン14 丸の中を直進【ブロック】
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER, END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED },       // シーン15 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン16 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            BLOCK_BASE_SPEED },       // シーン17 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン18
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          BLOCK_BASE_SPEED },       // シーン19 ゴールまで
// };


// 3-1 黒ダイヤパターン
// static SceneInformation gBlockScenes3_1[] = {

//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         DENGER_ANGLE },     // シーン0 赤丸からほぼ90度に旋回する
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER,  END_LCL_NO_LINE_CENTER+100, BLOCK_BASE_SPEED }, // シーン1 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                         DENGER_ANGLE2 },    // シーン2 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,  END_LCL_NO_LINE,           LOW },              // シーン3 何もないところを直進
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         FIRST_ANGLE },      // シーン4 曲がる

//     { LINE_TRACE,   LEFT_POSITION,   END_BLUE,   0,                         LOW },             // シーン5 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン6 直進のために左折
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン7 直進のための右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,    0,                         LOW },             // シーン8 赤丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン9 直進のために左折
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン10 直進のための右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,    END_LCL_ONE_SIDE,          LOW },             // シーン11 赤丸まで 【ブロック】
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,   0,                         RIGHT_ANGLE },     // シーン12 右折
//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン13
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_RED,    0,                         BLOCK_BASE_SPEED },      // シーン14 赤丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },      // シーン15 丸まで直進
//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン16
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_YELLOW, 0,                         BLOCK_BASE_SPEED },      // シーン17 黄丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },      // シーン18 丸まで直進
//     { LINE_TRACE,   LEFT_POSITION,   END_YELLOW, END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン19 黄丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         RIGHT_ANGLE },     // シーン20 右折
//     // { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン21
//     { LINE_TRACE,   LEFT_POSITION,   END_YELLOW, END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン19 黄丸まで
//     // { GO_TO_CIRCLE, LEFT_POSITION,   END_YELLOW, 0,                         BLOCK_BASE_SPEED },      // シーン22 黄丸まで

//     // { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },      // シーン23 丸まで直進
//     // { LINE_TRACE,   LEFT_POSITION,   END_GREEN,  END_LCL_ONE_SIDE-50,       BLOCK_BASE_SPEED },      // シーン24 緑丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         LAST_ANGLE },      // シーン25 左折して直線なのでLAST_ANGLE
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER,  END_LCL_NO_LINE_CENTER,    BLOCK_BASE_SPEED },      // シーン26 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                         FIRST_ANGLE },     // シーン27 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,  END_LCL_NO_LINE,           BLOCK_BASE_SPEED },      // シーン28 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                         FIRST_ANGLE },     // シーン29
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,   0,                         BLOCK_BASE_SPEED },      // シーン30 ゴールまで
// };

// 3-2青ダイヤパターン【済】
// static SceneInformation gBlockScenes3_2[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                             DENGER_ANGLE },     // シーン0 赤丸からほぼ90度に旋回する
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER,  END_LCL_NO_LINE_CENTER+100,    BLOCK_BASE_SPEED }, // シーン1 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                             DENGER_ANGLE2 },    // シーン2 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,  END_LCL_NO_LINE,               LOW },              // シーン3 何もないところを直進
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                             FIRST_ANGLE },      // シーン4 曲がる

//     { LINE_TRACE,   LEFT_POSITION,   END_BLUE,   0,                             LOW },              // シーン5 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                             GO_ANGLE },         // シーン6 直進のために左折
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                             GO_ANGLE },         // シーン7 直進のための右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,    0,                             LOW },              // シーン8 赤丸まで

//     { FIRE_TURN,    LEFT_POSITION,   TURN_END,  0,                              FIRE_ANGLE },       // シーン9 180度旋回する
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_FIRE_BLUE,              LOW },              // シーン10 青丸まで
//     { TURN_LEFT,    RIGHT_POSITION,   TURN_END,  0,                             RIGHT_ANGLE },      // シーン11 左折
    
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_BLUE_BLOCK,             LOW },              // シーン12 青丸まで
//     { TURN_LEFT,    RIGHT_POSITION,   TURN_END,  0,                             BLOCK_ANGLE },      // シーン13 左折【ブロック】

//     { LINE_TRACE,   RIGHT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,             BLOCK_BASE_SPEED }, // シーン14
//     { GO_TO_CIRCLE, RIGHT_POSITION,   END_RED,    0,                            BLOCK_BASE_SPEED }, // シーン15 赤丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,            BLOCK_BASE_SPEED }, // シーン16 丸まで直進

//     { LINE_TRACE,   RIGHT_POSITION,   END_RED,    END_LCL_ONE_SIDE,             BLOCK_BASE_SPEED }, // シーン17 赤丸まで
//     { TURN_RIGHT,   RIGHT_POSITION,   TURN_END,  0,                             RIGHT_ANGLE },      // シーン18 右折

//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,              BLOCK_BASE_SPEED }, // シーン19
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_YELLOW, 0,                             BLOCK_BASE_SPEED }, // シーン20 黄丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,             BLOCK_BASE_SPEED }, // シーン21 丸まで直進
//     { LINE_TRACE,   LEFT_POSITION,   END_YELLOW, END_LCL_ONE_SIDE,              BLOCK_BASE_SPEED }, // シーン22 黄丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                             RIGHT_ANGLE },      // シーン23 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_YELLOW, END_LCL_ONE_SIDE,              BLOCK_BASE_SPEED }, // シーン24 黄丸まで

//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                             LAST_ANGLE },       // シーン25 左折して直線なのでLAST_ANGLE
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER,  END_LCL_NO_LINE_CENTER,        BLOCK_BASE_SPEED }, // シーン26 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                             FIRST_ANGLE },      // シーン27 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,  END_LCL_NO_LINE,               BLOCK_BASE_SPEED }, // シーン28 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                             FIRST_ANGLE },      // シーン29
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,   0,                             BLOCK_BASE_SPEED }, // シーン30 ゴールまで
// };

// 3-3【済】
// static SceneInformation gBlockScenes3_3[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         DENGER_ANGLE },     // シーン0 赤丸からほぼ90度に旋回する
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER,  END_LCL_NO_LINE_CENTER+100, BLOCK_BASE_SPEED }, // シーン1 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                         DENGER_ANGLE2 },    // シーン2 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,  END_LCL_NO_LINE,           LOW },              // シーン3 何もないところを直進
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         FIRST_ANGLE },      // シーン4 曲がる

//     { LINE_TRACE,   LEFT_POSITION,   END_BLUE,   0,                         LOW },             // シーン5 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン6 直進のために左折
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン7 直進のための右折


//     { LINE_TRACE,   LEFT_POSITION,  END_RED,    0,                      LOW },              // シーン8 赤丸からスタート
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                      RIGHT_ANGLE },      // シーン9 右折
//     { LINE_TRACE,   LEFT_POSITION,  LOCALIZER,  END_LCL_ONE_SIDE,       LOW },              // シーン10 
//     { GO_TO_CIRCLE, LEFT_POSITION,  END_RED,    0,                      LOW },              // シーン11 赤丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,  LOCALIZER,  END_LCL_IN_CIRCLE,      LOW },              // シーン12 丸の中を直進 
//     // { LINE_TRACE,   LEFT_POSITION,  END_YELLOW, END_LCL_ONE_SIDE,       LOW },              // シーン13 黄丸まで【ブロック】
//     { LINE_TRACE,   RIGHT_POSITION, LOCALIZER,  END_LCL_ONE_SIDE,       BLOCK_BASE_SPEED },       // シーン17
//     { GO_TO_CIRCLE, RIGHT_POSITION, END_YELLOW,  0,                      BLOCK_BASE_SPEED },       // シーン18 黄色まで
//     { GO_IN_CIRCLE, RIGHT_POSITION, LOCALIZER,  END_LCL_IN_CIRCLE,      BLOCK_BASE_SPEED },       // シーン19 丸の中を直進
//     { LINE_TRACE,   RIGHT_POSITION, LOCALIZER,  END_LCL_ONE_SIDE,       BLOCK_BASE_SPEED },       // シーン17
//     { GO_TO_CIRCLE, RIGHT_POSITION, END_YELLOW,  0,                      BLOCK_BASE_SPEED },       // シーン18 黄色まで
//     { GO_IN_CIRCLE, RIGHT_POSITION, LOCALIZER,  END_LCL_IN_CIRCLE,      BLOCK_BASE_SPEED },       // シーン19 丸の中を直進

//     // { TURN_RIGHT,   LEFT_POSITION,  TURN_END,   0,                      BLOCK_ANGLE },      // シーン14 右折
//     // { LINE_TRACE,   LEFT_POSITION,  END_GREEN,  0,                      BLOCK_BASE_SPEED },       // シーン15 緑丸まで
//     // { TURN_LEFT,    LEFT_POSITION,  TURN_END,   0,                      RIGHT_ANGLE },      // シーン16 左折
    
//     // { LINE_TRACE,   RIGHT_POSITION, LOCALIZER,  END_LCL_ONE_SIDE,       BLOCK_BASE_SPEED },       // シーン17
//     // { GO_TO_CIRCLE, RIGHT_POSITION, END_GREEN,  0,                      BLOCK_BASE_SPEED },       // シーン18 緑丸まで
//     // { GO_IN_CIRCLE, RIGHT_POSITION, LOCALIZER,  END_LCL_IN_CIRCLE,      BLOCK_BASE_SPEED },       // シーン19 丸の中を直進
    
//     { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,  END_LCL_NO_LINE_CENTER+50, BLOCK_BASE_SPEED },       // シーン20 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                      FIRST_ANGLE },       // シーン21 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,  END_LCL_NO_LINE,        BLOCK_BASE_SPEED },       // シーン22 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION, TURN_END,   0,                      FIRST_ANGLE },      // シーン23
//     { LINE_TRACE,   LEFT_POSITION,  END_BLUE,   0,                      BLOCK_BASE_SPEED },       // シーン24 ゴールまで
// };

// 4-1青ダイヤパターン 【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          FIRST_ANGLE },      // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            LOW },              // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン2
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          LOW },              // シーン3 赤丸からスタート
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          RIGHT_ANGLE },      // シーン4 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          LOW },              // シーン5 赤丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          RIGHT_ANGLE },      // シーン6 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_BLUE,  END_LCL_BLUE_BLOCK,         LOW },              // シーン7 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                          BLOCK_ANGLE },      // シーン8 左折【ブロック】
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },       // シーン9 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                          BLOCK_BASE_SPEED },       // シーン10 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },       // シーン11 丸の中を直進
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },       // シーン12
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                          BLOCK_BASE_SPEED },       // シーン13 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },       // シーン14 丸の中を直進
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER, END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED },       // シーン15 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン16 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            BLOCK_BASE_SPEED },       // シーン17 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン18
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          BLOCK_BASE_SPEED },       // シーン19 ゴールまで
// };

// 4-2緑ダイヤパターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          FIRST_ANGLE },     // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,   END_LCL_NO_LINE,            LOW },             // シーン1 何もないところを直進
//     { TURN_LEFT,    LEFT_POSITION,  TURN_END,    0,                          FIRST_ANGLE },     // シーン2 曲がる
//     { LINE_TRACE,   LEFT_POSITION,  END_RED,     0,                          LOW },             // シーン3 赤丸まで
//     { TURN_LEFT,    LEFT_POSITION,  TURN_END,    0,                          GO_ANGLE },        // シーン4 直進のために左折
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          GO_ANGLE },        // シーン5 直進のための右折
//     { LINE_TRACE,   LEFT_POSITION,  END_RED,     END_LCL_ONE_SIDE,           LOW },             // シーン6 赤丸まで 
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          RIGHT_ANGLE },     // シーン7 右折
//     { LINE_TRACE,   LEFT_POSITION,  LOCALIZER,   END_LCL_ONE_SIDE,           LOW },             // シーン8
//     { GO_TO_CIRCLE, LEFT_POSITION,  END_RED,     0,                          LOW },             // シーン9 赤丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,  LOCALIZER,   END_LCL_IN_CIRCLE,          LOW },             // シーン10 丸の中を直進
//     { LINE_TRACE,   LEFT_POSITION,  END_YELLOW,  END_LCL_ONE_SIDE,           LOW },             // シーン11 黄丸まで 【ブロック】
//     { TURN_RIGHT,   LEFT_POSITION,  TURN_END,    0,                          BLOCK_BASE_SPEED },      // シーン12 右折
//     { LINE_TRACE,   LEFT_POSITION,  END_YELLOW,  END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン13
//     { TURN_LEFT,    LEFT_POSITION,  TURN_END,    0,                          RIGHT_ANGLE },     // シーン14 左折
//     { LINE_TRACE,   RIGHT_POSITION, LOCALIZER,   END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン15
//     { GO_TO_CIRCLE, RIGHT_POSITION, END_YELLOW,  END_LCL_ONE_SIDE,           BLOCK_BASE_SPEED },      // シーン16 黄丸まで 
//     { GO_IN_CIRCLE, RIGHT_POSITION, LOCALIZER,   END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED},       // シーン17 丸の中を直進
//     { GO_NO_LINE,   RIGHT_POSITION, LOCALIZER,   END_LCL_NO_LINE_CENTER+50,  BLOCK_BASE_SPEED },      // シーン18 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION, TURN_END,    0,                          FIRST_ANGLE },     // シーン19 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION, END_BLACK,   END_LCL_NO_LINE,            BLOCK_BASE_SPEED },      // シーン20 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION, TURN_END,    0,                          FIRST_ANGLE },     // シーン21
//     { LINE_TRACE,   LEFT_POSITION,  END_RED,     0,                          BLOCK_BASE_SPEED },      // シーン22 ゴールまで
// };


// // 4-3黄ダイヤパターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          FIRST_ANGLE },      // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            LOW },              // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン2  
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          LOW },              // シーン3 赤丸まで
//     { FIRE_TURN,    LEFT_POSITION,   TURN_END,  0,                          FIRE_ANGLE },       // シーン4 180度旋回する
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_FIRE_BLUE,          LOW },              // シーン5 青丸まで
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,  0,                          GO_ANGLE },         // シーン6 
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          GO_ANGLE },         // シーン7 
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_BLUE_BLOCK,         LOW },              // シーン8
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          RIGHT_ANGLE },      // シーン9 青丸左折
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           LOW },              // シーン10 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_BLUE,  0,                          LOW },              // シーン11 青丸まで直進 
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          LOW },              // シーン12 丸の中を直進 
//     { LINE_TRACE,   RIGHT_POSITION,  END_GREEN, 0,                          LOW },              // シーン13 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          LOW },              // シーン14 丸の中を直進
//     { LINE_TRACE,   RIGHT_POSITION,  END_GREEN, 0,                          BLOCK_BASE_SPEED },       // シーン15 緑丸まで 【ブロック】
//     { GO_NO_LINE,   LEFT_POSITION,   LOCALIZER, END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED },       // シーン16 何もないところを直進途中まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                          FIRST_ANGLE },      // シーン17 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            BLOCK_BASE_SPEED },       // シーン18 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン19
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          BLOCK_BASE_SPEED },       // シーン20 ゴールまで
// };


// // 5-1 黒ダイヤパターン
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         FIRST_ANGLE },     // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,  END_LCL_NO_LINE,           LOW },             // シーン1 何もないところを直進
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         FIRST_ANGLE },     // シーン2 曲がる
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,    0,                         LOW },             // シーン3 赤丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン4 直進のために左折
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         GO_ANGLE },        // シーン5 直進のための右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,    END_LCL_ONE_SIDE,          LOW },             // シーン6 赤丸まで 【ブロック】
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,   0,                         RIGHT_ANGLE },     // シーン7 右折
//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン8
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_RED,    0,                         BLOCK_BASE_SPEED },      // シーン9 赤丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },      // シーン10 丸まで直進
//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン11
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_YELLOW, 0,                         BLOCK_BASE_SPEED },      // シーン12 黄丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },      // シーン13 丸まで直進
//     { LINE_TRACE,   LEFT_POSITION,   END_YELLOW, END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン14 黄丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,   0,                         RIGHT_ANGLE },     // シーン15 右折
//     { LINE_TRACE,   LEFT_POSITION,   LOCALIZER,  END_LCL_ONE_SIDE,          BLOCK_BASE_SPEED },      // シーン16
//     { GO_TO_CIRCLE, LEFT_POSITION,   END_YELLOW, 0,                         BLOCK_BASE_SPEED },      // シーン17 黄丸まで
//     { GO_IN_CIRCLE, LEFT_POSITION,   LOCALIZER,  END_LCL_IN_CIRCLE,         BLOCK_BASE_SPEED },      // シーン18 丸まで直進
//     { LINE_TRACE,   LEFT_POSITION,   END_GREEN,  END_LCL_ONE_SIDE-50,       BLOCK_BASE_SPEED },      // シーン19 緑丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,   0,                         LAST_ANGLE },      // シーン20 左折して直線なのでLAST_ANGLE
//     { GO_NO_LINE,   RIGHT_POSITION,  LOCALIZER,  END_LCL_NO_LINE_CENTER,    BLOCK_BASE_SPEED },      // シーン15 何もないところを直進途中まで
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                         FIRST_ANGLE },     // シーン16 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK,  END_LCL_NO_LINE,           BLOCK_BASE_SPEED },      // シーン17 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,   0,                         FIRST_ANGLE },     // シーン18
//     { LINE_TRACE,   RIGHT_POSITION,  END_RED,    0,                         BLOCK_BASE_SPEED },      // シーン19 ゴールまで
// };

// 5-2黄ダイヤパターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                          FIRST_ANGLE },      // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            LOW },              // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン2  
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          LOW },              // シーン3 赤丸まで
//     { FIRE_TURN,    LEFT_POSITION,   TURN_END,  0,                          FIRE_ANGLE },       // シーン4 180度旋回する
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_FIRE_BLUE,          LOW },              // シーン5 青丸まで
//     { TURN_RIGHT,   RIGHT_POSITION,  TURN_END,  0,                          GO_ANGLE },         // シーン6 
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          GO_ANGLE },         // シーン7 
//     { LINE_TRACE,   RIGHT_POSITION,  END_BLUE,  END_LCL_BLUE_BLOCK,         LOW },              // シーン8
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          RIGHT_ANGLE },      // シーン9 青丸左折
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,           LOW },              // シーン10 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_BLUE,  0,                          BLOCK_BASE_SPEED },       // シーン11 青丸まで直進 
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },       // シーン12 丸の中を直進 【ブロック】
//     { LINE_TRACE,   RIGHT_POSITION,  END_GREEN, 0,                          BLOCK_BASE_SPEED },       // シーン13 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,          BLOCK_BASE_SPEED },       // シーン14 丸の中を直進
//     { LINE_TRACE,   RIGHT_POSITION,  END_GREEN, 0,                          BLOCK_BASE_SPEED },       // シーン15 緑丸まで
//     { GO_NO_LINE,   LEFT_POSITION,   LOCALIZER, END_LCL_NO_LINE_CENTER,     BLOCK_BASE_SPEED },       // シーン16 何もないところを直進途中まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                          FIRST_ANGLE },      // シーン17 少し旋回
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,            BLOCK_BASE_SPEED },       // シーン18 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                          FIRST_ANGLE },      // シーン19
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                          BLOCK_BASE_SPEED },       // シーン20 ゴールまで
// };

// 5-3青ダイヤパターン【済】
// static SceneInformation gBlockScenes[] = {
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                       FIRST_ANGLE },     // シーン0
//     { GO_NO_LINE,   RIGHT_POSITION,  END_BLACK, END_LCL_NO_LINE,         LOW },             // シーン1 何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                       FIRST_ANGLE },     // シーン2
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                       LOW },             // シーン3 赤丸からスタート
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                       RIGHT_ANGLE },     // シーン4 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   END_LCL_ONE_SIDE,        LOW },             // シーン5 赤丸まで
//     { TURN_RIGHT,   LEFT_POSITION,   TURN_END,  0,                       RIGHT_ANGLE },     // シーン6 右折
//     { LINE_TRACE,   LEFT_POSITION,   END_BLUE,  END_LCL_BLUE_BLOCK,      LOW },             // シーン7 青丸まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                       RIGHT_ANGLE },     // シーン8 左折
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,        LOW },             // シーン9 
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                       LOW },             // シーン10 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,       BLOCK_BASE_SPEED },      // シーン11 丸の中を直進 【ブロック】
//     { LINE_TRACE,   RIGHT_POSITION,  LOCALIZER, END_LCL_ONE_SIDE,        BLOCK_BASE_SPEED },      // シーン12
//     { GO_TO_CIRCLE, RIGHT_POSITION,  END_GREEN, 0,                       BLOCK_BASE_SPEED },      // シーン13 緑丸まで
//     { GO_IN_CIRCLE, RIGHT_POSITION,  LOCALIZER, END_LCL_IN_CIRCLE,       BLOCK_BASE_SPEED },      // シーン14 丸の中を直進
//     { GO_NO_LINE,   LEFT_POSITION,   LOCALIZER, END_LCL_NO_LINE_CENTER,  BLOCK_BASE_SPEED },      // シーン15 何もないところを直進途中まで
//     { TURN_LEFT,    LEFT_POSITION,   TURN_END,  0,                       FIRST_ANGLE },     // シーン16 少し旋回
//     { GO_NO_LINE,   LEFT_POSITION,   END_BLACK, END_LCL_NO_LINE,         BLOCK_BASE_SPEED },      // シーン17 黒線まで何もないところを直進
//     { TURN_LEFT,    RIGHT_POSITION,  TURN_END,  0,                       FIRST_ANGLE  },    // シーン18
//     { LINE_TRACE,   LEFT_POSITION,   END_RED,   0,                       BLOCK_BASE_SPEED },      // シーン19 ゴールまで
// };

#endif  // PATTERN_H_
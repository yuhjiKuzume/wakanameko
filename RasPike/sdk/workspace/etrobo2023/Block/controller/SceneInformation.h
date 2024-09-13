#ifndef SCENEINFORMATION_H_
#define SCENEINFORMATION_H_

#include "Sapporo.h"

// 実行するサービス
enum SceneCommands {
    LINE_TRACE = 0,
    CROSS_LINE = 1,
    GO_NO_LINE = 2,
    GO_TO_CIRCLE = 3,
    GO_IN_CIRCLE = 4,
    TURN_LEFT = 5,
    TURN_RIGHT = 6,
    FIRE_TURN = 7,
    GO_ZIG_ZAG = 8
};

// シーンの開始時の位置
enum SceneStartPostion {
    LEFT_POSITION = LEFT,
    RIGHT_POSITION = RIGHT
};

// 終了条件
enum SceneEndCondition {
    END_WHITE = 0,
    END_BLACK = 1,
    END_BLUE = 2,
    END_RED = 3,
    END_GREEN = 4,
    END_YELLOW = 5,
    CROSS_LINE_END = 10,
    LOCALIZER = 20,
    TURN_END = 30,
};

struct SceneInformation {
    SceneCommands command;
    SceneStartPostion startPositon;
    SceneEndCondition endCondition;
    int endLocalizerDistance;
    int detailCondition;
};

#endif /* SCENEINFORMATION_H_ */
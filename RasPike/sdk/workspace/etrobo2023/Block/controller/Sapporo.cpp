#include "Sapporo.h"
#include "LineTracerService.h"
#include "ColorCheckService.h"
#include "TurnService.h"
#include "SceneInformation.h"
#include "localizerTask.h"
#include "localizer.h"
#include "position.h"
#include "Clock.h"
#include "app.h"

#include "Pattern.h"

// デストラクタ問題の回避
// https://github.com/ETrobocon/etroboEV3/wiki/problem_and_coping
//void *_dso_handle = 0;

// // using宣言
using ev3api::Motor;
using ev3api::ColorSensor;
using ev3api::Clock;
Clock clock;
// // Device objects
// オブジェクトを静的に確保する
Motor gLeftWheel(PORT_C);
Motor gRightWheel(PORT_B);
Motor gArmMotor(PORT_A, false, MEDIUM_MOTOR);
ColorSensor gColorSensor(PORT_2);

// オブジェクトの定義
static LineTracerService *gLineTracerService;
static ColorCheckService *gColorCheckService;
static TurnService *gTurnService;
// 現在のシーン番号
int scene_count = 0;
double aim_list[5];
ColorLogic *color = new ColorLogic(gColorSensor);
// ブロック運びのシーンコピー先用
static SceneInformation gBlockScenes[100];
//ライントレースのスピードに関するパラメータ
int flag_stop = -1;//ブーストをかけるためのストップしてるかを判定するフラグ
int speed = 0;//スピード
int default_speed = 0;//ブーストをかけないフラグ
/**
 * EV3システム生成
 */
void user_system_create(int cose) {
    printf("sapporo_user_system_create");
    // オブジェクトの作成
    gLineTracerService = new LineTracerService(gLeftWheel, gRightWheel, gColorSensor);
    gColorCheckService = new ColorCheckService(gColorSensor);
    gTurnService = new TurnService(gLeftWheel, gRightWheel, gArmMotor, gColorSensor);

    // カラーセンサを下に向ける
    gArmMotor.setPWM(50);
    clock.sleep(500*1000);
    gArmMotor.setPWM(0);
    gLeftWheel.setBrake(true);
    gRightWheel.setBrake(true);

    // パターン決定
    if(cose == 1) {
        memcpy(gBlockScenes, gBlockScenes_red_and_blue_circle, sizeof(gBlockScenes_red_and_blue_circle));
        printf("パターン赤と青\n");
    } else if(cose == 2){
        memcpy(gBlockScenes, gBlockScenes_yellow_and_green_circle, sizeof(gBlockScenes_yellow_and_green_circle));
        printf("パターン黄と緑\n");
    }

    // 初期化完了通知
    ev3_led_set_color(LED_ORANGE);
}

/**
 * EV3システム破棄
 */
void user_system_destroy() {
    // printf("user_system_destroy_method_started %u\n",clock.now());
    gLeftWheel.reset();
    gRightWheel.reset();

    delete gLineTracerService;
    delete gColorCheckService;
    delete gTurnService;
    // printf("user_system_destroy_method_finished %u\n",clock.now());
}


/**
 * メインタスク
 */
// void main_task(intptr_t unused) {
//     user_system_create();  // センサやモータの初期化処理
//     // エンターキー押下
//     while (1) {
//         /* 読み込むパターンを変える！！ */
//         // センターボタン
//         if(ev3_button_is_pressed(ENTER_BUTTON)) {
//             memcpy(gBlockScenes, gBlockScenes_all_circle, sizeof(gBlockScenes_all_circle));
//             printf("パターン全てのサークル\n");
//             break;
//         }
//         // 左ボタン
//         else if(ev3_button_is_pressed(LEFT_BUTTON)) {
//             memcpy(gBlockScenes, gBlockScenes_red_and_blue_circle, sizeof(gBlockScenes_red_and_blue_circle));
//             printf("パターン赤と青\n");
//             break;
//         }
//         // 右ボタン
//         else if(ev3_button_is_pressed(RIGHT_BUTTON)) {
//             memcpy(gBlockScenes, gBlockScenes_yellow_and_green_circle, sizeof(gBlockScenes_yellow_and_green_circle));
//             printf("パターン黄と緑\n");
//             break;
//         }
//     }

//     // 終了条件用の周期タスクを起動
//     sta_cyc(CYC_COLOR_CHECK);
//     sta_cyc(LCL_CYC);

//     // ダブルループタスク起動
//     act_tsk(W_LOOP_TASK);

//     // メインタスクを待ち状態に
//     slp_tsk();

//     printf("w_loop終了!!\n");
//     scene_count = 0;
//     now_work = 2;

//     // ブロック運びタスク起動
//     long start_block = clock.now();
//     act_tsk(CARRY_BLOCK_TASK);
//     slp_tsk();
//     long end_block = clock.now() - start_block;
//     printf("block end time is %u\n", end_block);

//     user_system_destroy();  // 終了処理
//     ext_tsk();
// }

/**
 * ライントレースタスク
 */

void tracer_task_sapporo(intptr_t exinf) {
    int position = 0;

    position = gBlockScenes[scene_count].startPositon;
    speed = gBlockScenes[scene_count].detailCondition;

    //シーンが与えられたときの初期値をセットする
    if(flag_stop == STORE_SPEED){
        default_speed = speed;
    //加速する
    }else if(flag_stop == BOOST_SPEED){
        speed = speed + 8;
    //シーンが与えられた時の初期値に戻す
    }else if (flag_stop == INIT_SPEED){
        speed = default_speed;
    }

    // Lコースで右ポジションorRコースで左ポジション
    if((LR_FLAG == LEFT && position == RIGHT_POSITION) || (LR_FLAG == RIGHT && position == LEFT_POSITION)) {
        //左回転
        gLineTracerService->run(1, speed);
    // Lコースで左ポジションorRコースで右ポジション
    } else if((LR_FLAG == LEFT && position == LEFT_POSITION) || (LR_FLAG == RIGHT && position == RIGHT_POSITION)){
        // 右回転
        gLineTracerService->run(2, speed);
    }
    ext_tsk();
}


/**
 * 色判定タスク
 */
void color_check_task(intptr_t exinf) {
    // printf("color_check_task_started %u\n",clock.now());
    gColorCheckService->executeColorCheck();
    // printf("color_check_task_finished %u\n",clock.now());
    ext_tsk();
}


/**
走行距離推定
*/
Localizer localizer;
Position startPos;
void localizer_task(intptr_t exinf){
    // printf("localizer_task_started %u\n",clock.now());
    localizer_run();
    // printf("localizer_task_started %u\n",clock.now());
    ext_tsk();
}

/**
 * ブロック運搬タスク
 */
void carry_block_task(intptr_t exinf) {
    // printf("carry_block_task_started %u\n",clock.now());
    int end_flag = 0;
    int turn_flag = 0;

    // 基準スピード
    int baseSpeed;

    // 円の直径
    double circleDiameter = 60.0;
    double circleTocircle = 150.0;

    // 回転角の修正量に対する係数
    double k = 0.5;

    // 終了判定値の初期化
    double checkDistance = 0.0;
    int checkColor = 0;

    // ブロックエリア到達条件
    double distanceConditions = 60.0;

    // 中央値の算出に使う係数
    double half = 0.5;

    // 方位PIDに使う変数    
    double maxDireciton = 0.0;
    double minDirection = 0.0;
    double nowDirection = 0.0;
    double centerDirection = 0.0;
    int directionCounter = 0;
    double fixDirection = 0.0;
    double local_kyori[5]; 
    int localnum=0;
    int isStop = 1; 
    int stopnum = 15;
    int KEEP_FORWARD = 3;
    // メイン処理
    for (int i = 0; i < (sizeof(gBlockScenes)/sizeof(gBlockScenes[0])); i++) {
        end_flag = 0;
        double start_direction = 0;
        long int init_time = 0;

        printf("MOTHER NOW SCENE is %d\n", i);
        // printf("MOTHER END CONDITION is %d\n", gBlockScenes[i].endCondition);
        gLeftWheel.setPWM(0);
        gRightWheel.setPWM(0);
        clock.sleep(100*1000);

        // ライントレース走行
        if(gBlockScenes[i].command == LINE_TRACE) {
            flag_stop = -1;
            sta_cyc(CYC_TRACER);

            // 走行距離推定初期化
            localizer_setDistance();
            localizer_setDirection();
            maxDireciton = 0.0;
            minDirection = 0.0;
            nowDirection = 0.0;
            centerDirection = 0.0;
            directionCounter = 0;
            int counter = 0;
            int tmpListNum = 0;
            long preTime =  clock.now();
            long afterTime = 0;
            long tmpDirection = 0;
            // 終了確認
            while(1) {
                // printf("carry_block_task_linetrace_loop_called %u \n",clock.now());
                tslp_tsk(SLP_TIME);
                // counter++;
                // if(counter % 1000 == 0 ){
                    // printf("total distance : %lf\n", localizer_getDistance());
                    // printf("now scene is %d\n", j);
                    // printf("%u : get color service is %d\n",clock.now() ,color->checkColor());
                    // printf("endCondition is %d\n", gBlockScenes[j].endCondition);
                // }
                //clock.sleep(10*1000);
                nowDirection = localizer_getDirection();
                afterTime = clock.now();
                if(afterTime - preTime > 10*1000){             
                    local_kyori[localnum] = localizer_getDistance();
                    //printf("localnum %lf \n",local_kyori[localnum]);
                    localnum++;
                    //printf("localizer : %lf \n",localizer_getDirection());
                    preTime = afterTime;
                }
                if(localnum == stopnum){
                    localnum = 0;
                    // printf("kyori %lf %lf %lf %lf %lf\n",local_kyori[0],local_kyori[1],local_kyori[2],local_kyori[3],local_kyori[4]);
                    for(int j = 0;j < stopnum-1;j++ ){
                        //一定時間内に”1”以上進んでいれば、ブーストは行わない
                        if(local_kyori[j+1] - local_kyori[j] > 1){
                            isStop = 0;
                        }
                    }
                    //一度ブーストがかかったら、”3”進むまではブーストを継続する（ブロックをどかすのに時間がかかると思われるため）
                    //ブーストがかかっていないときはflag_stopは0なので、ブースト処理は実行されない
                    if(flag_stop == BOOST_SPEED){
                        if(nowDirection - tmpDirection < KEEP_FORWARD){
                            continue;
                        }
                    }                    
                    //ブースト処理を行う
                    if(isStop == 1) {
                        if(flag_stop == INIT_SPEED){
                            tmpDirection = nowDirection;
                        }
                        flag_stop = BOOST_SPEED;
                    }else{
                        isStop = 1;
                        flag_stop = INIT_SPEED;
                        for(int k = 0;k < stopnum;k++ ){
                            local_kyori[k] = k;
                        }                        
                    }
                }

                // nowDirectionが初めて0以外になった時
                if(nowDirection != aim_list[tmpListNum%5]){
                    tmpListNum++;
                    aim_list[tmpListNum%5] = nowDirection;
                }
                
                // printf("list %d %f %f %f %f %f %f\n",counter%5, nowDirection,aim_list[0],aim_list[1],aim_list[2],aim_list[3],aim_list[4]);

                if (gBlockScenes[i].endCondition == LOCALIZER) {
                    if(localizer_getDistance() >= gBlockScenes[i].endLocalizerDistance) {
                        end_flag = 1;
                    }
                }else if(gBlockScenes[i].endCondition == GREEN) {
                    if(localizer_getDistance() >= gBlockScenes[i].endLocalizerDistance && color->checkColor() == gBlockScenes[i].endCondition) {
                        end_flag = 1;
                    }
                }else if(gBlockScenes[i].endLocalizerDistance != 0) {
                    if(localizer_getDistance() >= gBlockScenes[i].endLocalizerDistance && gColorCheckService->getColor() == gBlockScenes[i].endCondition) {
                        end_flag = 1;
                    }
                } else if(gColorCheckService->getColor() == gBlockScenes[i].endCondition) {
                    end_flag = 1;
                }

                if (end_flag) {
                    // サービス停止
                    stp_cyc(CYC_TRACER);
                    // printf("%u : total distance : %lf\n", clock.now(), localizer_getDistance());
                    printf("%u : get color service is %d\n",clock.now() ,gColorCheckService->getColor());
                    gLeftWheel.setPWM(0);
                    gRightWheel.setPWM(0);
                    //centerDirection = (maxDireciton + minDirection) * half;
                    double ceter_num = 0;
                    for(int i = 0;i < 5; i++){
                        ceter_num =ceter_num +  aim_list[i];
                    }
                    // printf("方位 cnter : %f\n", ceter_num);
                    centerDirection = ceter_num / 5;
                    // printf("方位 目標方位値: %lf\n", centerDirection);
                    break;
                }
                // printf("carry_block_task_linetrace_loop_finished %u\n",clock.now());
            }

        // 何もないところを直進
        }else if(gBlockScenes[i].command == GO_NO_LINE) {
            // printf("carry_block_task_go_no_line_loop_started %u\n",clock.now());
            baseSpeed = gBlockScenes[i].detailCondition;
            
            // 丸を検知するまで方位直進
            localizer_setDistance();
            localizer_setDirection();
            // 方位のPIDで直進
            fixDirection = 0.0;
            init_time = clock.now();
            
            while(1) {
                // nowDirection = localizer_getDirection();
                fixDirection = localizer_getDirection();
                isStop = 1; 
                // fixDirection < 0 : 走行体が右向いている→左下げ、右上
                // fixDirection > 0 : 走行体が左向いている→右下げ、左上
                gLeftWheel.setPWM(baseSpeed + fixDirection);
                gRightWheel.setPWM(baseSpeed - fixDirection);
                clock.sleep(10*1000);
                local_kyori[localnum] = localizer_getDistance();
                // printf("localnum %lf \n",local_kyori[localnum]);
                localnum++;
                if(localnum == stopnum){
                    localnum = 0;
                    // printf("kyori %lf %lf %lf %lf %lf\n",local_kyori[0],local_kyori[1],local_kyori[2],local_kyori[3],local_kyori[4]);
                    for(int i = 0;i < stopnum-1;i++ ){
                        if(local_kyori[i+1] - local_kyori[i] > 0.00001){
                            isStop = 0;  
                        }
                    }
                    if(isStop == 1) {
                        baseSpeed = baseSpeed + 1;
                        // printf("booost\n");
                    }else{
                        isStop = 1;
                        for(int i = 0;i < stopnum;i++ ){
                            local_kyori[i] = i;
                        }                        
                    }
                }

                // 赤の外側の端から青の外側の端まで48cm
                checkDistance = localizer_getDistance();
                checkColor = color->checkColor();

                tslp_tsk(SLP_TIME);
                if (gBlockScenes[i].endCondition == LOCALIZER) {
                    if(localizer_getDistance() >= gBlockScenes[i].endLocalizerDistance) {
                        gLeftWheel.setPWM(0);
                        gRightWheel.setPWM(0);
                        // printf("ブロックエリア到達 検知した色: %d\n", checkColor);
                        break;
                    }
                } else if (localizer_getDistance() >= gBlockScenes[i].endLocalizerDistance) {
                    if(checkColor == RED || checkColor == BLACK || checkColor == BLUE){ 
                        gLeftWheel.setPWM(0);
                        gRightWheel.setPWM(0);
                        // printf("ブロックエリア到達 検知した色: %d\n", checkColor);
                        break;
                    }
                }
            }
            // printf("carry_block_task_go_no_line_loop_finished %u\n",clock.now());
        // 丸を検知するまで直進
        }else if(gBlockScenes[i].command == GO_TO_CIRCLE) {

            baseSpeed = gBlockScenes[i].detailCondition;
            
            // 丸を検知するまで方位直進
            localizer_setDistance();
            // nowDirection = 0.0;
            // 方位のPIDで直進
            fixDirection = 0.0;
            init_time = clock.now();
            double local_kyori[stopnum]; 
            int localnum=0;
            int isStop = 1;            
            while(1) {
                
                nowDirection = localizer_getDirection();
                fixDirection = centerDirection - nowDirection;

                // fixDirection < 0 : 走行体が右向いている→左下げ、右上
                // fixDirection > 0 : 走行体が左向いている→右下げ、左上
                gLeftWheel.setPWM(baseSpeed + fixDirection);
                gRightWheel.setPWM(baseSpeed - fixDirection);
                clock.sleep(10*1000);
                local_kyori[localnum] = localizer_getDistance();
                //printf("localnum %lf \n",local_kyori[localnum]);
                localnum++;
                if(localnum == stopnum){
                    localnum = 0;
                    // printf("kyori %lf %lf %lf %lf %lf\n",local_kyori[0],local_kyori[1],local_kyori[2],local_kyori[3],local_kyori[4]);
                    for(int i = 0;i < stopnum -1;i++ ){
                        if(local_kyori[i+1] - local_kyori[i] > 0.00001){
                            isStop = 0;  
                        }
                    }
                    if(isStop == 1) {
                        baseSpeed = baseSpeed + 1;
                        // printf("booost\n");
                    }else{
                        isStop = 1;
                        for(int i = 0;i < stopnum;i++ ){
                            local_kyori[i] = i;
                        }                        
                    }
                }                
                tslp_tsk(SLP_TIME);
                // 丸検知
                if(gBlockScenes[i].endCondition == color->checkColor()) {
                    gLeftWheel.setPWM(0);
                    gRightWheel.setPWM(0);
                    break;
                }
            }

        // 丸の中を直進
        }else if(gBlockScenes[i].command == GO_IN_CIRCLE) {
            
            // 丸を検知するまで方位直進
            localizer_setDistance();
            // 方位のPIDで直進
            fixDirection = 0.0;
            init_time = clock.now();
            double local_kyori[5]; 
            int localnum=0;
            int isStop = 1;
            while(1) {
                nowDirection = localizer_getDirection();
                fixDirection = centerDirection - nowDirection;

                // fixDirection < 0 : 走行体が右向いている→左下げ、右上
                // fixDirection > 0 : 走行体が左向いている→右下げ、左上
                gLeftWheel.setPWM(baseSpeed + fixDirection);
                gRightWheel.setPWM(baseSpeed - fixDirection);
                clock.sleep(10*1000);
                local_kyori[localnum] = localizer_getDistance();
                //printf("localnum %lf \n",local_kyori[localnum]);
                localnum++;
                if(localnum == stopnum){
                    localnum = 0;
                    // printf("kyori %lf %lf %lf %lf %lf\n",local_kyori[0],local_kyori[1],local_kyori[2],local_kyori[3],local_kyori[4]);
                    for(int i = 0;i < stopnum -1;i++ ){
                        if(local_kyori[i+1] - local_kyori[i] > 0.00001){
                            isStop = 0;  
                        }
                    }
                    if(isStop == 1) {
                        baseSpeed = baseSpeed + 1;
                        // printf("booost\n");
                    }else{
                        isStop = 1;
                        for(int i = 0;i < stopnum;i++ ){
                            local_kyori[i] = i;
                        }                        
                    }
                }

                // 一定距離
                if(localizer_getDistance() >= gBlockScenes[i].endLocalizerDistance) {
                    gLeftWheel.setPWM(0);
                    gRightWheel.setPWM(0);
                    // printf("10cm: %lf\n", localizer_getDistance());
                    // printf("10cm直進\n");
                    break;
                }
            }
        // 左旋回
        }else if(gBlockScenes[i].command == TURN_LEFT) {
            if (LR_FLAG == LEFT) {
                gTurnService->nice_turn(LEFT, gBlockScenes[i].detailCondition);
            // 右コースの場合右旋回
            } else if (LR_FLAG == RIGHT) {
                gTurnService->nice_turn(RIGHT, gBlockScenes[i].detailCondition);
            }     
        // 右旋回
        }else if(gBlockScenes[i].command == TURN_RIGHT) {
            if (LR_FLAG == LEFT) {
                gTurnService->nice_turn(RIGHT, gBlockScenes[i].detailCondition);
            // 右コースの場合左旋回
            } else if (LR_FLAG == RIGHT) {
                gTurnService->nice_turn(LEFT, gBlockScenes[i].detailCondition);
            }
     
        // 回転
        }else if(gBlockScenes[i].command == FIRE_TURN) {
            gTurnService->fire_turn(LR_FLAG);

        // ジグザグ直線
        }else if(gBlockScenes[i].command == GO_ZIG_ZAG) {
            if(LR_FLAG == LEFT) {
                gTurnService->zig_zag(gBlockScenes[i].startPositon);
            // 右コースの場合左右反転
            } else if(LR_FLAG == RIGHT) {
                if(gBlockScenes[i].startPositon == LEFT_POSITION) {
                    gTurnService->zig_zag(RIGHT_POSITION);
                } else {
                    gTurnService->zig_zag(LEFT_POSITION);
                }
            }
        }

        printf("--%d---シーン終了!\n", i);
        scene_count++;
    }
    wup_tsk(MAIN_TASK);
}

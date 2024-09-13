#ifdef __cplusplus
extern "C" {
#endif

#include "ev3api.h"

/* タスク優先度 */
#define MAIN_PRIORITY    (TMIN_APP_TPRI + 1) /* メインタスク */
#define TRACER_PRIORITY  (TMIN_APP_TPRI + 2) /* ライントレースタスク */

/* タスク周期の定義 */
#define LINE_TRACER_PERIOD  (10 * 1000) /* ライントレースタスク:10msec周期 */

/* センサーポートの定義 */
static const sensor_port_t
    touch_sensor    = EV3_PORT_1,
    color_sensor    = EV3_PORT_2,
    sonar_sensor    = EV3_PORT_3,
    gyro_sensor     = EV3_PORT_4;

/* モーターポートの定義 */
static const motor_port_t
    arm_motor       = EV3_PORT_A,
    left_motor      = EV3_PORT_C,
    right_motor     = EV3_PORT_B;

#ifndef STACK_SIZE
#define STACK_SIZE      (4096)
#endif /* STACK_SIZE */

#ifndef TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void tracer_task(intptr_t exinf);
extern void tracer_task_R(intptr_t exinf);

// 札幌モジュール追加
#define CARRY_BLOCK_PRIORITY    MAIN_PRIORITY + 1
#define TRACER_PRIORITY_SAPPORO MAIN_PRIORITY + 2
#define LOCALIZER_PRIORITY      MAIN_PRIORITY + 2
#define COLOR_CHECK_PRIORITY    MAIN_PRIORITY + 2

extern void carry_block_task(intptr_t exinf);
extern void tracer_task_sapporo(intptr_t exinf);
extern void color_check_task(intptr_t exinf);
extern void localizer_task(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif

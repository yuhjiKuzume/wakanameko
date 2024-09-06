#ifdef __cplusplus
extern "C" {
#endif

/* 円周率 */
#define PI 3.14159265358

/* 初期化 */
void Direction_init();

/* 方位を取得(右旋回が正転) */
float Direction_getDirection();

 // 方位を更新
void Direction_update();

#ifdef __cplusplus
}
#endif

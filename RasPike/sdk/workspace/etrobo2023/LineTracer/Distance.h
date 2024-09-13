#ifdef __cplusplus
extern "C" {
#endif

/* 円周率 */
#define PI 3.14159265358

/* 初期化関数 */
void Distance_init();

/* 距離を更新 */
void Distance_update();

/* 走行距離を取得 */
float Distance_getDistance();

/* 右タイヤの4ms間の距離を取得 */
float Distance_getDistance4msRight();

/* 左タイヤの4ms間の距離を取得 */
float Distance_getDistance4msLeft();

#ifdef __cplusplus
}
#endif

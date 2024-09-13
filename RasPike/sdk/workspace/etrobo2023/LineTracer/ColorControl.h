#ifdef __cplusplus
extern "C" {
#endif

// カラーセンサー取得
extern void set_colorsensor_value();
extern rgb_raw_t get_colorsensor_value();
extern void reset_previous_color();
// HSV
extern void convert_rgb_to_hsv();
extern bool_t check_Line_color_hsv(int16_t target_color_number);
extern int16_t is_change_line_color_hsv();

extern bool_t is_bule_Line(rgb_raw_t rgb_val);
extern int16_t is_change_line_color(rgb_raw_t rgb_val);
extern bool_t is_black_Line(rgb_raw_t rgb_val);

#ifdef __cplusplus
}
#endif

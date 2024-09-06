#ifdef __cplusplus
extern "C"
{
#endif

    extern int16_t steering_amount_pid_calculation(rgb_raw_t rgb_val, int target_color, int selected_pid_parameter);
    extern int16_t steering_amount_p_calculation(rgb_raw_t rgb_val);
    extern void initialize_pid_value();

#ifdef __cplusplus
}
#endif

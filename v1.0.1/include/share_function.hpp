#pragma once

#define TEXTURE_BACKGROUND (0)
#define TEXTURE_WIFI_ICON (1)
#define TEXTURE_BATTERY_LEVEL_ICON (2)
#define TEXTURE_BATTERY_CHARGE_ICON (3)
#define TEXTURE_CHINESE_FONT (4)
#define TEXTURE_ARABIC_FONT (5)
#define TEXTURE_ENGLISH_FONT (6)
#define TEXTURE_ARMENIAN_FONT (7)

#define STACKSIZE (24 * 1024)

extern bool share_ac_success;
extern bool share_apt_success;
extern bool share_mcu_success;
extern bool share_ptmu_success;
extern bool share_fs_success;
extern bool share_httpc_success;
extern bool share_ns_success;
extern bool share_ndsp_success;

extern bool share_key_A_press;
extern bool share_key_B_press;
extern bool share_key_X_press;
extern bool share_key_Y_press;
extern bool share_key_DRIGHT_press;
extern bool share_key_DLEFT_press;
extern bool share_key_ZL_press;
extern bool share_key_ZR_press;
extern bool share_key_START_press;
extern bool share_key_SELECT_press;
extern bool share_key_touch_press;
extern bool share_key_A_held;
extern bool share_key_B_held;
extern bool share_key_X_held;
extern bool share_key_Y_held;
extern bool share_key_CPAD_UP_held;
extern bool share_key_CPAD_DOWN_held;
extern bool share_key_CPAD_RIGHT_held;
extern bool share_key_CPAD_LEFT_held;
extern bool share_key_DUP_held;
extern bool share_key_DDOWN_held;
extern bool share_key_DRIGHT_held;
extern bool share_key_DLEFT_held;
extern bool share_key_touch_held;

extern bool share_menu_main_run;
extern bool share_setting_main_run;
extern bool share_speed_test_main_run;
extern bool share_speed_test_thread_suspend;
extern bool share_speed_test_already_init;
extern bool share_speed_test_thread_run;
extern bool share_connect_test_succes;
extern bool share_connect_test_thread_run;
extern bool share_google_tr_tr_thread_run;
extern bool share_google_tr_thread_suspend;
extern bool share_google_tr_already_init;
extern bool share_google_tr_main_run;
extern bool share_update_thread_run;
extern bool share_hid_thread_run;
extern bool share_app_logs_show;
extern bool share_system_setting_menu_show;
extern bool share_line_setting_menu_show;
extern bool share_wifi_enabled;
extern bool share_disabled_enter_afk_mode;
extern bool hid_disabled;

extern bool share_line_already_init;
extern bool share_line_main_run;
extern bool share_line_thread_suspend;
extern bool line_update_detail_show;
extern bool line_frame_time_show;
extern bool line_auto_update_mode;
extern bool night_mode;
extern bool draw_vsync_mode;
extern bool line_message_select_mode;
extern int language_select_num;
extern int message_select_num;
extern float text_x_cache;
extern float text_y_cache;
extern float text_size_cache;
extern float text_interval_cache;
extern float app_log_x_cache;


extern u64 share_thread_exit_time_out;
extern u32 share_connect_test_response_code;
extern u8 share_wifi_signal;
extern u8 share_battery_level;
extern u8 share_battery_charge;
extern u8* wifi_state;
extern u8* wifi_state_internet_sample;

extern int share_afk_time;
extern int share_afk_lcd_bridgeness;
extern int share_app_log_num;
extern int share_app_log_view_num_cache;
extern int share_fps_show;
extern int share_hours;
extern int share_minutes;
extern int share_seconds;
extern int share_day;
extern int share_month;
extern int share_held_time;
extern int share_touch_pos_x;
extern int share_touch_pos_y;
extern int share_touch_pos_x_before;
extern int share_touch_pos_x_moved;
extern int share_touch_pos_x_move_left;
extern int share_touch_pos_y_before;
extern int share_touch_pos_y_moved;
extern int share_touch_pos_y_move_left;

extern int share_app_log_view_num;
extern int share_fps;
extern int share_lcd_bridgeness;
extern int share_time_to_enter_afk;
extern float share_frame_time_point[270];
extern float share_drawing_time_history[120];
extern float share_app_log_x;
extern float share_drawing_time;
extern double share_scroll_speed;
extern double share_app_up_time_ms;

extern char share_status[100];
extern char share_swkb_input_text[8192];
extern char* share_connected_ssid;
extern std::string share_app_logs[4096];
extern std::string share_clipboard[10];
extern std::string share_bot_button_string;
extern std::string share_square_string;
extern std::string share_circle_string;
extern std::string share_test_string;
extern std::string share_connect_check_url;
extern std::string share_battery_level_string;
extern std::string share_setting[10];
extern std::string share_speed_test_ver;
extern std::string share_google_translation_ver;
extern std::string share_line_ver;
extern std::string share_app_ver;

/*

extern bool share_speed_test_start_request;
extern int speed_test_data_size;
extern int share_speed_test_total_download_size;
extern int share_speed_test_download_size[400];
extern float share_speed_test_total_download_time;
extern float share_speed_test_download_time[400];
extern float share_speed_test_result;
extern std::string speed_test_data_size_string[8];
*/

extern touchPosition touch_pos;
extern Result share_function_result;
extern TickCounter share_tick_counter_up_time, share_total_frame_time, share_tick_counter_cpu_time;
extern httpcContext share_connect_test_httpc;
extern Thread share_connect_test_thread;
extern SwkbdState share_swkb;
extern SwkbdLearningData share_swkb_learn_data;
extern SwkbdDictWord share_swkb_words[8];
extern SwkbdButton share_swkb_press_button;

void Share_connectivity_check_thread(void* arg);

void Share_get_system_info(void);

bool Share_exit_check(void);

void Share_update_thread(void* arg);

void Share_app_log_draw(void);

int Share_app_log_save(std::string type, std::string text, Result result, bool draw);

void Share_app_log_add_result(int add_log_num, std::string add_text, Result result, bool draw);

void Share_key_flag_reset(void);

void Share_scan_hid(void* arg);

#pragma once
#include "citro2d.h"

#define STACKSIZE (24 * 1024)

struct Result_with_string
{
	std::string string;
	Result code;
};

//Svc init result flag
extern bool share_ac_success;
extern bool share_apt_success;
extern bool share_mcu_success;
extern bool share_ptmu_success;
extern bool share_fs_success;
extern bool share_httpc_success;
extern bool share_rom_success;
extern bool share_cfg_success;
extern bool share_ndsp_success;

//Key flag
extern bool share_key_A_press;
extern bool share_key_B_press;
extern bool share_key_X_press;
extern bool share_key_Y_press;
extern bool share_key_CPAD_UP_press;
extern bool share_key_CPAD_DOWN_press;
extern bool share_key_CPAD_RIGHT_press;
extern bool share_key_CPAD_LEFT_press;
extern bool share_key_DUP_press;
extern bool share_key_DDOWN_press;
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
extern bool share_key_L_held;
extern bool share_key_R_held;
extern bool share_key_touch_held;

//Already init flag
extern bool share_google_tr_already_init;
extern bool share_image_viewer_already_init;
extern bool share_line_already_init;
extern bool share_speed_test_already_init;

//App run flag
extern bool share_google_tr_main_run;
extern bool share_image_viewer_main_run;
extern bool share_line_main_run;
extern bool share_menu_main_run;
extern bool share_setting_main_run;
extern bool share_speed_test_main_run;

//Thread run flag
extern bool share_connect_test_thread_run;
extern bool share_google_tr_tr_thread_run;
extern bool share_hid_thread_run;
extern bool share_image_viewer_download_thread_run;
extern bool share_image_viewer_parse_thread_run;
extern bool share_line_log_download_thread_run;
extern bool share_line_log_parse_thread_run;
extern bool share_line_message_send_thread_run;
extern bool share_line_update_thread_run;
extern bool share_speed_test_thread_run;
extern bool share_update_thread_run;

//Thread suspend flag
extern bool share_google_tr_thread_suspend;
extern bool share_image_viewer_thread_suspend;
extern bool share_line_thread_suspend;
extern bool share_speed_test_thread_suspend;

extern bool share_allow_send_app_info;
extern bool share_debug_mode;
extern bool share_connect_test_succes;
extern bool share_app_logs_show;
extern bool share_wifi_enabled;
extern bool share_disabled_enter_afk_mode;
extern bool share_night_mode;
extern bool share_draw_vsync_mode;
extern bool share_hid_disabled;
extern bool share_system_setting_menu_show;

extern bool line_auto_update_mode;
extern int language_select_num;
extern float message_select_num;
extern float text_x_cache;
extern float text_y_cache;
extern float text_size_cache;
extern float text_interval_cache;
extern float app_log_x_cache;

extern u8 share_wifi_signal;
extern u8 share_battery_level;
extern u8 share_battery_charge;
extern u8* wifi_state;
extern u8* wifi_state_internet_sample;
extern u32 share_connect_test_response_code;
extern u64 share_thread_exit_time_out;

extern int share_afk_time;
extern int share_afk_lcd_brightness;
extern int share_app_log_num;
extern int share_app_log_view_num_cache;
extern int share_num_of_app_start;
extern int share_fps_show;
extern int share_free_ram;
extern int share_hours;
extern int share_minutes;
extern int share_seconds;
extern int share_day;
extern int share_month;
extern int share_image_viewer_image_pos_x;
extern int share_image_viewer_image_pos_y;
extern int share_line_menu_mode;
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
extern int share_lcd_brightness;
extern int share_time_to_enter_afk;
extern float share_image_viewer_image_zoom;
extern float share_setting_menu_y_offset;
extern float share_frame_time_point[270];
extern float share_app_log_x;
extern float share_drawing_time;
extern double share_scroll_speed;
extern double share_app_up_time_ms;

extern char share_status[100];
extern char share_swkb_input_text[8192];
extern std::string share_app_logs[4096];
extern std::string share_clipboard[15];
extern std::string share_bot_button_string;
extern std::string share_square_string;
extern std::string share_circle_string;
extern std::string share_connect_check_url;
extern std::string share_battery_level_string;
extern std::string share_setting[10];
extern std::string share_line_message_en[19];
extern std::string share_line_message_jp[19];
extern std::string share_speedtest_message_en[12];
extern std::string share_speedtest_message_jp[12];
extern std::string share_image_viewer_message_en[8];
extern std::string share_image_viewer_message_jp[8];
extern std::string share_setting_menu_message_en[20];
extern std::string share_setting_menu_message_jp[20];
extern std::string share_speed_test_ver;
extern std::string share_google_translation_ver;
extern std::string share_image_viewer_ver;
extern std::string share_line_ver;
extern std::string share_app_ver;
extern std::string share_httpc_user_agent;

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

extern C2D_SpriteSheet Background_texture, Wifi_icon_texture, Battery_level_icon_texture, Battery_charge_icon_texture, Square_texture, Chinese_font_texture, Arabic_font_texture, Armenian_font_texture, English_font_texture, Punctuation_close_font_texture;
extern C2D_Image Background_image[2], Wifi_icon_image[9], Battery_level_icon_image[21], Battery_charge_icon_image[1], Square_image[14], Chinese_font_image[2], Arabic_font_image[255], Armenian_font_image[92], English_font_image[96], Punctuation_close_font_image[93];
extern touchPosition touch_pos;
extern Result share_function_result;
extern TickCounter share_tick_counter_up_time, share_total_frame_time, share_tick_counter_cpu_time;
extern httpcContext share_connect_test_httpc;
extern Thread share_connect_test_thread;
extern SwkbdState share_swkb;
extern SwkbdLearningData share_swkb_learn_data;
extern SwkbdDictWord share_swkb_words[8];
extern SwkbdButton share_swkb_press_button;

void Share_send_app_info(void* arg);

void Share_connectivity_check_thread(void* arg);

void Share_get_system_info(void);

int Share_free_ram(void);

bool Share_exit_check(void);

void Share_update_thread(void* arg);

void Share_app_log_draw(void);

int Share_app_log_save(std::string type, std::string text, Result result, bool draw);

void Share_app_log_add_result(int add_log_num, std::string add_text, Result result, bool draw);

void Share_key_flag_reset(void);

void Share_scan_hid(void* arg);

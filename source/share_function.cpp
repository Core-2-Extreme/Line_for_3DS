#include <3ds.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include "change_setting.hpp"
#include "share_function.hpp"
#include "draw.hpp"
#include "line.hpp"
#include "image_viewer.hpp"

//Svc init result flag
bool s_ac_success = false;
bool s_apt_success = false;
bool s_am_success = false;
bool s_mcu_success = false;
bool s_ptmu_success = false;
bool s_fs_success = false;
bool s_httpc_success = false;
bool s_rom_success = false;
bool s_cfg_success = false;

//Key flag
bool s_key_A_press = false;
bool s_key_B_press = false;
bool s_key_X_press = false;
bool s_key_Y_press = false;
bool s_key_CPAD_UP_press = false;
bool s_key_CPAD_DOWN_press = false;
bool s_key_CPAD_RIGHT_press = false;
bool s_key_CPAD_LEFT_press = false;
bool s_key_DUP_press = false;
bool s_key_DDOWN_press = false;
bool s_key_DRIGHT_press = false;
bool s_key_DLEFT_press = false;
bool s_key_L_press = false;
bool s_key_R_press = false;
bool s_key_ZL_press = false;
bool s_key_ZR_press = false;
bool s_key_START_press = false;
bool s_key_SELECT_press = false;
bool s_key_touch_press = false;
bool s_key_A_held = false;
bool s_key_B_held = false;
bool s_key_X_held = false;
bool s_key_Y_held = false;
bool s_key_CPAD_UP_held = false;
bool s_key_CPAD_DOWN_held = false;
bool s_key_CPAD_RIGHT_held = false;
bool s_key_CPAD_LEFT_held = false;
bool s_key_DUP_held = false;
bool s_key_DDOWN_held = false;
bool s_key_DRIGHT_held = false;
bool s_key_DLEFT_held = false;
bool s_key_L_held = false;
bool s_key_R_held = false;
bool s_key_ZL_held = false;
bool s_key_ZR_held = false;
bool s_key_touch_held = false;

//Already init flag
bool s_gtr_already_init = false;
bool s_imv_already_init = false;
bool s_line_already_init = false;
bool s_spt_already_init = false;
bool s_sem_already_init = false;

//App run flag
bool s_gtr_main_run = false;
bool s_imv_main_run = false;
bool s_line_main_run = false;
bool s_menu_main_run = true;
bool s_sem_main_run = false;
bool s_spt_main_run = false;

//Thread run flag
bool s_connect_test_thread_run = false;
bool s_gtr_tr_thread_run = false;
bool s_hid_thread_run = false;
bool s_imv_download_thread_run = false;
bool s_imv_parse_thread_run = false;
bool s_line_log_download_thread_run = false;
bool s_line_log_load_thread_run = false;
bool s_line_log_parse_thread_run = false;
bool s_line_send_message_thread_run = false;
bool s_line_update_thread_run = false;
bool s_spt_thread_run = false;
bool s_update_thread_run = false;
bool s_update_check_thread_run = false;
bool s_launch_app_thread_run = false;
bool s_destroy_app_thread_run = false;

//Thread suspend flag
bool s_gtr_thread_suspend = false;
bool s_imv_thread_suspend = false;
bool s_line_thread_suspend = false;
bool s_spt_thread_suspend = false;
bool s_sem_thread_suspend = false;

bool s_allow_send_app_info = false;
bool s_debug_mode = false;
bool s_connect_test_succes = false;
bool s_debug_slow = false;
bool s_gtr_type_text_request = false;
bool s_use_specific_system_font = false;
bool s_use_external_font[47];
bool s_imv_adjust_url_request = false;
bool s_imv_image_dl_request = false;
bool s_imv_image_parse_request = false;
bool s_line_auto_update_mode = false;
bool s_line_hide_id = false;
bool s_line_log_load_request = false;
bool s_line_log_update_request = false;
bool s_line_send_message_check[2] = { false, false };
bool s_line_send_message_request = false;
bool s_line_send_sticker_request = false;
bool s_line_select_sticker_request = false;
bool s_line_type_id_request = false;
bool s_line_type_app_ps_request = false;
bool s_line_type_message_request = false;
bool s_line_type_main_url_request = false;
bool s_line_type_script_ps_request = false;
bool s_app_logs_show = false;
bool s_wifi_enabled = false;
bool s_disabled_enter_afk_mode = false;
bool s_night_mode = false;
bool s_draw_vsync_mode = true;
bool s_hid_disabled = true;
bool s_system_setting_menu_show = false;
bool s_flash_mode = false;
bool s_sem_update_check_request = false;
bool s_sem_show_newest_ver_data = false;
bool s_sem_select_ver = false;
bool s_sem_available_ver[8]; // 3dsx, 32mb, 64mb, 72mb, 80mb, 96mb, 124mb, 178mb
bool s_sem_file_download_request = false;
bool s_spt_start_request = false;
bool s_error_display = false;

float message_select_num = 0.0f;
float text_x_cache = 40.0;
float text_y_cache = 0.0;
float text_size_cache = 0.66;
float text_interval_cache = 35.0;
float app_log_x_cache = 0.0;
float s_line_bottom_y = 0.0;
float s_gtr_text_x = 0.0f;

u8 s_wifi_signal = -1;
u8 s_battery_level = -1;
u8 s_battery_charge = -1;
u8* wifi_state;
u8* wifi_state_internet_sample;

int s_afk_time = 0;
int s_afk_lcd_brightness = 0;
int s_app_log_num = 0;
int s_app_log_view_num_cache = 0;
int s_num_of_app_start = 0;
int s_fps_show = 0;
int s_free_ram = 0;
int s_free_linear_ram = 0;
int s_gtr_selected_history_num = 0;
int s_hours = -1;
int s_minutes = -1;
int s_seconds = -1;
int s_days = -1;
int s_months = -1;
int s_imv_image_pos_x = 50;
int s_imv_image_pos_y = 20;
int s_imv_clipboard_select_num = 1;
int s_lang_select_num = 0;
int s_line_menu_mode = 0;
int s_line_room_select_num = 0;
int s_line_sticker_tab_select_num = 0;
int s_line_sticker_select_num = 0;
int s_held_time = 0;
int s_touch_pos_x = 0;
int s_touch_pos_y = 0;
int s_touch_pos_x_before = 0;
int s_touch_pos_x_moved = 0;
int s_touch_pos_y_before = 0;
int s_touch_pos_y_moved = 0;
int s_sem_help_mode_num = -1;
int s_app_log_view_num = 0;
int s_fps;
int s_lcd_brightness = 50;
int s_time_to_enter_afk = 300;
int s_current_app_ver = 9;
int s_current_gas_ver = 3;
int s_sem_selected_edition_num = 0;
int s_line_log_httpc_buffer_size = 0x200000;
int s_line_log_fs_buffer_size = 0x200000;
int s_spt_spt_httpc_buffer_size = 0x700000;
int s_imv_image_httpc_buffer_size = 0x200000;
int s_spt_data_size = 0;
/*
   0 ~   94   (95) Basic latin
  95 ~  222  (128) Latin 1 supplement
 223 ~  314   (92) Ipa extensions
 315 ~  394   (80) Spacing modifier letters
 395 ~  506  (112) Combining diacritical marks
 507 ~  641  (135) Greek and coptic
 642 ~  897  (256) Cyrillic
 898 ~  945   (48) Cyrillic supplement
 946 ~ 1037   (91) Armenian
1038 ~ 1125   (88) Hebrew
1126 ~ 1390  (255) Arabic
1391 ~ 1508  (128) Devanagari
1509 ~ 1587   (79) Gurmukhi
1588 ~ 1659   (72) Tamil
1660 ~ 1755   (96) Telugu
1756 ~ 1844   (88) Kannada
1845 ~ 1934   (90) Sinhala
1935 ~ 2021   (87) Thai
2022 ~ 2088   (67) Lao
2089 ~ 2299  (211) Tibetan
2300 ~ 2387   (88) Georgian
2388 ~ 3027  (640) Unified canadian aboriginal syllabics
3028 ~ 3155  (128) Phonetic extensions
3156 ~ 3218   (63) Combining diacritical marks supplement
3219 ~ 3451  (233) Greek extended
3452 ~ 3562  (111) General punctuation
3563 ~ 3604   (42) Superscripts and subscripts
3605 ~ 3637   (33) Combining diacritical marks for symbols
3638 ~ 3749  (112) Arrows
3750 ~ 4005  (256) Mathematical operators
4006 ~ 4261  (256) Miscellaneous technical
4262 ~ 4272   (11) Optical character recognition
4273 ~ 4400  (128) Box drawing
4401 ~ 4432   (32) Block elements
4433 ~ 4528   (96) Geometric shapes
4529 ~ 4784  (256) Miscellaneous_symbols
4785 ~ 4976  (192) Dingbats
4977 ~ 5040   (64) Cjk symbol and punctuation
5041 ~ 5133   (93) Hiragana
5134 ~ 5229   (96) Katakana
5230 ~ 5323   (94) Hangul compatibility jamo
5324 ~ 6488 (1165) Yi syllables
6489 ~ 6543   (55) Yi radicals
6544 ~ 6575   (32) Cjk compatibility forms
6576 ~ 6800  (225) Halfwidth and fullwidth forms
6801 ~ 7568  (768) Miscellaneous symbols and pictographs
*/
int s_font_characters[46] = {
  95, 128,  92,  80, 112, 135, 256,  48,  91,  88,  255, 128,  79,  72,  96,  88,
  90,  87,  67, 211,  88, 640, 128,  63, 233,  111,  42,  33, 112, 256, 256,  11,
 128,  32,  96, 256, 192,  64,  93,  96,  94, 1165,  55,  32, 225, 768,
};
int s_font_start_num[46] = {
	0,   95,  223,  315,  395,  507,  642,  898,  946, 1038, 1126, 1391, 1509, 1588, 1660, 1756,
 1845, 1935, 2022, 2089, 2300, 2388, 3028, 3156, 3219, 3452, 3563, 3605, 3638, 3750, 4006, 4262,
 4273, 4401, 4433, 4529, 4785, 4977, 5041, 5134, 5230, 5324, 6489, 6544, 6576, 6801,
};
float s_touch_pos_x_move_left = 0;
float s_touch_pos_y_move_left = 0;
float s_imv_image_zoom = 1.0;
float s_sem_y_offset = 0;
float s_frame_time;
float s_app_log_x = 0.0;
//#0600~#06FF
float s_arabic_font_interval[255] = {
 16, 23, 10, 22, 22, 18, 11, 11, 13, 11, 15, 5,  2,  4, 12, 9,
  5,  4, 5, 6, 6,  5, 7,  4,  3,  2,  3,  2, 18,      4, 8,
 11, 8, 5,  3, 10,  3, 12,  1, 12, 7, 12, 12, 9, 9, 9, 6,
 6, 8, 8, 19, 19, 22, 22, 13, 13, 9, 9, 14, 14, 11, 11, 11,
 5, 15, 13, 12, 11, 9, 11, 7, 10, 12, 12, 5, 5, 5, 5, 5,
 5, 5,  4, 6,  4,  4,  1, 6,  2,  4, 5,  3,  2,  3,  3,  3,
  4,  3, 7, 8, 7, 7, 8, 8, 8, 8, 9,  4,  2, 7, 13, 10,
  1, 5,  4,  4,  3, 5, 12, 12, 14, 12, 12, 12, 12, 12, 12, 12,
 12, 9, 9, 9, 9, 9, 9, 9, 6, 6, 6, 6, 6, 6, 6, 6,
 6, 8, 8, 8, 8, 9, 8, 8, 8, 8, 19, 19, 19, 22, 22, 13,
 9, 15, 15, 15, 15, 15, 15, 13, 13, 14, 17, 14, 12, 12, 12, 14,
 14, 14, 14, 14, 14, 11, 11, 11, 8, 10, 11, 11, 11, 11, 11, 8,
 7, 7, 7, 7, 10, 10, 10, 10, 10, 10, 10, 10, 12, 15, 12, 8,
 12, 12, 11, 11, 5, 7, 12, 9,  5, 5, 6, 6, 12, 10, 15,  2,
  2, 6, 5, 12,  3, 6, 7, 7, 6, 8,  3,  3,  3, 5, 5, 8,
  4,  3, 7, 8, 8, 8, 6, 8, 8, 8, 16, 18, 7,  4, 5, 7,
};
//#0531~#058
float s_armenian_font_interval[91] = {
	 11, 7, 10, 10, 8, 8, 7, 7, 11, 10, 9, 6, 9, 12, 8,
 7, 9, 10, 8, 11, 10, 10, 11, 7, 8, 10, 8, 10, 8, 11, 7,
 7, 7, 9, 9, 11, 8, 12,          2,  2,  2,  5,  2,  5, 6,
 15, 10, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  5, 8, 7, 7,
 7, 7, 7, 7, 7,  3, 6, 6, 7,  4, 10, 7, 7, 7, 7, 10,
 7, 7,  5, 7, 5, 5, 8, 8, 12,  2, 8,         11, 11, 9,
};
//#2500~#257F
float s_box_drawing_font_interval[128] = {
 12, 12,  2,  4, 12, 12,  2,  4, 12, 12,  2,  4, 7, 7, 8, 8,
 7, 7, 8, 8, 7, 7, 8, 8, 7, 7, 8, 8, 7, 7, 8, 8,
 8, 8, 8, 8, 7, 7, 8, 8, 8, 8, 8, 8, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  2,  4,
 12,  5, 7, 8, 8, 7, 8, 8, 7, 8, 8, 7, 8, 8, 7, 8,
 8, 7, 8, 8, 12, 12, 12, 12, 12, 12, 12, 12, 12, 7, 7, 7,
 7, 11, 11, 11, 5,  2, 5,  2, 5,  4, 5,  4, 12,  4, 12,  4,
};
//#3000~#303F
float s_cjk_symbol_and_punctuation_font_interval[64] = {
 25,  5, 5, 7, 16, 12, 13, 15, 6, 6, 8, 8, 7, 7, 7, 7,
 6, 6, 12, 13, 6, 6, 7, 7,  5,  5, 5, 5, 17, 7, 7, 7,
 15,  2, 6, 9, 12, 9, 12, 12, 12, 12, 5, 5, 5, 5,  3,  3,
 16, 8, 12, 11, 14, 9, 15, 16, 16, 16, 16, 6, 13, 13, 17, 10,
};
//
float s_devanagari_font_interval[128] = {
 7, 9, 8, 14, 12, 13, 16, 10, 10, 11, 13, 18, 13, 12, 12, 12,
 12, 16, 16, 16, 16, 13, 14, 11, 12, 12, 13, 13, 13, 13, 13, 10,
 10, 11, 10, 13, 11, 11, 10, 11, 11, 11, 11, 13, 11, 12, 12, 11,
 9, 9, 13, 14, 14, 11, 12, 12, 12, 10,  2,  5, 8, 7, 16, 16,
 16, 9, 11, 9, 9, 9, 9, 9, 8, 16, 16, 16, 16, 10,  3, 10,
 15, 8, 12, 8, 8, 7, 8, 8, 13, 14, 11, 13, 11, 10, 13, 11,
 17, 13, 10, 11,  2, 5, 7, 5, 8, 7, 11, 8, 10, 10, 9, 8,
  5,  3, 12, 13, 16, 16, 13, 13, 18, 13, 11, 12, 15, 8, 14, 14,
};
//#2700~#27BF
float s_dingbats_font_interval[192] = {
 18, 16, 16, 16, 18, 15, 12, 14, 14, 12, 13, 13, 9, 14, 15, 16,
 15, 16, 16, 13, 14, 13, 13, 11, 12, 13, 13, 13, 13, 8, 10, 9,
 9, 12, 14, 14, 14, 13, 13, 13, 17, 14, 14, 14, 14, 14, 14, 14,
 15, 13, 13, 12, 13, 13, 12, 13, 13, 13, 13, 12, 12, 12, 13, 14,
 14, 13, 13, 12, 12, 12, 13, 14, 13, 12, 12, 14, 11, 15, 13, 13,
 13, 13, 13, 12, 12, 6, 13, 5,  2,  4, 7, 5, 5, 11, 11, 5,
 11, 11, 8, 9, 15, 12, 12, 14, 6, 6, 7, 7, 6, 6, 8, 8,
 7, 7,  5,  5, 6, 6, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 16, 13, 13, 13, 11, 16, 9, 16, 16, 14, 14, 14,
 15, 15, 16, 16, 14, 16, 16, 7, 16, 15, 14, 15, 15, 12, 12, 15,
 14, 15, 14, 17, 13, 15, 13, 13, 16, 13, 14, 15, 14, 17, 16, 21,
};
//#0020~#007F
float s_basic_latin_font_interval[96] = { 13,
 5,  2, 5, 10, 9, 14, 11,  2,  4,  4, 6, 9,  2, 5,  2, 5,
 9,  5, 9, 9, 9, 9, 9, 9, 9, 9,  2,  2, 9, 9, 9, 9,
 17, 13, 10, 11, 11, 10, 9, 12, 10,  2, 7, 11, 8, 12, 10, 13,
 10, 13, 11, 10, 10, 10, 12, 17, 12, 12, 10,  4, 5,  4, 8, 9,
  3, 9, 8, 8, 8, 9, 6, 8, 8,  2,  4, 8,  2, 13, 8, 9,
 8, 8, 5, 8,  5, 8, 9, 13, 9, 9, 8, 5,  2, 5, 9,
};
//#25A0~#25F0
float s_geometric_shapes_font_interval[96] = {
 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  4,  4, 8, 8,  4,  4,
 8, 8, 8, 8, 8, 8, 8, 8, 5, 5, 9, 9, 8, 8, 6, 6,
 8, 8, 5, 5, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8,  4,  5, 11, 11, 11, 11,  4,  4,  4,  4,
 8, 8, 8, 8, 8, 8,  4, 8, 8, 8, 8, 8, 8, 8, 8, 11,
 13, 13, 13, 13, 14, 14, 14, 14, 13, 13, 13, 11, 11, 11, 11, 14,
};
//
float s_telugu_font_interval[96] = {
 8, 16, 20, 15,     11, 13, 9, 20, 11, 17, 22, 15,     10, 10,
 11,     11, 11, 16, 9, 11, 9, 16, 10, 11, 11, 10, 20, 13, 14,
 9, 11, 11, 13, 11, 11, 11, 11, 10,     10, 10, 11, 12, 16, 20,
 9, 14, 10, 10, 13, 10, 14, 14, 11, 16,             11, 21, 15,
 15, 15, 22, 15, 22,     21, 21, 23,     24, 24, 25, 17,
					 14, 14,     13, 13, 11,
 25, 23, 6, 9,         10, 13, 13, 8, 11, 14, 13, 9, 14, 13,
								 9,  2, 7, 12, 13, 13, 13, 13,
};
//#A490~#A4C6
float s_yi_radicals_font_interval[55] = {
  2, 11, 11, 11, 11, 11, 11, 11, 11, 11, 10, 7, 11, 11, 11, 7,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 7, 11, 11, 11, 11, 11, 9,
 10, 10, 8, 11, 11, 11, 11, 11, 10, 11, 11, 10, 11, 11, 11, 11,
 11, 11, 11, 11, 11, 7,
};
//#3040~#309F
float s_hiragana_font_interval[93] = {
	 11, 14, 11, 14, 11, 13, 12, 14, 12, 14, 14, 16, 14, 16, 12,
 14, 14, 16, 13, 14, 14, 15, 12, 13, 14, 15, 15, 16, 15, 16, 14,
 15, 13, 15, 11, 14, 15, 14, 15, 12, 14, 16, 14, 15, 15, 14, 15,
 16, 16, 15, 15, 15, 16, 16, 16, 15, 15, 15, 15, 15, 16, 19, 14,
 15, 14, 14, 12, 15, 11, 14, 12, 14, 12, 11, 14, 15, 14, 12, 14,
 14, 15, 14, 15, 16, 12, 11,          4,  5,  4,  5, 12, 13, 11,
};
//#30A0~#30FF
float s_katakana_font_interval[96] = {
 10, 12, 16, 10, 14, 11, 14, 11, 15, 12, 16, 15, 15, 16, 16, 14,
 16, 16, 16, 14, 15, 15, 17, 15, 15, 16, 16, 15, 15, 13, 14, 14,
 16, 15, 16, 11, 14, 15, 16, 16, 11, 10, 15, 15, 14, 16, 14, 16,
 17, 17, 13, 15, 15, 13, 15, 16, 16, 16, 16, 16, 16, 16, 14, 15,
 16, 13, 15, 11, 14, 12, 16, 11, 14, 15, 11, 16, 13, 13, 10, 14,
 15, 16, 14, 15, 14, 11, 12, 16, 16, 16, 15,  3, 15, 9, 9, 10,
};
float s_halfwidth_and_fullwidth_forms_font_interval = 12;
float s_mathematical_operators_font_interval = 12;
float s_latin_1_supplement_font_interval = 12;
float s_miscellaneous_symbols_font_interval = 12;
float s_greek_and_coptic_font_interval = 12;
float s_cyrillic_font_interval = 12;
float s_ipa_extensions_font_interval = 12;
float s_gurmukhi_font_interval = 12;
float s_general_punctuation_font_interval = 12;
float s_block_elements_font_interval = 12;
float s_phonetic_extensions_font_interval = 12;
float s_thai_font_interval = 12;
float s_georgian_font_interval = 12;
float s_hangul_compatibility_jamo_font_interval = 12;
float s_miscellaneous_technical_font_interval = 12;
float s_tamil_font_interval = 12;
float s_spacing_modifier_letters_font_interval = 12;
float s_tibetan_font_interval = 12;
float s_arrows_font_interval = 12;
float s_cyrillic_supplement_font_interval = 12;
float s_superscripts_and_subscripts_font_interval = 12;
float s_cjk_compatibility_forms_font_interval = 12;
float s_combining_diacritical_marks_font_interval = 12;
float s_optical_character_recognition_font_interval = 12;
float s_greek_extended_font_interval = 12;
float s_miscellaneous_symbols_and_pictographs_font_interval = 12;
float s_unified_canadian_aboriginal_syllabics_font_interval = 12;
float s_hebrew_font_interval = 12;
float s_lao_font_interval = 12;
float s_kannada_font_interval = 12;
float s_combining_diacritical_marks_for_symbols_font_interval = 12;
float s_combining_diacritical_marks_supplement_font_interval = 12;
float s_sinhala_font_interval = 12;
float s_yi_syllables_font_interval = 12;
float s_font_pos;
double s_scroll_speed = 0.5;
double s_app_up_time_ms = 0;
double s_log_time[4096];

char s_status[100];
char s_swkb_input_text[8192];
std::string s_clipboards[15];
std::string s_app_logs[4096];
std::string s_bot_button_string[2] = { "<                \u25BD                >", "                  \u25BD                 " };
std::string s_square_string = "\u25A0";
std::string s_circle_string = "\u25CF";
std::string s_battery_level_string = "?";
std::string s_error_summary = "";
std::string s_error_description = "";
std::string s_error_place = "";
std::string s_error_code = "";
std::string s_gtr_history[17] = { "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a" };
std::string s_setting[20]; //0 sorce data, 1 language, 2 screen brightness when normal, 3 time to enter afk, 4 screen brightness when afk , 5 setting menu show, 6 scroll speed, 7 allow send app info, 8 number of app start
std::string s_line_message_log[300];
std::string s_spt_message_en[12] = {
	"Downloaded size : ",
	"Download time : ",
	"Speed : ",
	"Select test data size",
	"About 1MB",
	"About 2MB",
	"About 4MB",
	"About 8MB",
	"About 15MB",
	"About 30MB",
	"About 60MB",
	"Start"
};
std::string s_spt_message_jp[12] = {
	"ダウンロード済み容量 : ",
	"ダウンロード時間 : ",
	"速度 : ",
	"テストデータ容量選択",
	"約1MB",
	"約2MB",
	"約4MB",
	"約8MB",
	"約15MB",
	"約30MB",
	"約60MB",
	"開始"
};
std::string s_imv_message_en[8] = {
	"Rendering\n(Key A)",
	"Download\n(Key B)",
	"Adjust url\n(Key Y)",
	"Up\n(Key D UP)",
	"Down\n(Key D DOWN)",
	"Decrease size(Key L)",
	"Increase size(Key R)",
	"Selected clipboard num ",
};
std::string s_imv_message_jp[8] = {
	"レンダリング\n(Aキー)",
	"ダウンロード\n(Bキー)",
	"url調節\n(Yキー)",
	"上\n(十字上キー)",
	"下\n(十字下キー)",
	"サイズを小さく(Lキー)",
	"サイズを大きく(Rキー)",
	"選択されたクリップボード番号 ",
};
std::string s_spt_ver = "v1.0.3";
std::string s_gtr_ver = "v1.0.1";
std::string s_imv_ver = "v1.0.1";
std::string s_line_ver = "v1.3.0";
std::string s_app_ver = "v1.3.0";
std::string s_httpc_user_agent = "Line for 3DS " + s_app_ver;

//#0600~#06FF
std::string s_arabic_sample[255] = {
 "\u0600", "\u0601", "\u0602", "\u0603", "\u0604", "\u0605", "\u0606", "\u0607", "\u0608", "\u0609", "\u060A", "\u060B", "\u060C", "\u060D", "\u060E", "\u060F",
 "\u0610", "\u0611", "\u0612", "\u0613", "\u0614", "\u0615", "\u0616", "\u0617", "\u0618", "\u0619", "\u061A", "\u061B", "\u061C", /*"\u061D"*/ "\u061E", "\u061F",
 "\u0620", "\u0621", "\u0622", "\u0623", "\u0624", "\u0625", "\u0626", "\u0627", "\u0628", "\u0629", "\u062A", "\u062B", "\u062C", "\u062D", "\u062E", "\u062F",
 "\u0630", "\u0631", "\u0632", "\u0633", "\u0634", "\u0635", "\u0636", "\u0637", "\u0638", "\u0639", "\u063A", "\u063B", "\u063C", "\u063D", "\u063E", "\u063F",
 "\u0640", "\u0641", "\u0642", "\u0643", "\u0644", "\u0645", "\u0646", "\u0647", "\u0648", "\u0649", "\u064A", "\u064B", "\u064C", "\u064D", "\u064E", "\u064F",
 "\u0650", "\u0651", "\u0652", "\u0653", "\u0654", "\u0655", "\u0656", "\u0657", "\u0658", "\u0659", "\u065A", "\u065B", "\u065C", "\u065D", "\u065E", "\u065F",
 "\u0660", "\u0661", "\u0662", "\u0663", "\u0664", "\u0665", "\u0666", "\u0667", "\u0668", "\u0669", "\u066A", "\u066B", "\u066C", "\u066D", "\u066E", "\u066F",
 "\u0670", "\u0671", "\u0672", "\u0673", "\u0674", "\u0675", "\u0676", "\u0677", "\u0678", "\u0679", "\u067A", "\u067B", "\u067C", "\u067D", "\u067E", "\u067F",
 "\u0680", "\u0681", "\u0682", "\u0683", "\u0684", "\u0685", "\u0686", "\u0687", "\u0688", "\u0689", "\u068A", "\u068B", "\u068C", "\u068D", "\u068E", "\u068F",
 "\u0690", "\u0691", "\u0692", "\u0693", "\u0694", "\u0695", "\u0696", "\u0697", "\u0698", "\u0699", "\u069A", "\u069B", "\u069C", "\u069D", "\u069E", "\u069F",
 "\u06A0", "\u06A1", "\u06A2", "\u06A3", "\u06A4", "\u06A5", "\u06A6", "\u06A7", "\u06A8", "\u06A9", "\u06AA", "\u06AB", "\u06AC", "\u06AD", "\u06AE", "\u06AF",
 "\u06B0", "\u06B1", "\u06B2", "\u06B3", "\u06B4", "\u06B5", "\u06B6", "\u06B7", "\u06B8", "\u06B9", "\u06BA", "\u06BB", "\u06BC", "\u06BD", "\u06BE", "\u06BF",
 "\u06C0", "\u06C1", "\u06C2", "\u06C3", "\u06C4", "\u06C5", "\u06C6", "\u06C7", "\u06C8", "\u06C9", "\u06CA", "\u06CB", "\u06CC", "\u06CD", "\u06CE", "\u06CF",
 "\u06D0", "\u06D1", "\u06D2", "\u06D3", "\u06D4", "\u06D5", "\u06D6", "\u06D7", "\u06D8", "\u06D9", "\u06DA", "\u06DB", "\u06DC", "\u06DD", "\u06DE", "\u06DF",
 "\u06E0", "\u06E1", "\u06E2", "\u06E3", "\u06E4", "\u06E5", "\u06E6", "\u06E7", "\u06E8", "\u06E9", "\u06EA", "\u06EB", "\u06EC", "\u06ED", "\u06EE", "\u06EF",
 "\u06F0", "\u06F1", "\u06F2", "\u06F3", "\u06F4", "\u06F5", "\u06F6", "\u06F7", "\u06F8", "\u06F9", "\u06fA", "\u06FB", "\u06FC", "\u06FD", "\u06FE", "\u06FF"
};
std::string s_arabic_right_to_left_sample[235] = {
 "\u0600", "\u0601", "\u0602", "\u0603", "\u0604", "\u0605", "\u0606", "\u0607", "\u0608", "\u0609", "\u060A", "\u060B", "\u060C", "\u060D", "\u060E", "\u060F",
 "\u0610", "\u0611", "\u0612", "\u0613", "\u0614", "\u0615", "\u0616", "\u0617", "\u0618", "\u0619", "\u061A", "\u061B", "\u061C", /*"\u061D"*/ "\u061E", "\u061F",
 "\u0620", "\u0621", "\u0622", "\u0623", "\u0624", "\u0625", "\u0626", "\u0627", "\u0628", "\u0629", "\u062A", "\u062B", "\u062C", "\u062D", "\u062E", "\u062F",
 "\u0630", "\u0631", "\u0632", "\u0633", "\u0634", "\u0635", "\u0636", "\u0637", "\u0638", "\u0639", "\u063A", "\u063B", "\u063C", "\u063D", "\u063E", "\u063F",
 "\u0640", "\u0641", "\u0642", "\u0643", "\u0644", "\u0645", "\u0646", "\u0647", "\u0648", "\u0649", "\u064A", "\u064B", "\u064C", "\u064D", "\u064E", "\u064F",
 "\u0650", "\u0651", "\u0652", "\u0653", "\u0654", "\u0655", "\u0656", "\u0657", "\u0658", "\u0659", "\u065A", "\u065B", "\u065C", "\u065D", "\u065E", "\u065F",
 "\u066A", "\u066B", "\u066C", "\u066D", "\u066E", "\u066F",
 "\u0670", "\u0671", "\u0672", "\u0673", "\u0674", "\u0675", "\u0676", "\u0677", "\u0678", "\u0679", "\u067A", "\u067B", "\u067C", "\u067D", "\u067E", "\u067F",
 "\u0680", "\u0681", "\u0682", "\u0683", "\u0684", "\u0685", "\u0686", "\u0687", "\u0688", "\u0689", "\u068A", "\u068B", "\u068C", "\u068D", "\u068E", "\u068F",
 "\u0690", "\u0691", "\u0692", "\u0693", "\u0694", "\u0695", "\u0696", "\u0697", "\u0698", "\u0699", "\u069A", "\u069B", "\u069C", "\u069D", "\u069E", "\u069F",
 "\u06A0", "\u06A1", "\u06A2", "\u06A3", "\u06A4", "\u06A5", "\u06A6", "\u06A7", "\u06A8", "\u06A9", "\u06AA", "\u06AB", "\u06AC", "\u06AD", "\u06AE", "\u06AF",
 "\u06B0", "\u06B1", "\u06B2", "\u06B3", "\u06B4", "\u06B5", "\u06B6", "\u06B7", "\u06B8", "\u06B9", "\u06BA", "\u06BB", "\u06BC", "\u06BD", "\u06BE", "\u06BF",
 "\u06C0", "\u06C1", "\u06C2", "\u06C3", "\u06C4", "\u06C5", "\u06C6", "\u06C7", "\u06C8", "\u06C9", "\u06CA", "\u06CB", "\u06CC", "\u06CD", "\u06CE", "\u06CF",
 "\u06D0", "\u06D1", "\u06D2", "\u06D3", "\u06D4", "\u06D5", "\u06D6", "\u06D7", "\u06D8", "\u06D9", "\u06DA", "\u06DB", "\u06DC", "\u06DD", "\u06DE", "\u06DF",
 "\u06E0", "\u06E1", "\u06E2", "\u06E3", "\u06E4", "\u06E5", "\u06E6", "\u06E7", "\u06E8", "\u06E9", "\u06EA", "\u06EB", "\u06EC", "\u06ED", "\u06EE", "\u06EF",
 "\u06fA", "\u06FB", "\u06FC", "\u06FD", "\u06FE", "\u06FF"
};
//#0531~#058F Invalid
std::string s_armenian_sample[91] = {
 "\u0531", "\u0532", "\u0533", "\u0534", "\u0535", "\u0536", "\u0537", "\u0538", "\u0539", "\u053A", "\u053B", "\u053C", "\u053D", "\u053E", "\u053F",
 "\u0540", "\u0541", "\u0542", "\u0543", "\u0544", "\u0545", "\u0546", "\u0547", "\u0548", "\u0549", "\u054A", "\u054B", "\u054C", "\u054D", "\u054E", "\u054F",
 "\u0550", "\u0551", "\u0552", "\u0553", "\u0554", "\u0555", "\u0556", /*u0557     u0558*/ "\u0559", "\u055A", "\u055B", "\u055C", "\u055D", "\u055E", "\u055F",
 "\u0560", "\u0561", "\u0562", "\u0563", "\u0564", "\u0565", "\u0566", "\u0567", "\u0568", "\u0569", "\u056A", "\u056B", "\u056C", "\u056D", "\u056E", "\u056F",
 "\u0570", "\u0571", "\u0572", "\u0573", "\u0574", "\u0575", "\u0576", "\u0577", "\u0578", "\u0579", "\u057A", "\u057B", "\u057C", "\u057D", "\u057E", "\u057F",
 "\u0580", "\u0581", "\u0582", "\u0583", "\u0584", "\u0585", "\u0586", "\u0587", "\u0588", "\u0589", "\u058A", /*"\u058B" "\u058C"*/ "\u058D", "\u058E", "\u058F"
};
//#2500~#257F
std::string s_box_drawing_font_sample[128] = {
 "\u2500","\u2501","\u2502","\u2503","\u2504","\u2505","\u2506","\u2507","\u2508","\u2509","\u250A","\u250B","\u250C","\u250D","\u250E","\u250F",
 "\u2510","\u2511","\u2512","\u2513","\u2514","\u2515","\u2516","\u2517","\u2518","\u2519","\u251A","\u251B","\u251C","\u251D","\u251E","\u251F",
 "\u2520","\u2521","\u2522","\u2523","\u2524","\u2525","\u2526","\u2527","\u2528","\u2529","\u252A","\u252B","\u252C","\u252D","\u252E","\u252F",
 "\u2530","\u2531","\u2532","\u2533","\u2534","\u2535","\u2536","\u2537","\u2538","\u2539","\u253A","\u253B","\u253C","\u253D","\u253E","\u253F",
 "\u2540","\u2541","\u2542","\u2543","\u2544","\u2545","\u2546","\u2547","\u2548","\u2549","\u254A","\u254B","\u254C","\u254D","\u254E","\u254F",
 "\u2550","\u2551","\u2552","\u2553","\u2554","\u2555","\u2556","\u2557","\u2558","\u2559","\u255A","\u255B","\u255C","\u255D","\u255E","\u255F",
 "\u2560","\u2561","\u2562","\u2563","\u2564","\u2565","\u2566","\u2567","\u2568","\u2569","\u256A","\u256B","\u256C","\u256D","\u256E","\u256F",
 "\u2570","\u2571","\u2572","\u2573","\u2574","\u2575","\u2576","\u2577","\u2578","\u2579","\u257A","\u257B","\u257C","\u257D","\u257E","\u257F",
};
//#3000~#303F
std::string s_cjk_symbol_and_punctuation_font_sample[64] = {
 "\u3000","\u3001","\u3002","\u3003","\u3004","\u3005","\u3006","\u3007","\u3008","\u3009","\u300A","\u300B","\u300C","\u300D","\u300E","\u300F",
 "\u3010","\u3011","\u3012","\u3013","\u3014","\u3015","\u3016","\u3017","\u3018","\u3019","\u301A","\u301B","\u301C","\u301D","\u301E","\u301F",
 "\u3020","\u3021","\u3022","\u3023","\u3024","\u3025","\u3026","\u3027","\u3028","\u3029","\u302A","\u302B","\u302C","\u302D","\u302E","\u302F",
 "\u3030","\u3031","\u3032","\u3033","\u3034","\u3035","\u3036","\u3037","\u3038","\u3039","\u303A","\u303B","\u303C","\u303D","\u303E","\u303F",
};
std::string s_devanagari_font_sample[128] = {
 "\u0900","\u0901","\u0902","\u0903","\u0904","\u0905","\u0906","\u0907","\u0908","\u0909","\u090A","\u090B","\u090C","\u090D","\u090E","\u090F",
 "\u0910","\u0911","\u0912","\u0913","\u0914","\u0915","\u0916","\u0917","\u0918","\u0919","\u091A","\u091B","\u091C","\u091D","\u091E","\u091F",
 "\u0920","\u0921","\u0922","\u0923","\u0924","\u0925","\u0926","\u0927","\u0928","\u0929","\u092A","\u092B","\u092C","\u092D","\u092E","\u092F",
 "\u0930","\u0931","\u0932","\u0933","\u0934","\u0935","\u0936","\u0937","\u0938","\u0939","\u093A","\u093B","\u093C","\u093D","\u093E","\u093F",
 "\u0940","\u0941","\u0942","\u0943","\u0944","\u0945","\u0946","\u0947","\u0948","\u0949","\u094A","\u094B","\u094C","\u094D","\u094E","\u094F",
 "\u0950","\u0951","\u0952","\u0953","\u0954","\u0955","\u0956","\u0957","\u0958","\u0959","\u095A","\u095B","\u095C","\u095D","\u095E","\u095F",
 "\u0960","\u0961","\u0962","\u0963","\u0964","\u0965","\u0966","\u0967","\u0968","\u0969","\u096A","\u096B","\u096C","\u096D","\u096E","\u096F",
 "\u0970","\u0971","\u0972","\u0973","\u0974","\u0975","\u0976","\u0977","\u0978","\u0979","\u097A","\u097B","\u097C","\u097D","\u097E","\u097F",
};
//#2700~#27BF
std::string s_dingbats_font_sample[192] = {
 "\u2700", "\u2701", "\u2702", "\u2703", "\u2704", "\u2705", "\u2706", "\u2707", "\u2708", "\u2709", "\u270A", "\u270B", "\u270C", "\u270D", "\u270E", "\u270F",
 "\u2710", "\u2711", "\u2712", "\u2713", "\u2714", "\u2715", "\u2716", "\u2717", "\u2718", "\u2719", "\u271A", "\u271B", "\u271C", "\u271D", "\u271E", "\u271F",
 "\u2720", "\u2721", "\u2722", "\u2723", "\u2724", "\u2725", "\u2726", "\u2727", "\u2728", "\u2729", "\u272A", "\u272B", "\u272C", "\u272D", "\u272E", "\u272F",
 "\u2730", "\u2731", "\u2732", "\u2733", "\u2734", "\u2735", "\u2736", "\u2737", "\u2738", "\u2739", "\u273A", "\u273B", "\u273C", "\u273D", "\u273E", "\u273F",
 "\u2740", "\u2741", "\u2742", "\u2743", "\u2744", "\u2745", "\u2746", "\u2747", "\u2748", "\u2749", "\u274A", "\u274B", "\u274C", "\u274D", "\u274E", "\u274F",
 "\u2750", "\u2751", "\u2752", "\u2753", "\u2754", "\u2755", "\u2756", "\u2757", "\u2758", "\u2759", "\u275A", "\u275B", "\u275C", "\u275D", "\u275E", "\u275F",
 "\u2760", "\u2761", "\u2762", "\u2763", "\u2764", "\u2765", "\u2766", "\u2767", "\u2768", "\u2769", "\u276A", "\u276B", "\u276C", "\u276D", "\u276E", "\u276F",
 "\u2770", "\u2771", "\u2772", "\u2773", "\u2774", "\u2775", "\u2776", "\u2777", "\u2778", "\u2779", "\u277A", "\u277B", "\u277C", "\u277D", "\u277E", "\u277F",
 "\u2780", "\u2781", "\u2782", "\u2783", "\u2784", "\u2785", "\u2786", "\u2787", "\u2788", "\u2789", "\u278A", "\u278B", "\u278C", "\u278D", "\u278E", "\u278F",
 "\u2790", "\u2791", "\u2792", "\u2793", "\u2794", "\u2795", "\u2796", "\u2797", "\u2798", "\u2799", "\u279A", "\u279B", "\u279C", "\u279D", "\u279E", "\u279F",
 "\u27A0", "\u27A1", "\u27A2", "\u27A3", "\u27A4", "\u27A5", "\u27A6", "\u27A7", "\u27A8", "\u27A9", "\u27AA", "\u27AB", "\u27AC", "\u27AD", "\u27AE", "\u27AF",
 "\u27B0", "\u27B1", "\u27B2", "\u27B3", "\u27B4", "\u27B5", "\u27B6", "\u27B7", "\u27B8", "\u27B9", "\u27BA", "\u27BB", "\u27BC", "\u27BD", "\u27BE", "\u27BF",
};
//#0020~#007F
std::string s_basic_latin_font_sample[128] = {
 "\u0000", "\u0001", "\u0002", "\u0003", "\u0004", "\u0005", "\u0006", "\u0007", "\u0008", "\u0009", "\u000A", "\u000B", "\u000C", "\u000D", "\u000E", "\u000F",
 "\u0010", "\u0011", "\u0012", "\u0013", "\u0014", "\u0015", "\u0016", "\u0017", "\u0018", "\u0019", "\u001A", "\u001B", "\u001C", "\u001D", "\u001E", "\u001F",
 "\u0020", "\u0021", "\u0022", "\u0023", "\u0024", "\u0025", "\u0026", "\u0027", "\u0028", "\u0029", "\u002A", "\u002B", "\u002C", "\u002D", "\u002E", "\u002F",
 "\u0030", "\u0031", "\u0032", "\u0033", "\u0034", "\u0035", "\u0036", "\u0037", "\u0038", "\u0039", "\u003A", "\u003B", "\u003C", "\u003D", "\u003E", "\u003F",
 "\u0040", "\u0041", "\u0042", "\u0043", "\u0044", "\u0045", "\u0046", "\u0047", "\u0048", "\u0049", "\u004A", "\u004B", "\u004C", "\u004D", "\u004E", "\u004F",
 "\u0050", "\u0051", "\u0052", "\u0053", "\u0054", "\u0055", "\u0056", "\u0057", "\u0058", "\u0059", "\u005A", "\u005B", "\u005C", "\u005D", "\u005E", "\u005F",
 "\u0060", "\u0061", "\u0062", "\u0063", "\u0064", "\u0065", "\u0066", "\u0067", "\u0068", "\u0069", "\u006A", "\u006B", "\u006C", "\u006D", "\u006E", "\u006F",
 "\u0070", "\u0071", "\u0072", "\u0073", "\u0074", "\u0075", "\u0076", "\u0077", "\u0078", "\u0079", "\u007A", "\u007B", "\u007C", "\u007D", "\u007E", "\u007F",
};
//#25A0~#25F0
std::string s_geometric_shapes_font_sample[96] = {
 "\u25A0", "\u25A1", "\u25A2", "\u25A3", "\u25A4", "\u25A5", "\u25A6", "\u25A7", "\u25A8", "\u25A9", "\u25AA", "\u25AB", "\u25AC", "\u25AD", "\u25AE", "\u25AF",
 "\u25B0", "\u25B1", "\u25B2", "\u25B3", "\u25B4", "\u25B5", "\u25B6", "\u25B7", "\u25B8", "\u25B9", "\u25BA", "\u25BB", "\u25BC", "\u25BD", "\u25BE", "\u25BF",
 "\u25C0", "\u25C1", "\u25C2", "\u25C3", "\u25C4", "\u25C5", "\u25C6", "\u25C7", "\u25C8", "\u25C9", "\u25CA", "\u25CB", "\u25CC", "\u25CD", "\u25CE", "\u25CF",
 "\u25D0", "\u25D1", "\u25D2", "\u25D3", "\u25D4", "\u25D5", "\u25D6", "\u25D7", "\u25D8", "\u25D9", "\u25DA", "\u25DB", "\u25DC", "\u25DD", "\u25DE", "\u25DF",
 "\u25E0", "\u25E1", "\u25E2", "\u25E3", "\u25E4", "\u25E5", "\u25E6", "\u25E7", "\u25E8", "\u25E9", "\u25EA", "\u25EB", "\u25EC", "\u25ED", "\u25EE", "\u25EF",
 "\u25F0", "\u25F1", "\u25F2", "\u25F3", "\u25F4", "\u25F5", "\u25F6", "\u25F7", "\u25F8", "\u25F9", "\u25FA", "\u25FB", "\u25FC", "\u25FD", "\u25FE", "\u25FF",
};
//#0C00~0C7F
std::string s_telugu_font_sample[96] = {
 "\u0C00", "\u0C01", "\u0C02", "\u0C03", /*u0C04*/ "\u0C05", "\u0C06", "\u0C07", "\u0C08", "\u0C09", "\u0C0A", "\u0C0B", "\u0C0C", /*u0C0D*/ "\u0C0E", "\u0C0F",
 "\u0C10", /*u0C11*/ "\u0C12", "\u0C13", "\u0C14", "\u0C15", "\u0C16", "\u0C17", "\u0C18", "\u0C19", "\u0C1A", "\u0C1B", "\u0C1C", "\u0C1D", "\u0C1E", "\u0C1F",
 "\u0C20", "\u0C21", "\u0C22", "\u0C23", "\u0C24", "\u0C25", "\u0C26", "\u0C27", "\u0C28", /*u0C29*/ "\u0C2A", "\u0C2B", "\u0C2C", "\u0C2D", "\u0C2E", "\u0C2F",
 "\u0C30", "\u0C31", "\u0C32", "\u0C33", "\u0C34", "\u0C35", "\u0C36", "\u0C37", "\u0C38", "\u0C39", /*u0C3A     u0C3B     u0C3C*/ "\u0C3D", "\u0C3E", "\u0C3F",
 "\u0C40", "\u0C41", "\u0C42", "\u0C43", "\u0C44", /*u0C45*/ "\u0C46", "\u0C47", "\u0C48", /*u0C49*/ "\u0C4A", "\u0C4B", "\u0C4C", "\u0C4D", /*u0C4E     u0C4F*/
 /*u0C50     u0C51     u0C52     u0C53     u0C54*/ "\u0C55", "\u0C56", /*u0C57*/ "\u0C58", "\u0C59", "\u0C5A", /*u0C5B     u0C5C     u0C5D     u0C5E     u0C5F*/
 "\u0C60", "\u0C61", "\u0C62", "\u0C63", /*u0C64     u0C65*/ "\u0C66", "\u0C67", "\u0C68", "\u0C69", "\u0C6A", "\u0C6B", "\u0C6C", "\u0C6D", "\u0C6E", "\u0C6F",
 /*u0C70     u0C71     u0C72     u0C73     u0C74     u0C75     u0C76     u0C77*/ "\u0C78", "\u0C79", "\u0C7A", "\u0C7B", "\u0C7C", "\u0C7D", "\u0C7E", "\u0C7F",
};
//#A490~#A4C6
std::string s_yi_radicals_font_sample[55] = {
 "\uA490", "\uA491", "\uA492", "\uA493", "\uA494", "\uA495", "\uA496", "\uA497", "\uA498", "\uA499", "\uA49A", "\uA49B", "\uA49C", "\uA49D", "\uA49E", "\uA49F",
 "\uA4A0", "\uA4A1", "\uA4A2", "\uA4A3", "\uA4A4", "\uA4A5", "\uA4A6", "\uA4A7", "\uA4A8", "\uA4A9", "\uA4AA", "\uA4AB", "\uA4AC", "\uA4AD", "\uA4AE", "\uA4AF",
 "\uA4B0", "\uA4B1", "\uA4B2", "\uA4B3", "\uA4B4", "\uA4B5", "\uA4B6", "\uA4B7", "\uA4B8", "\uA4B9", "\uA4BA", "\uA4BB", "\uA4BC", "\uA4BD", "\uA4BE", "\uA4BF",
 "\uA4C0", "\uA4C1", "\uA4C2", "\uA4C3", "\uA4C4", "\uA4C5", "\uA4C6",
};
//#3040~#309F
std::string s_hiragana_font_sample[93] = {
	/*u3040*/"\u3041","\u3042","\u3043","\u3044","\u3045","\u3046","\u3047","\u3048","\u3049","\u304A","\u304B","\u304C","\u304D","\u304E","\u304F",
	"\u3050","\u3051","\u3052","\u3053","\u3054","\u3055","\u3056","\u3057","\u3058","\u3059","\u305A","\u305B","\u305C","\u305D","\u305E","\u305F",
	"\u3060","\u3061","\u3062","\u3063","\u3064","\u3065","\u3066","\u3067","\u3068","\u3069","\u306A","\u306B","\u306C","\u306D","\u306E","\u306F",
	"\u3070","\u3071","\u3072","\u3073","\u3074","\u3075","\u3076","\u3077","\u3078","\u3079","\u307A","\u307B","\u307C","\u307D","\u307E","\u307F",
	"\u3080","\u3081","\u3082","\u3083","\u3084","\u3085","\u3086","\u3087","\u3088","\u3089","\u308A","\u308B","\u308C","\u308D","\u308E","\u308F",
	"\u3090","\u3091","\u3092","\u3093","\u3094","\u3095","\u3096",/*u3097    u3098*/"\u3099","\u309A","\u309B","\u309C","\u309D","\u309E","\u309F",
};
//#30A0~#30FF
std::string s_katakana_font_sample[96] = {
 "\u30A0","\u30A1","\u30A2","\u30A3","\u30A4","\u30A5","\u30A6","\u30A7","\u30A8","\u30A9","\u30AA","\u30AB","\u30AC","\u30AD","\u30AE","\u30AF",
 "\u30B0","\u30B1","\u30B2","\u30B3","\u30B4","\u30B5","\u30B6","\u30B7","\u30B8","\u30B9","\u30BA","\u30BB","\u30BC","\u30BD","\u30BE","\u30BF",
 "\u30C0","\u30C1","\u30C2","\u30C3","\u30C4","\u30C5","\u30C6","\u30C7","\u30C8","\u30C9","\u30CA","\u30CB","\u30CC","\u30CD","\u30CE","\u30CF",
 "\u30D0","\u30D1","\u30D2","\u30D3","\u30D4","\u30D5","\u30D6","\u30D7","\u30D8","\u30D9","\u30DA","\u30DB","\u30DC","\u30DD","\u30DE","\u30DF",
 "\u30E0","\u30E1","\u30E2","\u30E3","\u30E4","\u30E5","\u30E6","\u30E7","\u30E8","\u30E9","\u30EA","\u30EB","\u30EC","\u30ED","\u30EE","\u30EF",
 "\u30F0","\u30F1","\u30F2","\u30F3","\u30F4","\u30F5","\u30F6","\u30F7","\u30F8","\u30F9","\u30FA","\u30FB","\u30FC","\u30FD","\u30FE","\u30FF",
};
std::string s_halfwidth_and_fullwidth_forms_font_sample[225] = {
	/*uFF00*/"\uFF01","\uFF02","\uFF03","\uFF04","\uFF05","\uFF06","\uFF07","\uFF08","\uFF09","\uFF0A","\uFF0B","\uFF0C","\uFF0D","\uFF0E","\uFF0F",
	"\uFF10","\uFF11","\uFF12","\uFF13","\uFF14","\uFF15","\uFF16","\uFF17","\uFF18","\uFF19","\uFF1A","\uFF1B","\uFF1C","\uFF1D","\uFF1E","\uFF1F",
	"\uFF20","\uFF21","\uFF22","\uFF23","\uFF24","\uFF25","\uFF26","\uFF27","\uFF28","\uFF29","\uFF2A","\uFF2B","\uFF2C","\uFF2D","\uFF2E","\uFF2F",
	"\uFF30","\uFF31","\uFF32","\uFF33","\uFF34","\uFF35","\uFF36","\uFF37","\uFF38","\uFF39","\uFF3A","\uFF3B","\uFF3C","\uFF3D","\uFF3E","\uFF3F",
	"\uFF40","\uFF41","\uFF42","\uFF43","\uFF44","\uFF45","\uFF46","\uFF47","\uFF48","\uFF49","\uFF4A","\uFF4B","\uFF4C","\uFF4D","\uFF4E","\uFF4F",
	"\uFF50","\uFF51","\uFF52","\uFF53","\uFF54","\uFF55","\uFF56","\uFF57","\uFF58","\uFF59","\uFF5A","\uFF5B","\uFF5C","\uFF5D","\uFF5E","\uFF5F",
	"\uFF60","\uFF61","\uFF62","\uFF63","\uFF64","\uFF65","\uFF66","\uFF67","\uFF68","\uFF69","\uFF6A","\uFF6B","\uFF6C","\uFF6D","\uFF6E","\uFF6F",
	"\uFF70","\uFF71","\uFF72","\uFF73","\uFF74","\uFF75","\uFF76","\uFF77","\uFF78","\uFF79","\uFF7A","\uFF7B","\uFF7C","\uFF7D","\uFF7E","\uFF7F",
	"\uFF80","\uFF81","\uFF82","\uFF83","\uFF84","\uFF85","\uFF86","\uFF87","\uFF88","\uFF89","\uFF8A","\uFF8B","\uFF8C","\uFF8D","\uFF8E","\uFF8F",
	"\uFF90","\uFF91","\uFF92","\uFF93","\uFF94","\uFF95","\uFF96","\uFF97","\uFF98","\uFF99","\uFF9A","\uFF9B","\uFF9C","\uFF9D","\uFF9E","\uFF9F",
	"\uFFA0","\uFFA1","\uFFA2","\uFFA3","\uFFA4","\uFFA5","\uFFA6","\uFFA7","\uFFA8","\uFFA9","\uFFAA","\uFFAB","\uFFAC","\uFFAD","\uFFAE","\uFFAF",
	"\uFFB0","\uFFB1","\uFFB2","\uFFB3","\uFFB4","\uFFB5","\uFFB6","\uFFB7","\uFFB8","\uFFB9","\uFFBA","\uFFBB","\uFFBC","\uFFBD","\uFFBE",/*uFFBF
	  uFFC0    uFFC1*/"\uFFC2","\uFFC3","\uFFC4","\uFFC5","\uFFC6","\uFFC7",/*uFFC8    uFFC9*/"\uFFCA","\uFFCB","\uFFCC","\uFFCD","\uFFCE","\uFFCF",
	  /*uFFD0    uFFD1*/"\uFFD2","\uFFD3","\uFFD4","\uFFD5","\uFFD6","\uFFD7",/*uFFD8    uFFD9*/"\uFFDA","\uFFDB","\uFFDC",/*uFFDD    uFFDE    uFFDF*/
	  "\uFFE0","\uFFE1","\uFFE2","\uFFE3","\uFFE4","\uFFE5","\uFFE6",/*uFFE7*/"\uFFE8","\uFFE9","\uFFEA","\uFFEB","\uFFEC","\uFFED","\uFFEE",/*uFFEF*/
};
std::string s_mathematical_operators_font_sample[256] = {
 "\u2200","\u2201","\u2202","\u2203","\u2204","\u2205","\u2206","\u2207","\u2208","\u2209","\u220A","\u220B","\u220C","\u220D","\u220E","\u220F",
 "\u2210","\u2211","\u2212","\u2213","\u2214","\u2215","\u2216","\u2217","\u2218","\u2219","\u221A","\u221B","\u221C","\u221D","\u221E","\u221F",
 "\u2220","\u2221","\u2222","\u2223","\u2224","\u2225","\u2226","\u2227","\u2228","\u2229","\u222A","\u222B","\u222C","\u222D","\u222E","\u222F",
 "\u2230","\u2231","\u2232","\u2233","\u2234","\u2235","\u2236","\u2237","\u2238","\u2239","\u223A","\u223B","\u223C","\u223D","\u223E","\u223F",
 "\u2240","\u2241","\u2242","\u2243","\u2244","\u2245","\u2246","\u2247","\u2248","\u2249","\u224A","\u224B","\u224C","\u224D","\u224E","\u224F",
 "\u2250","\u2251","\u2252","\u2253","\u2254","\u2255","\u2256","\u2257","\u2258","\u2259","\u225A","\u225B","\u225C","\u225D","\u225E","\u225F",
 "\u2260","\u2261","\u2262","\u2263","\u2264","\u2265","\u2266","\u2267","\u2268","\u2269","\u226A","\u226B","\u226C","\u226D","\u226E","\u226F",
 "\u2270","\u2271","\u2272","\u2273","\u2274","\u2275","\u2276","\u2277","\u2278","\u2279","\u227A","\u227B","\u227C","\u227D","\u227E","\u227F",
 "\u2280","\u2281","\u2282","\u2283","\u2284","\u2285","\u2286","\u2287","\u2288","\u2289","\u228A","\u228B","\u228C","\u228D","\u228E","\u228F",
 "\u2290","\u2291","\u2292","\u2293","\u2294","\u2295","\u2296","\u2297","\u2298","\u2299","\u229A","\u229B","\u229C","\u229D","\u229E","\u229F",
 "\u22A0","\u22A1","\u22A2","\u22A3","\u22A4","\u22A5","\u22A6","\u22A7","\u22A8","\u22A9","\u22AA","\u22AB","\u22AC","\u22AD","\u22AE","\u22AF",
 "\u22B0","\u22B1","\u22B2","\u22B3","\u22B4","\u22B5","\u22B6","\u22B7","\u22B8","\u22B9","\u22BA","\u22BB","\u22BC","\u22BD","\u22BE","\u22BF",
 "\u22C0","\u22C1","\u22C2","\u22C3","\u22C4","\u22C5","\u22C6","\u22C7","\u22C8","\u22C9","\u22CA","\u22CB","\u22CC","\u22CD","\u22CE","\u22CF",
 "\u22D0","\u22D1","\u22D2","\u22D3","\u22D4","\u22D5","\u22D6","\u22D7","\u22D8","\u22D9","\u22DA","\u22DB","\u22DC","\u22DD","\u22DE","\u22DF",
 "\u22E0","\u22E1","\u22E2","\u22E3","\u22E4","\u22E5","\u22E6","\u22E7","\u22E8","\u22E9","\u22EA","\u22EB","\u22EC","\u22ED","\u22EE","\u22EF",
 "\u22F0","\u22F1","\u22F2","\u22F3","\u22F4","\u22F5","\u22F6","\u22F7","\u22F8","\u22F9","\u22FA","\u22FB","\u22FC","\u22FD","\u22FE","\u22FF",
};
std::string s_latin_1_supplement_font_sample[128] = {
 "\u0080", "\u0081", "\u0082", "\u0083", "\u0084", "\u0085", "\u0086", "\u0087", "\u0088", "\u0089", "\u008A", "\u008B", "\u008C", "\u008D", "\u008E", "\u008F",
 "\u0090", "\u0091", "\u0092", "\u0093", "\u0094", "\u0095", "\u0096", "\u0097", "\u0098", "\u0099", "\u009A", "\u009B", "\u009C", "\u009D", "\u009E", "\u009F",
 "\u00A0", "\u00A1", "\u00A2", "\u00A3", "\u00A4", "\u00A5", "\u00A6", "\u00A7", "\u00A8", "\u00A9", "\u00AA", "\u00AB", "\u00AC", "\u00AD", "\u00AE", "\u00AF",
 "\u00B0", "\u00B1", "\u00B2", "\u00B3", "\u00B4", "\u00B5", "\u00B6", "\u00B7", "\u00B8", "\u00B9", "\u00BA", "\u00BB", "\u00BC", "\u00BD", "\u00BE", "\u00BF",
 "\u00C0", "\u00C1", "\u00C2", "\u00C3", "\u00C4", "\u00C5", "\u00C6", "\u00C7", "\u00C8", "\u00C9", "\u00CA", "\u00CB", "\u00CC", "\u00CD", "\u00CE", "\u00CF",
 "\u00D0", "\u00D1", "\u00D2", "\u00D3", "\u00D4", "\u00D5", "\u00D6", "\u00D7", "\u00D8", "\u00D9", "\u00DA", "\u00DB", "\u00DC", "\u00DD", "\u00DE", "\u00DF",
 "\u00E0", "\u00E1", "\u00E2", "\u00E3", "\u00E4", "\u00E5", "\u00E6", "\u00E7", "\u00E8", "\u00E9", "\u00EA", "\u00EB", "\u00EC", "\u00ED", "\u00EE", "\u00EF",
 "\u00F0", "\u00F1", "\u00F2", "\u00F3", "\u00F4", "\u00F5", "\u00F6", "\u00F7", "\u00F8", "\u00F9", "\u00FA", "\u00FB", "\u00FC", "\u00FD", "\u00FE", "\u00FF",
};
std::string s_miscellaneous_symbols_font_sample[256] = {
 "\u2600","\u2601","\u2602","\u2603","\u2604","\u2605","\u2606","\u2607","\u2608","\u2609","\u260A","\u260B","\u260C","\u260D","\u260E","\u260F",
 "\u2610","\u2611","\u2612","\u2613","\u2614","\u2615","\u2616","\u2617","\u2618","\u2619","\u261A","\u261B","\u261C","\u261D","\u261E","\u261F",
 "\u2620","\u2621","\u2622","\u2623","\u2624","\u2625","\u2626","\u2627","\u2628","\u2629","\u262A","\u262B","\u262C","\u262D","\u262E","\u262F",
 "\u2630","\u2631","\u2632","\u2633","\u2634","\u2635","\u2636","\u2637","\u2638","\u2639","\u263A","\u263B","\u263C","\u263D","\u263E","\u263F",
 "\u2640","\u2641","\u2642","\u2643","\u2644","\u2645","\u2646","\u2647","\u2648","\u2649","\u264A","\u264B","\u264C","\u264D","\u264E","\u264F",
 "\u2650","\u2651","\u2652","\u2653","\u2654","\u2655","\u2656","\u2657","\u2658","\u2659","\u265A","\u265B","\u265C","\u265D","\u265E","\u265F",
 "\u2660","\u2661","\u2662","\u2663","\u2664","\u2665","\u2666","\u2667","\u2668","\u2669","\u266A","\u266B","\u266C","\u266D","\u266E","\u266F",
 "\u2670","\u2671","\u2672","\u2673","\u2674","\u2675","\u2676","\u2677","\u2678","\u2679","\u267A","\u267B","\u267C","\u267D","\u267E","\u267F",
 "\u2680","\u2681","\u2682","\u2683","\u2684","\u2685","\u2686","\u2687","\u2688","\u2689","\u268A","\u268B","\u268C","\u268D","\u268E","\u268F",
 "\u2690","\u2691","\u2692","\u2693","\u2694","\u2695","\u2696","\u2697","\u2698","\u2699","\u269A","\u269B","\u269C","\u269D","\u269E","\u269F",
 "\u26A0","\u26A1","\u26A2","\u26A3","\u26A4","\u26A5","\u26A6","\u26A7","\u26A8","\u26A9","\u26AA","\u26AB","\u26AC","\u26AD","\u26AE","\u26AF",
 "\u26B0","\u26B1","\u26B2","\u26B3","\u26B4","\u26B5","\u26B6","\u26B7","\u26B8","\u26B9","\u26BA","\u26BB","\u26BC","\u26BD","\u26BE","\u26BF",
 "\u26C0","\u26C1","\u26C2","\u26C3","\u26C4","\u26C5","\u26C6","\u26C7","\u26C8","\u26C9","\u26CA","\u26CB","\u26CC","\u26CD","\u26CE","\u26CF",
 "\u26D0","\u26D1","\u26D2","\u26D3","\u26D4","\u26D5","\u26D6","\u26D7","\u26D8","\u26D9","\u26DA","\u26DB","\u26DC","\u26DD","\u26DE","\u26DF",
 "\u26E0","\u26E1","\u26E2","\u26E3","\u26E4","\u26E5","\u26E6","\u26E7","\u26E8","\u26E9","\u26EA","\u26EB","\u26EC","\u26ED","\u26EE","\u26EF",
 "\u26F0","\u26F1","\u26F2","\u26F3","\u26F4","\u26F5","\u26F6","\u26F7","\u26F8","\u26F9","\u26FA","\u26FB","\u26FC","\u26FD","\u26FE","\u26FF",
};
std::string s_greek_and_coptic_font_sample[135] = {
 "\u0370","\u0371","\u0372","\u0373","\u0374","\u0375","\u0376","\u0377",/*u0378    u0379*/"\u037A","\u037B","\u037C","\u037D","\u037E","\u037F",
 /*u0380    u0381    u0382    u0383*/"\u0384","\u0385","\u0386","\u0387","\u0388","\u0389","\u038A",/*u038B*/"\u038C",/*u038D*/"\u038E","\u038F",
 "\u0390","\u0391","\u0392","\u0393","\u0394","\u0395","\u0396","\u0397","\u0398","\u0399","\u039A","\u039B","\u039C","\u039D","\u039E","\u039F",
 "\u03A0","\u03A1",/*u03A2*/"\u03A3","\u03A4","\u03A5","\u03A6","\u03A7","\u03A8","\u03A9","\u03AA","\u03AB","\u03AC","\u03AD","\u03AE","\u03AF",
 "\u03B0","\u03B1","\u03B2","\u03B3","\u03B4","\u03B5","\u03B6","\u03B7","\u03B8","\u03B9","\u03BA","\u03BB","\u03BC","\u03BD","\u03BE","\u03BF",
 "\u03C0","\u03C1","\u03C2","\u03C3","\u03C4","\u03C5","\u03C6","\u03C7","\u03C8","\u03C9","\u03CA","\u03CB","\u03CC","\u03CD","\u03CE","\u03CF",
 "\u03D0","\u03D1","\u03D2","\u03D3","\u03D4","\u03D5","\u03D6","\u03D7","\u03D8","\u03D9","\u03DA","\u03DB","\u03DC","\u03DD","\u03DE","\u03DF",
 "\u03E0","\u03E1","\u03E2","\u03E3","\u03E4","\u03E5","\u03E6","\u03E7","\u03E8","\u03E9","\u03EA","\u03EB","\u03EC","\u03ED","\u03EE","\u03EF",
 "\u03F0","\u03F1","\u03F2","\u03F3","\u03F4","\u03F5","\u03F6","\u03F7","\u03F8","\u03F9","\u03FA","\u03FB","\u03FC","\u03FD","\u03FE","\u03FF",
};
std::string s_cyrillic_font_sample[256] = {
 "\u0400", "\u0401", "\u0402", "\u0403", "\u0404", "\u0405", "\u0406", "\u0407", "\u0408", "\u0409", "\u040A", "\u040B", "\u040C", "\u040D", "\u040E", "\u040F",
 "\u0410", "\u0411", "\u0412", "\u0413", "\u0414", "\u0415", "\u0416", "\u0417", "\u0418", "\u0419", "\u041A", "\u041B", "\u041C", "\u041D", "\u041E", "\u041F",
 "\u0420", "\u0421", "\u0422", "\u0423", "\u0424", "\u0425", "\u0426", "\u0427", "\u0428", "\u0429", "\u042A", "\u042B", "\u042C", "\u042D", "\u042E", "\u042F",
 "\u0430", "\u0431", "\u0432", "\u0433", "\u0434", "\u0435", "\u0436", "\u0437", "\u0438", "\u0439", "\u043A", "\u043B", "\u043C", "\u043D", "\u043E", "\u043F",
 "\u0440", "\u0441", "\u0442", "\u0443", "\u0444", "\u0445", "\u0446", "\u0447", "\u0448", "\u0449", "\u044A", "\u044B", "\u044C", "\u044D", "\u044E", "\u044F",
 "\u0450", "\u0451", "\u0452", "\u0453", "\u0454", "\u0455", "\u0456", "\u0457", "\u0458", "\u0459", "\u045A", "\u045B", "\u045C", "\u045D", "\u045E", "\u045F",
 "\u0460", "\u0461", "\u0462", "\u0463", "\u0464", "\u0465", "\u0466", "\u0467", "\u0468", "\u0469", "\u046A", "\u046B", "\u046C", "\u046D", "\u046E", "\u046F",
 "\u0470", "\u0471", "\u0472", "\u0473", "\u0474", "\u0475", "\u0476", "\u0477", "\u0478", "\u0479", "\u047A", "\u047B", "\u047C", "\u047D", "\u047E", "\u047F",
 "\u0480", "\u0481", "\u0482", "\u0483", "\u0484", "\u0485", "\u0486", "\u0487", "\u0488", "\u0489", "\u048A", "\u048B", "\u048C", "\u048D", "\u048E", "\u048F",
 "\u0490", "\u0491", "\u0492", "\u0493", "\u0494", "\u0495", "\u0496", "\u0497", "\u0498", "\u0499", "\u049A", "\u049B", "\u049C", "\u049D", "\u049E", "\u049F",
 "\u04A0", "\u04A1", "\u04A2", "\u04A3", "\u04A4", "\u04A5", "\u04A6", "\u04A7", "\u04A8", "\u04A9", "\u04AA", "\u04AB", "\u04AC", "\u04AD", "\u04AE", "\u04AF",
 "\u04B0", "\u04B1", "\u04B2", "\u04B3", "\u04B4", "\u04B5", "\u04B6", "\u04B7", "\u04B8", "\u04B9", "\u04BA", "\u04BB", "\u04BC", "\u04BD", "\u04BE", "\u04BF",
 "\u04C0", "\u04C1", "\u04C2", "\u04C3", "\u04C4", "\u04C5", "\u04C6", "\u04C7", "\u04C8", "\u04C9", "\u04CA", "\u04CB", "\u04CC", "\u04CD", "\u04CE", "\u04CF",
 "\u04D0", "\u04D1", "\u04D2", "\u04D3", "\u04D4", "\u04D5", "\u04D6", "\u04D7", "\u04D8", "\u04D9", "\u04DA", "\u04DB", "\u04DC", "\u04DD", "\u04DE", "\u04DF",
 "\u04E0", "\u04E1", "\u04E2", "\u04E3", "\u04E4", "\u04E5", "\u04E6", "\u04E7", "\u04E8", "\u04E9", "\u04EA", "\u04EB", "\u04EC", "\u04ED", "\u04EE", "\u04EF",
 "\u04F0", "\u04F1", "\u04F2", "\u04F3", "\u04F4", "\u04F5", "\u04F6", "\u04F7", "\u04F8", "\u04F9", "\u04fA", "\u04FB", "\u04FC", "\u04FD", "\u04FE", "\u04FF"
};
std::string s_ipa_extensions_font_sample[96] = {
 "\u0250", "\u0251", "\u0252", "\u0253", "\u0254", "\u0255", "\u0256", "\u0257", "\u0258", "\u0259", "\u025A", "\u025B", "\u025C", "\u025D", "\u025E", "\u025F",
 "\u0260", "\u0261", "\u0262", "\u0263", "\u0264", "\u0265", "\u0266", "\u0267", "\u0268", "\u0269", "\u026A", "\u026B", "\u026C", "\u026D", "\u026E", "\u026F",
 "\u0270", "\u0271", "\u0272", "\u0273", "\u0274", "\u0275", "\u0276", "\u0277", "\u0278", "\u0279", "\u027A", "\u027B", "\u027C", "\u027D", "\u027E", "\u027F",
 "\u0280", "\u0281", "\u0282", "\u0283", "\u0284", "\u0285", "\u0286", "\u0287", "\u0288", "\u0289", "\u028A", "\u028B", "\u028C", "\u028D", "\u028E", "\u028F",
 "\u0290", "\u0291", "\u0292", "\u0293", "\u0294", "\u0295", "\u0296", "\u0297", "\u0298", "\u0299", "\u029A", "\u029B", "\u029C", "\u029D", "\u029E", "\u029F",
 "\u02A0", "\u02A1", "\u02A2", "\u02A3", "\u02A4", "\u02A5", "\u02A6", "\u02A7", "\u02A8", "\u02A9", "\u02AA", "\u02AB", "\u02AC", "\u02AD", "\u02AE", "\u02AF",
};
std::string s_gurmukhi_font_sample[79] = {
	/*u0A00*/ "\u0A01", "\u0A02", "\u0A03", /*u0A04*/ "\u0A05", "\u0A06", "\u0A07", "\u0A08", "\u0A09", "\u0A0A", /*u0A0B     u0A0C     u0A0D     u0A0E*/ "\u0A0F",
	"\u0A10", /*u0A11     u0A12*/ "\u0A13", "\u0A14", "\u0A15", "\u0A16", "\u0A17", "\u0A18", "\u0A19", "\u0A1A", "\u0A1B", "\u0A1C", "\u0A1D", "\u0A1E", "\u0A1F",
	"\u0A20", "\u0A21", "\u0A22", "\u0A23", "\u0A24", "\u0A25", "\u0A26", "\u0A27", "\u0A28", /*u0A29*/ "\u0A2A", "\u0A2B", "\u0A2C", "\u0A2D", "\u0A2E", "\u0A2F",
	"\u0A30", /*u0A31*/ "\u0A32", "\u0A33", /*u0A34*/ "\u0A35", "\u0A36", /*u0A37*/ "\u0A38", "\u0A39", /*u0A3A     u0A3B*/ "\u0A3C", /*u0A3D*/ "\u0A3E", "\u0A3F",
	"\u0A40", "\u0A41", "\u0A42", /*u0A43     u0A44     u0A45     u0A46*/ "\u0A47", "\u0A48", /*u0A49     u0A4A*/ "\u0A4B", "\u0A4C", "\u0A4D", /*u0A4E     u0A4F*/
	/*u0A50*/ "\u0A51", /*u0A52     u0A53     u0A54     u0A55     u0A56     u0A57     u0A58*/ "\u0A59", "\u0A5A", "\u0A5B", "\u0A5C", /*u0A5D*/ "\u0A5E", /*u0A5F*/
	/*u0A60     u0A61     u0A62     u0A63     u0A64     u0A65*/ "\u0A66", "\u0A67", "\u0A68", "\u0A69", "\u0A6A", "\u0A6B", "\u0A6C", "\u0A6D", "\u0A6E", "\u0A6F",
	"\u0A70", "\u0A71", "\u0A72", "\u0A73", "\u0A74", "\u0A75", /*u0A76     u0A77     u0A78     u0A79     u0A7A     u0A7B     u0A7C     u0A7D     u0A7E     u0A7F*/
};
std::string s_general_punctuation_font_sample[111] = {
 "\u2000", "\u2001", "\u2002", "\u2003", "\u2004", "\u2005", "\u2006", "\u2007", "\u2008", "\u2009", "\u200A", "\u200B", "\u200C", "\u200D", "\u200E", "\u200F",
 "\u2010", "\u2011", "\u2012", "\u2013", "\u2014", "\u2015", "\u2016", "\u2017", "\u2018", "\u2019", "\u201A", "\u201B", "\u201C", "\u201D", "\u201E", "\u201F",
 "\u2020", "\u2021", "\u2022", "\u2023", "\u2024", "\u2025", "\u2026", "\u2027", "\u2028", "\u2029", "\u202A", "\u202B", "\u202C", "\u202D", "\u202E", "\u202F",
 "\u2030", "\u2031", "\u2032", "\u2033", "\u2034", "\u2035", "\u2036", "\u2037", "\u2038", "\u2039", "\u203A", "\u203B", "\u203C", "\u203D", "\u203E", "\u203F",
 "\u2040", "\u2041", "\u2042", "\u2043", "\u2044", "\u2045", "\u2046", "\u2047", "\u2048", "\u2049", "\u204A", "\u204B", "\u204C", "\u204D", "\u204E", "\u204F",
 "\u2050", "\u2051", "\u2052", "\u2053", "\u2054", "\u2055", "\u2056", "\u2057", "\u2058", "\u2059", "\u205A", "\u205B", "\u205C", "\u205D", "\u205E", "\u205F",
 "\u2060", "\u2061", "\u2062", "\u2063", "\u2064", /*u2065*/ "\u2066", "\u2067", "\u2068", "\u2069", "\u206A", "\u206B", "\u206C", "\u206D", "\u206E", "\u206F",
};
std::string s_block_elements_font_sample[32] = {
 "\u2580", "\u2581", "\u2582", "\u2583", "\u2584", "\u2585", "\u2586", "\u2587", "\u2588", "\u2589", "\u258A", "\u258B", "\u258C", "\u258D", "\u258E", "\u258F",
 "\u2590", "\u2591", "\u2592", "\u2593", "\u2594", "\u2595", "\u2596", "\u2597", "\u2598", "\u2599", "\u259A", "\u259B", "\u259C", "\u259D", "\u259E", "\u259F",
};
std::string s_phonetic_extensions_font_sample[128] = {
 "\u1D00", "\u1D01", "\u1D02", "\u1D03", "\u1D04", "\u1D05", "\u1D06", "\u1D07", "\u1D08", "\u1D09", "\u1D0A", "\u1D0B", "\u1D0C", "\u1D0D", "\u1D0E", "\u1D0F",
 "\u1D10", "\u1D11", "\u1D12", "\u1D13", "\u1D14", "\u1D15", "\u1D16", "\u1D17", "\u1D18", "\u1D19", "\u1D1A", "\u1D1B", "\u1D1C", "\u1D1D", "\u1D1E", "\u1D1F",
 "\u1D20", "\u1D21", "\u1D22", "\u1D23", "\u1D24", "\u1D25", "\u1D26", "\u1D27", "\u1D28", "\u1D29", "\u1D2A", "\u1D2B", "\u1D2C", "\u1D2D", "\u1D2E", "\u1D2F",
 "\u1D30", "\u1D31", "\u1D32", "\u1D33", "\u1D34", "\u1D35", "\u1D36", "\u1D37", "\u1D38", "\u1D39", "\u1D3A", "\u1D3B", "\u1D3C", "\u1D3D", "\u1D3E", "\u1D3F",
 "\u1D40", "\u1D41", "\u1D42", "\u1D43", "\u1D44", "\u1D45", "\u1D46", "\u1D47", "\u1D48", "\u1D49", "\u1D4A", "\u1D4B", "\u1D4C", "\u1D4D", "\u1D4E", "\u1D4F",
 "\u1D50", "\u1D51", "\u1D52", "\u1D53", "\u1D54", "\u1D55", "\u1D56", "\u1D57", "\u1D58", "\u1D59", "\u1D5A", "\u1D5B", "\u1D5C", "\u1D5D", "\u1D5E", "\u1D5F",
 "\u1D60", "\u1D61", "\u1D62", "\u1D63", "\u1D64", "\u1D65", "\u1D66", "\u1D67", "\u1D68", "\u1D69", "\u1D6A", "\u1D6B", "\u1D6C", "\u1D6D", "\u1D6E", "\u1D6F",
 "\u1D70", "\u1D71", "\u1D72", "\u1D73", "\u1D74", "\u1D75", "\u1D76", "\u1D77", "\u1D78", "\u1D79", "\u1D7A", "\u1D7B", "\u1D7C", "\u1D7D", "\u1D7E", "\u1D7F",
};
std::string s_thai_font_sample[87] = {
	/*u0E00"*/"\u0E01", "\u0E02", "\u0E03", "\u0E04", "\u0E05", "\u0E06", "\u0E07", "\u0E08", "\u0E09", "\u0E0A", "\u0E0B", "\u0E0C", "\u0E0D", "\u0E0E", "\u0E0F",
	"\u0E10", "\u0E11", "\u0E12", "\u0E13", "\u0E14", "\u0E15", "\u0E16", "\u0E17", "\u0E18", "\u0E19", "\u0E1A", "\u0E1B", "\u0E1C", "\u0E1D", "\u0E1E", "\u0E1F",
	"\u0E20", "\u0E21", "\u0E22", "\u0E23", "\u0E24", "\u0E25", "\u0E26", "\u0E27", "\u0E28", "\u0E29", "\u0E2A", "\u0E2B", "\u0E2C", "\u0E2D", "\u0E2E", "\u0E2F",
	"\u0E30", "\u0E31", "\u0E32", "\u0E33", "\u0E34", "\u0E35", "\u0E36", "\u0E37", "\u0E38", "\u0E39", "\u0E3A", /*u0E3B     u0E3C     u0E3D     u0E3E*/ "\u0E3F",
	"\u0E40", "\u0E41", "\u0E42", "\u0E43", "\u0E44", "\u0E45", "\u0E46", "\u0E47", "\u0E48", "\u0E49", "\u0E4A", "\u0E4B", "\u0E4C", "\u0E4D", "\u0E4E", "\u0E4F",
	"\u0E50", "\u0E51", "\u0E52", "\u0E53", "\u0E54", "\u0E55", "\u0E56", "\u0E57", "\u0E58", "\u0E59", "\u0E5A", "\u0E5B", /*u0E5C", "\u0E5D", "\u0E5E", "\u0E5F*/
};
std::string s_georgian_font_sample[88] = {
 "\u10A0", "\u10A1", "\u10A2", "\u10A3", "\u10A4", "\u10A5", "\u10A6", "\u10A7", "\u10A8", "\u10A9", "\u10AA", "\u10AB", "\u10AC", "\u10AD", "\u10AE", "\u10AF",
 "\u10B0", "\u10B1", "\u10B2", "\u10B3", "\u10B4", "\u10B5", "\u10B6", "\u10B7", "\u10B8", "\u10B9", "\u10BA", "\u10BB", "\u10BC", "\u10BD", "\u10BE", "\u10BF",
 "\u10C0", "\u10C1", "\u10C2", "\u10C3", "\u10C4", "\u10C5", /*u10C6*/ "\u10C7", /*u10C8     u10C9     u10CA     u10CB     u10CC*/ "\u10CD", /*u10CE     u10CF*/
 "\u10D0", "\u10D1", "\u10D2", "\u10D3", "\u10D4", "\u10D5", "\u10D6", "\u10D7", "\u10D8", "\u10D9", "\u10DA", "\u10DB", "\u10DC", "\u10DD", "\u10DE", "\u10DF",
 "\u10E0", "\u10E1", "\u10E2", "\u10E3", "\u10E4", "\u10E5", "\u10E6", "\u10E7", "\u10E8", "\u10E9", "\u10EA", "\u10EB", "\u10EC", "\u10ED", "\u10EE", "\u10EF",
 "\u10F0", "\u10F1", "\u10F2", "\u10F3", "\u10F4", "\u10F5", "\u10F6", "\u10F7", "\u10F8", "\u10F9", "\u10FA", "\u10FB", "\u10FC", "\u10FD", "\u10FE", "\u10FF",
};
std::string s_hangul_compatibility_jamo_font_sample[94] = {
	/*u3130*/ "\u3131", "\u3132", "\u3133", "\u3134", "\u3135", "\u3136", "\u3137", "\u3138", "\u3139", "\u313A", "\u313B", "\u313C", "\u313D", "\u313E", "\u313F",
	"\u3140", "\u3141", "\u3142", "\u3143", "\u3144", "\u3145", "\u3146", "\u3147", "\u3148", "\u3149", "\u314A", "\u314B", "\u314C", "\u314D", "\u314E", "\u314F",
	"\u3150", "\u3151", "\u3152", "\u3153", "\u3154", "\u3155", "\u3156", "\u3157", "\u3158", "\u3159", "\u315A", "\u315B", "\u315C", "\u315D", "\u315E", "\u315F",
	"\u3160", "\u3161", "\u3162", "\u3163", "\u3164", "\u3165", "\u3166", "\u3167", "\u3168", "\u3169", "\u316A", "\u316B", "\u316C", "\u316D", "\u316E", "\u316F",
	"\u3170", "\u3171", "\u3172", "\u3173", "\u3174", "\u3175", "\u3176", "\u3177", "\u3178", "\u3179", "\u317A", "\u317B", "\u317C", "\u317D", "\u317E", "\u317F",
	"\u3180", "\u3181", "\u3182", "\u3183", "\u3184", "\u3185", "\u3186", "\u3187", "\u3188", "\u3189", "\u318A", "\u318B", "\u318C", "\u318D", "\u318E", /*u318F*/
};
std::string s_miscellaneous_technical_font_sample[256] = {
 "\u2300", "\u2301", "\u2302", "\u2303", "\u2304", "\u2305", "\u2306", "\u2307", "\u2308", "\u2309", "\u230A", "\u230B", "\u230C", "\u230D", "\u230E", "\u230F",
 "\u2310", "\u2311", "\u2312", "\u2313", "\u2314", "\u2315", "\u2316", "\u2317", "\u2318", "\u2319", "\u231A", "\u231B", "\u231C", "\u231D", "\u231E", "\u231F",
 "\u2320", "\u2321", "\u2322", "\u2323", "\u2324", "\u2325", "\u2326", "\u2327", "\u2328", "\u2329", "\u232A", "\u232B", "\u232C", "\u232D", "\u232E", "\u232F",
 "\u2330", "\u2331", "\u2332", "\u2333", "\u2334", "\u2335", "\u2336", "\u2337", "\u2338", "\u2339", "\u233A", "\u233B", "\u233C", "\u233D", "\u233E", "\u233F",
 "\u2340", "\u2341", "\u2342", "\u2343", "\u2344", "\u2345", "\u2346", "\u2347", "\u2348", "\u2349", "\u234A", "\u234B", "\u234C", "\u234D", "\u234E", "\u234F",
 "\u2350", "\u2351", "\u2352", "\u2353", "\u2354", "\u2355", "\u2356", "\u2357", "\u2358", "\u2359", "\u235A", "\u235B", "\u235C", "\u235D", "\u235E", "\u235F",
 "\u2360", "\u2361", "\u2362", "\u2363", "\u2364", "\u2365", "\u2366", "\u2367", "\u2368", "\u2369", "\u236A", "\u236B", "\u236C", "\u236D", "\u236E", "\u236F",
 "\u2370", "\u2371", "\u2372", "\u2373", "\u2374", "\u2375", "\u2376", "\u2377", "\u2378", "\u2379", "\u237A", "\u237B", "\u237C", "\u237D", "\u237E", "\u237F",
 "\u2380", "\u2381", "\u2382", "\u2383", "\u2384", "\u2385", "\u2386", "\u2387", "\u2388", "\u2389", "\u238A", "\u238B", "\u238C", "\u238D", "\u238E", "\u238F",
 "\u2390", "\u2391", "\u2392", "\u2393", "\u2394", "\u2395", "\u2396", "\u2397", "\u2398", "\u2399", "\u239A", "\u239B", "\u239C", "\u239D", "\u239E", "\u239F",
 "\u23A0", "\u23A1", "\u23A2", "\u23A3", "\u23A4", "\u23A5", "\u23A6", "\u23A7", "\u23A8", "\u23A9", "\u23AA", "\u23AB", "\u23AC", "\u23AD", "\u23AE", "\u23AF",
 "\u23B0", "\u23B1", "\u23B2", "\u23B3", "\u23B4", "\u23B5", "\u23B6", "\u23B7", "\u23B8", "\u23B9", "\u23BA", "\u23BB", "\u23BC", "\u23BD", "\u23BE", "\u23BF",
 "\u23C0", "\u23C1", "\u23C2", "\u23C3", "\u23C4", "\u23C5", "\u23C6", "\u23C7", "\u23C8", "\u23C9", "\u23CA", "\u23CB", "\u23CC", "\u23CD", "\u23CE", "\u23CF",
 "\u23D0", "\u23D1", "\u23D2", "\u23D3", "\u23D4", "\u23D5", "\u23D6", "\u23D7", "\u23D8", "\u23D9", "\u23DA", "\u23DB", "\u23DC", "\u23DD", "\u23DE", "\u23DF",
 "\u23E0", "\u23E1", "\u23E2", "\u23E3", "\u23E4", "\u23E5", "\u23E6", "\u23E7", "\u23E8", "\u23E9", "\u23EA", "\u23EB", "\u23EC", "\u23ED", "\u23EE", "\u23EF",
 "\u23F0", "\u23F1", "\u23F2", "\u23F3", "\u23F4", "\u23F5", "\u23F6", "\u23F7", "\u23F8", "\u23F9", "\u23FA", "\u23FB", "\u23FC", "\u23FD", "\u23FE", "\u23FF",
};
std::string s_tamil_font_sample[72] = {
 "\u0B82", "\u0B83", "\u0B85", "\u0B86", "\u0B87", "\u0B88", "\u0B89", "\u0B8A", "\u0B8E", "\u0B8F",
 "\u0B90", "\u0B92", "\u0B93", "\u0B94", "\u0B95", "\u0B99", "\u0B9A", "\u0B9C", "\u0B9E", "\u0B9F",
 "\u0BA3", "\u0BA4", "\u0BA8", "\u0BA9", "\u0BAA", "\u0BAE", "\u0BAF",
 "\u0BB0", "\u0BB1", "\u0BB2", "\u0BB3", "\u0BB4", "\u0BB5", "\u0BB6", "\u0BB7", "\u0BB8", "\u0BB9", "\u0BBE", "\u0BBF",
 "\u0BC0", "\u0BC1", "\u0BC2", "\u0BC6", "\u0BC7", "\u0BC8", "\u0BCA", "\u0BCB", "\u0BCC", "\u0BCD",
 "\u0BD0", "\u0BD7",
 "\u0BE6", "\u0BE7", "\u0BE8", "\u0BE9", "\u0BEA", "\u0BEB", "\u0BEC", "\u0BED", "\u0BEE", "\u0BEF",
 "\u0BF0", "\u0BF1", "\u0BF2", "\u0BF3", "\u0BF4", "\u0BF5", "\u0BF6", "\u0BF7", "\u0BF8", "\u0BF9", "\u0BFA",
};
std::string s_spacing_modifier_letters_font_sample[80] = {
 "\u02B0", "\u02B1", "\u02B2", "\u02B3", "\u02B4", "\u02B5", "\u02B6", "\u02B7", "\u02B8", "\u02B9", "\u02BA", "\u02BB", "\u02BC", "\u02BD", "\u02BE", "\u02BF",
 "\u02C0", "\u02C1", "\u02C2", "\u02C3", "\u02C4", "\u02C5", "\u02C6", "\u02C7", "\u02C8", "\u02C9", "\u02CA", "\u02CB", "\u02CC", "\u02CD", "\u02CE", "\u02CF",
 "\u02D0", "\u02D1", "\u02D2", "\u02D3", "\u02D4", "\u02D5", "\u02D6", "\u02D7", "\u02D8", "\u02D9", "\u02DA", "\u02DB", "\u02DC", "\u02DD", "\u02DE", "\u02DF",
 "\u02E0", "\u02E1", "\u02E2", "\u02E3", "\u02E4", "\u02E5", "\u02E6", "\u02E7", "\u02E8", "\u02E9", "\u02EA", "\u02EB", "\u02EC", "\u02ED", "\u02EE", "\u02EF",
 "\u02F0", "\u02F1", "\u02F2", "\u02F3", "\u02F4", "\u02F5", "\u02F6", "\u02F7", "\u02F8", "\u02F9", "\u02FA", "\u02FB", "\u02FC", "\u02FD", "\u02FE", "\u02FF",
};
std::string s_tibetan_font_sample[211] = {
 "\u0F00", "\u0F01", "\u0F02", "\u0F03", "\u0F04", "\u0F05", "\u0F06", "\u0F07", "\u0F08", "\u0F09", "\u0F0A", "\u0F0B", "\u0F0C", "\u0F0D", "\u0F0E", "\u0F0F",
 "\u0F10", "\u0F11", "\u0F12", "\u0F13", "\u0F14", "\u0F15", "\u0F16", "\u0F17", "\u0F18", "\u0F19", "\u0F1A", "\u0F1B", "\u0F1C", "\u0F1D", "\u0F1E", "\u0F1F",
 "\u0F20", "\u0F21", "\u0F22", "\u0F23", "\u0F24", "\u0F25", "\u0F26", "\u0F27", "\u0F28", "\u0F29", "\u0F2A", "\u0F2B", "\u0F2C", "\u0F2D", "\u0F2E", "\u0F2F",
 "\u0F30", "\u0F31", "\u0F32", "\u0F33", "\u0F34", "\u0F35", "\u0F36", "\u0F37", "\u0F38", "\u0F39", "\u0F3A", "\u0F3B", "\u0F3C", "\u0F3D", "\u0F3E", "\u0F3F",
 "\u0F40", "\u0F41", "\u0F42", "\u0F43", "\u0F44", "\u0F45", "\u0F46", "\u0F47", "\u0F49", "\u0F4A", "\u0F4B", "\u0F4C", "\u0F4D", "\u0F4E", "\u0F4F",
 "\u0F50", "\u0F51", "\u0F52", "\u0F53", "\u0F54", "\u0F55", "\u0F56", "\u0F57", "\u0F58", "\u0F59", "\u0F5A", "\u0F5B", "\u0F5C", "\u0F5D", "\u0F5E", "\u0F5F",
 "\u0F60", "\u0F61", "\u0F62", "\u0F63", "\u0F64", "\u0F65", "\u0F66", "\u0F67", "\u0F68", "\u0F69", "\u0F6A", "\u0F6B", "\u0F6C",
 "\u0F71", "\u0F72", "\u0F73", "\u0F74", "\u0F75", "\u0F76", "\u0F77", "\u0F78", "\u0F79", "\u0F7A", "\u0F7B", "\u0F7C", "\u0F7D", "\u0F7E", "\u0F7F",
 "\u0F80", "\u0F81", "\u0F82", "\u0F83", "\u0F84", "\u0F85", "\u0F86", "\u0F87", "\u0F88", "\u0F89", "\u0F8A", "\u0F8B", "\u0F8C", "\u0F8D", "\u0F8E", "\u0F8F",
 "\u0F90", "\u0F91", "\u0F92", "\u0F93", "\u0F94", "\u0F95", "\u0F96", "\u0F97", "\u0F99", "\u0F9A", "\u0F9B", "\u0F9C", "\u0F9D", "\u0F9E", "\u0F9F",
 "\u0FA0", "\u0FA1", "\u0FA2", "\u0FA3", "\u0FA4", "\u0FA5", "\u0FA6", "\u0FA7", "\u0FA8", "\u0FA9", "\u0FAA", "\u0FAB", "\u0FAC", "\u0FAD", "\u0FAE", "\u0FAF",
 "\u0FB0", "\u0FB1", "\u0FB2", "\u0FB3", "\u0FB4", "\u0FB5", "\u0FB6", "\u0FB7", "\u0FB8", "\u0FB9", "\u0FBA", "\u0FBB", "\u0FBC", "\u0FBE", "\u0FBF",
 "\u0FC0", "\u0FC1", "\u0FC2", "\u0FC3", "\u0FC4", "\u0FC5", "\u0FC6", "\u0FC7", "\u0FC8", "\u0FC9", "\u0FCA", "\u0FCB", "\u0FCC", "\u0FCE", "\u0FCF",
 "\u0FD0", "\u0FD1", "\u0FD2", "\u0FD3", "\u0FD4", "\u0FD5", "\u0FD6", "\u0FD7", "\u0FD8", "\u0FD9", "\u0FDA",
};
std::string s_arrows_font_sample[112] = {
 "\u2190", "\u2191", "\u2192", "\u2193", "\u2194", "\u2195", "\u2196", "\u2197", "\u2198", "\u2199", "\u219A", "\u219B", "\u219C", "\u219D", "\u219E", "\u219F",
 "\u21A0", "\u21A1", "\u21A2", "\u21A3", "\u21A4", "\u21A5", "\u21A6", "\u21A7", "\u21A8", "\u21A9", "\u21AA", "\u21AB", "\u21AC", "\u21AD", "\u21AE", "\u21AF",
 "\u21B0", "\u21B1", "\u21B2", "\u21B3", "\u21B4", "\u21B5", "\u21B6", "\u21B7", "\u21B8", "\u21B9", "\u21BA", "\u21BB", "\u21BC", "\u21BD", "\u21BE", "\u21BF",
 "\u21C0", "\u21C1", "\u21C2", "\u21C3", "\u21C4", "\u21C5", "\u21C6", "\u21C7", "\u21C8", "\u21C9", "\u21CA", "\u21CB", "\u21CC", "\u21CD", "\u21CE", "\u21CF",
 "\u21D0", "\u21D1", "\u21D2", "\u21D3", "\u21D4", "\u21D5", "\u21D6", "\u21D7", "\u21D8", "\u21D9", "\u21DA", "\u21DB", "\u21DC", "\u21DD", "\u21DE", "\u21DF",
 "\u21E0", "\u21E1", "\u21E2", "\u21E3", "\u21E4", "\u21E5", "\u21E6", "\u21E7", "\u21E8", "\u21E9", "\u21EA", "\u21EB", "\u21EC", "\u21ED", "\u21EE", "\u21EF",
 "\u21F0", "\u21F1", "\u21F2", "\u21F3", "\u21F4", "\u21F5", "\u21F6", "\u21F7", "\u21F8", "\u21F9", "\u21FA", "\u21FB", "\u21FC", "\u21FD", "\u21FE", "\u21FF",
};
std::string s_cyrillic_supplement_font_sample[48] = {
 "\u0500", "\u0501", "\u0502", "\u0503", "\u0504", "\u0505", "\u0506", "\u0507", "\u0508", "\u0509", "\u050A", "\u050B", "\u050C", "\u050D", "\u050E", "\u050F",
 "\u0510", "\u0511", "\u0512", "\u0513", "\u0514", "\u0515", "\u0516", "\u0517", "\u0518", "\u0519", "\u051A", "\u051B", "\u051C", "\u051D", "\u051E", "\u051F",
 "\u0520", "\u0521", "\u0522", "\u0523", "\u0524", "\u0525", "\u0526", "\u0527", "\u0528", "\u0529", "\u052A", "\u052B", "\u052C", "\u052D", "\u052E", "\u052F",
};
std::string s_superscripts_and_subscripts_font_sample[42] = {
 "\u2070", "\u2071", "\u2074", "\u2075", "\u2076", "\u2077", "\u2078", "\u2079", "\u207A", "\u207B", "\u207C", "\u207D", "\u207E", "\u207F",
 "\u2080", "\u2081", "\u2082", "\u2083", "\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089", "\u208A", "\u208B", "\u208C", "\u208D", "\u208E",
 "\u2090", "\u2091", "\u2092", "\u2093", "\u2094", "\u2095", "\u2096", "\u2097", "\u2098", "\u2099", "\u209A", "\u209B", "\u209C",
};
std::string s_cjk_compatibility_forms_font_sample[42] = {
 "\uFE30", "\uFE31", "\uFE32", "\uFE33", "\uFE34", "\uFE35", "\uFE36", "\uFE37", "\uFE38", "\uFE39", "\uFE3A", "\uFE3B", "\uFE3C", "\uFE3D", "\uFE3E", "\uFE3F",
 "\uFE40", "\uFE41", "\uFE42", "\uFE43", "\uFE44", "\uFE45", "\uFE46", "\uFE47", "\uFE48", "\uFE49", "\uFE4A", "\uFE4B", "\uFE4C", "\uFE4D", "\uFE4E", "\uFE4F",
};
std::string s_combining_diacritical_marks_font_sample[112] = {
 "\u0300", "\u0301", "\u0302", "\u0303", "\u0304", "\u0305", "\u0306", "\u0307", "\u0308", "\u0309", "\u030A", "\u030B", "\u030C", "\u030D", "\u030E", "\u030F",
 "\u0310", "\u0311", "\u0312", "\u0313", "\u0314", "\u0315", "\u0316", "\u0317", "\u0318", "\u0319", "\u031A", "\u031B", "\u031C", "\u031D", "\u031E", "\u031F",
 "\u0320", "\u0321", "\u0322", "\u0323", "\u0324", "\u0325", "\u0326", "\u0327", "\u0328", "\u0329", "\u032A", "\u032B", "\u032C", "\u032D", "\u032E", "\u032F",
 "\u0330", "\u0331", "\u0332", "\u0333", "\u0334", "\u0335", "\u0336", "\u0337", "\u0338", "\u0339", "\u033A", "\u033B", "\u033C", "\u033D", "\u033E", "\u033F",
 "\u0340", "\u0341", "\u0342", "\u0343", "\u0344", "\u0345", "\u0346", "\u0347", "\u0348", "\u0349", "\u034A", "\u034B", "\u034C", "\u034D", "\u034E", "\u034F",
 "\u0350", "\u0351", "\u0352", "\u0353", "\u0354", "\u0355", "\u0356", "\u0357", "\u0358", "\u0359", "\u035A", "\u035B", "\u035C", "\u035D", "\u035E", "\u035F",
 "\u0360", "\u0361", "\u0362", "\u0363", "\u0364", "\u0365", "\u0366", "\u0367", "\u0368", "\u0369", "\u036A", "\u036B", "\u036C", "\u036D", "\u036E", "\u036F",
};
std::string s_optical_character_recognition_font_sample[11] = {
 "\u2440", "\u2441", "\u2442", "\u2443", "\u2444", "\u2445", "\u2446", "\u2447", "\u2448", "\u2449", "\u244A",
};
std::string s_greek_extended_font_sample[233] = {
 "\u1F00", "\u1F01", "\u1F02", "\u1F03", "\u1F04", "\u1F05", "\u1F06", "\u1F07", "\u1F08", "\u1F09", "\u1F0A", "\u1F0B", "\u1F0C", "\u1F0D", "\u1F0E", "\u1F0F",
 "\u1F10", "\u1F11", "\u1F12", "\u1F13", "\u1F14", "\u1F15", "\u1F18", "\u1F19", "\u1F1A", "\u1F1B", "\u1F1C", "\u1F1D",
 "\u1F20", "\u1F21", "\u1F22", "\u1F23", "\u1F24", "\u1F25", "\u1F26", "\u1F27", "\u1F28", "\u1F29", "\u1F2A", "\u1F2B", "\u1F2C", "\u1F2D", "\u1F2E", "\u1F2F",
 "\u1F30", "\u1F31", "\u1F32", "\u1F33", "\u1F34", "\u1F35", "\u1F36", "\u1F37", "\u1F38", "\u1F39", "\u1F3A", "\u1F3B", "\u1F3C", "\u1F3D", "\u1F3E", "\u1F3F",
 "\u1F40", "\u1F41", "\u1F42", "\u1F43", "\u1F44", "\u1F45", "\u1F48", "\u1F49", "\u1F4A", "\u1F4B", "\u1F4C", "\u1F4D",
 "\u1F50", "\u1F51", "\u1F52", "\u1F53", "\u1F54", "\u1F55", "\u1F56", "\u1F57", "\u1F59", "\u1F5B", "\u1F5D", "\u1F5F",
 "\u1F60", "\u1F61", "\u1F62", "\u1F63", "\u1F64", "\u1F65", "\u1F66", "\u1F67", "\u1F68", "\u1F69", "\u1F6A", "\u1F6B", "\u1F6C", "\u1F6D", "\u1F6E", "\u1F6F",
 "\u1F70", "\u1F71", "\u1F72", "\u1F73", "\u1F74", "\u1F75", "\u1F76", "\u1F77", "\u1F78", "\u1F79", "\u1F7A", "\u1F7B", "\u1F7C", "\u1F7D",
 "\u1F80", "\u1F81", "\u1F82", "\u1F83", "\u1F84", "\u1F85", "\u1F86", "\u1F87", "\u1F88", "\u1F89", "\u1F8A", "\u1F8B", "\u1F8C", "\u1F8D", "\u1F8E", "\u1F8F",
 "\u1F90", "\u1F91", "\u1F92", "\u1F93", "\u1F94", "\u1F95", "\u1F96", "\u1F97", "\u1F98", "\u1F99", "\u1F9A", "\u1F9B", "\u1F9C", "\u1F9D", "\u1F9E", "\u1F9F",
 "\u1FA0", "\u1FA1", "\u1FA2", "\u1FA3", "\u1FA4", "\u1FA5", "\u1FA6", "\u1FA7", "\u1FA8", "\u1FA9", "\u1FAA", "\u1FAB", "\u1FAC", "\u1FAD", "\u1FAE", "\u1FAF",
 "\u1FB0", "\u1FB1", "\u1FB2", "\u1FB3", "\u1FB4", "\u1FB6", "\u1FB7", "\u1FB8", "\u1FB9", "\u1FBA", "\u1FBB", "\u1FBC", "\u1FBD", "\u1FBE", "\u1FBF",
 "\u1FC0", "\u1FC1", "\u1FC2", "\u1FC3", "\u1FC4", "\u1FC6", "\u1FC7", "\u1FC8", "\u1FC9", "\u1FCA", "\u1FCB", "\u1FCC", "\u1FCD", "\u1FCE", "\u1FCF",
 "\u1FD0", "\u1FD1", "\u1FD2", "\u1FD3", "\u1FD6", "\u1FD7", "\u1FD8", "\u1FD9", "\u1FDA", "\u1FDB", "\u1FDD", "\u1FDE", "\u1FDF",
 "\u1FE0", "\u1FE1", "\u1FE2", "\u1FE3", "\u1FE4", "\u1FE5", "\u1FE6", "\u1FE7", "\u1FE8", "\u1FE9", "\u1FEA", "\u1FEB", "\u1FEC", "\u1FED", "\u1FEE", "\u1FEF",
 "\u1FF2", "\u1FF3", "\u1FF4", "\u1FF6", "\u1FF7", "\u1FF8", "\u1FF9", "\u1FFA", "\u1FFB", "\u1FFC", "\u1FFD", "\u1FFE",

};
std::string s_miscellaneous_symbols_and_pictographs_font_sample[768] = {
 "\U0001F300", "\U0001F301", "\U0001F302", "\U0001F303", "\U0001F304", "\U0001F305", "\U0001F306", "\U0001F307", "\U0001F308", "\U0001F309", "\U0001F30A", "\U0001F30B", "\U0001F30C", "\U0001F30D", "\U0001F30E", "\U0001F30F",
 "\U0001F310", "\U0001F311", "\U0001F312", "\U0001F313", "\U0001F314", "\U0001F315", "\U0001F316", "\U0001F317", "\U0001F318", "\U0001F319", "\U0001F31A", "\U0001F31B", "\U0001F31C", "\U0001F31D", "\U0001F31E", "\U0001F31F",
 "\U0001F320", "\U0001F321", "\U0001F322", "\U0001F323", "\U0001F324", "\U0001F325", "\U0001F326", "\U0001F327", "\U0001F328", "\U0001F329", "\U0001F32A", "\U0001F32B", "\U0001F32C", "\U0001F32D", "\U0001F32E", "\U0001F32F",
 "\U0001F330", "\U0001F331", "\U0001F332", "\U0001F333", "\U0001F334", "\U0001F335", "\U0001F336", "\U0001F337", "\U0001F338", "\U0001F339", "\U0001F33A", "\U0001F33B", "\U0001F33C", "\U0001F33D", "\U0001F33E", "\U0001F33F",
 "\U0001F340", "\U0001F341", "\U0001F342", "\U0001F343", "\U0001F344", "\U0001F345", "\U0001F346", "\U0001F347", "\U0001F348", "\U0001F349", "\U0001F34A", "\U0001F34B", "\U0001F34C", "\U0001F34D", "\U0001F34E", "\U0001F34F",
 "\U0001F350", "\U0001F351", "\U0001F352", "\U0001F353", "\U0001F354", "\U0001F355", "\U0001F356", "\U0001F357", "\U0001F358", "\U0001F359", "\U0001F35A", "\U0001F35B", "\U0001F35C", "\U0001F35D", "\U0001F35E", "\U0001F35F",
 "\U0001F360", "\U0001F361", "\U0001F362", "\U0001F363", "\U0001F364", "\U0001F365", "\U0001F366", "\U0001F367", "\U0001F368", "\U0001F369", "\U0001F36A", "\U0001F36B", "\U0001F36C", "\U0001F36D", "\U0001F36E", "\U0001F36F",
 "\U0001F370", "\U0001F371", "\U0001F372", "\U0001F373", "\U0001F374", "\U0001F375", "\U0001F376", "\U0001F377", "\U0001F378", "\U0001F379", "\U0001F37A", "\U0001F37B", "\U0001F37C", "\U0001F37D", "\U0001F37E", "\U0001F37F",
 "\U0001F380", "\U0001F381", "\U0001F382", "\U0001F383", "\U0001F384", "\U0001F385", "\U0001F386", "\U0001F387", "\U0001F388", "\U0001F389", "\U0001F38A", "\U0001F38B", "\U0001F38C", "\U0001F38D", "\U0001F38E", "\U0001F38F",
 "\U0001F390", "\U0001F391", "\U0001F392", "\U0001F393", "\U0001F394", "\U0001F395", "\U0001F396", "\U0001F397", "\U0001F398", "\U0001F399", "\U0001F39A", "\U0001F39B", "\U0001F39C", "\U0001F39D", "\U0001F39E", "\U0001F39F",
 "\U0001F3A0", "\U0001F3A1", "\U0001F3A2", "\U0001F3A3", "\U0001F3A4", "\U0001F3A5", "\U0001F3A6", "\U0001F3A7", "\U0001F3A8", "\U0001F3A9", "\U0001F3AA", "\U0001F3AB", "\U0001F3AC", "\U0001F3AD", "\U0001F3AE", "\U0001F3AF",
 "\U0001F3B0", "\U0001F3B1", "\U0001F3B2", "\U0001F3B3", "\U0001F3B4", "\U0001F3B5", "\U0001F3B6", "\U0001F3B7", "\U0001F3B8", "\U0001F3B9", "\U0001F3BA", "\U0001F3BB", "\U0001F3BC", "\U0001F3BD", "\U0001F3BE", "\U0001F3BF",
 "\U0001F3C0", "\U0001F3C1", "\U0001F3C2", "\U0001F3C3", "\U0001F3C4", "\U0001F3C5", "\U0001F3C6", "\U0001F3C7", "\U0001F3C8", "\U0001F3C9", "\U0001F3CA", "\U0001F3CB", "\U0001F3CC", "\U0001F3CD", "\U0001F3CE", "\U0001F3CF",
 "\U0001F3D0", "\U0001F3D1", "\U0001F3D2", "\U0001F3D3", "\U0001F3D4", "\U0001F3D5", "\U0001F3D6", "\U0001F3D7", "\U0001F3D8", "\U0001F3D9", "\U0001F3DA", "\U0001F3DB", "\U0001F3DC", "\U0001F3DD", "\U0001F3DE", "\U0001F3DF",
 "\U0001F3E0", "\U0001F3E1", "\U0001F3E2", "\U0001F3E3", "\U0001F3E4", "\U0001F3E5", "\U0001F3E6", "\U0001F3E7", "\U0001F3E8", "\U0001F3E9", "\U0001F3EA", "\U0001F3EB", "\U0001F3EC", "\U0001F3ED", "\U0001F3EE", "\U0001F3EF",
 "\U0001F3F0", "\U0001F3F1", "\U0001F3F2", "\U0001F3F3", "\U0001F3F4", "\U0001F3F5", "\U0001F3F6", "\U0001F3F7", "\U0001F3F8", "\U0001F3F9", "\U0001F3FA", "\U0001F3FB", "\U0001F3FC", "\U0001F3FD", "\U0001F3FE", "\U0001F3FF",
 "\U0001F400", "\U0001F401", "\U0001F402", "\U0001F403", "\U0001F404", "\U0001F405", "\U0001F406", "\U0001F407", "\U0001F408", "\U0001F409", "\U0001F40A", "\U0001F40B", "\U0001F40C", "\U0001F40D", "\U0001F40E", "\U0001F40F",
 "\U0001F410", "\U0001F411", "\U0001F412", "\U0001F413", "\U0001F414", "\U0001F415", "\U0001F416", "\U0001F417", "\U0001F418", "\U0001F419", "\U0001F41A", "\U0001F41B", "\U0001F41C", "\U0001F41D", "\U0001F41E", "\U0001F41F",
 "\U0001F420", "\U0001F421", "\U0001F422", "\U0001F423", "\U0001F424", "\U0001F425", "\U0001F426", "\U0001F427", "\U0001F428", "\U0001F429", "\U0001F42A", "\U0001F42B", "\U0001F42C", "\U0001F42D", "\U0001F42E", "\U0001F42F",
 "\U0001F430", "\U0001F431", "\U0001F432", "\U0001F433", "\U0001F434", "\U0001F435", "\U0001F436", "\U0001F437", "\U0001F438", "\U0001F439", "\U0001F43A", "\U0001F43B", "\U0001F43C", "\U0001F43D", "\U0001F43E", "\U0001F43F",
 "\U0001F440", "\U0001F441", "\U0001F442", "\U0001F443", "\U0001F444", "\U0001F445", "\U0001F446", "\U0001F447", "\U0001F448", "\U0001F449", "\U0001F44A", "\U0001F44B", "\U0001F44C", "\U0001F44D", "\U0001F44E", "\U0001F44F",
 "\U0001F450", "\U0001F451", "\U0001F452", "\U0001F453", "\U0001F454", "\U0001F455", "\U0001F456", "\U0001F457", "\U0001F458", "\U0001F459", "\U0001F45A", "\U0001F45B", "\U0001F45C", "\U0001F45D", "\U0001F45E", "\U0001F45F",
 "\U0001F460", "\U0001F461", "\U0001F462", "\U0001F463", "\U0001F464", "\U0001F465", "\U0001F466", "\U0001F467", "\U0001F468", "\U0001F469", "\U0001F46A", "\U0001F46B", "\U0001F46C", "\U0001F46D", "\U0001F46E", "\U0001F46F",
 "\U0001F470", "\U0001F471", "\U0001F472", "\U0001F473", "\U0001F474", "\U0001F475", "\U0001F476", "\U0001F477", "\U0001F478", "\U0001F479", "\U0001F47A", "\U0001F47B", "\U0001F47C", "\U0001F47D", "\U0001F47E", "\U0001F47F",
 "\U0001F480", "\U0001F481", "\U0001F482", "\U0001F483", "\U0001F484", "\U0001F485", "\U0001F486", "\U0001F487", "\U0001F488", "\U0001F489", "\U0001F48A", "\U0001F48B", "\U0001F48C", "\U0001F48D", "\U0001F48E", "\U0001F48F",
 "\U0001F490", "\U0001F491", "\U0001F492", "\U0001F493", "\U0001F494", "\U0001F495", "\U0001F496", "\U0001F497", "\U0001F498", "\U0001F499", "\U0001F49A", "\U0001F49B", "\U0001F49C", "\U0001F49D", "\U0001F49E", "\U0001F49F",
 "\U0001F4A0", "\U0001F4A1", "\U0001F4A2", "\U0001F4A3", "\U0001F4A4", "\U0001F4A5", "\U0001F4A6", "\U0001F4A7", "\U0001F4A8", "\U0001F4A9", "\U0001F4AA", "\U0001F4AB", "\U0001F4AC", "\U0001F4AD", "\U0001F4AE", "\U0001F4AF",
 "\U0001F4B0", "\U0001F4B1", "\U0001F4B2", "\U0001F4B3", "\U0001F4B4", "\U0001F4B5", "\U0001F4B6", "\U0001F4B7", "\U0001F4B8", "\U0001F4B9", "\U0001F4BA", "\U0001F4BB", "\U0001F4BC", "\U0001F4BD", "\U0001F4BE", "\U0001F4BF",
 "\U0001F4C0", "\U0001F4C1", "\U0001F4C2", "\U0001F4C3", "\U0001F4C4", "\U0001F4C5", "\U0001F4C6", "\U0001F4C7", "\U0001F4C8", "\U0001F4C9", "\U0001F4CA", "\U0001F4CB", "\U0001F4CC", "\U0001F4CD", "\U0001F4CE", "\U0001F4CF",
 "\U0001F4D0", "\U0001F4D1", "\U0001F4D2", "\U0001F4D3", "\U0001F4D4", "\U0001F4D5", "\U0001F4D6", "\U0001F4D7", "\U0001F4D8", "\U0001F4D9", "\U0001F4DA", "\U0001F4DB", "\U0001F4DC", "\U0001F4DD", "\U0001F4DE", "\U0001F4DF",
 "\U0001F4E0", "\U0001F4E1", "\U0001F4E2", "\U0001F4E3", "\U0001F4E4", "\U0001F4E5", "\U0001F4E6", "\U0001F4E7", "\U0001F4E8", "\U0001F4E9", "\U0001F4EA", "\U0001F4EB", "\U0001F4EC", "\U0001F4ED", "\U0001F4EE", "\U0001F4EF",
 "\U0001F4F0", "\U0001F4F1", "\U0001F4F2", "\U0001F4F3", "\U0001F4F4", "\U0001F4F5", "\U0001F4F6", "\U0001F4F7", "\U0001F4F8", "\U0001F4F9", "\U0001F4FA", "\U0001F4FB", "\U0001F4FC", "\U0001F4FD", "\U0001F4FE", "\U0001F4FF",
 "\U0001F500", "\U0001F501", "\U0001F502", "\U0001F503", "\U0001F504", "\U0001F505", "\U0001F506", "\U0001F507", "\U0001F508", "\U0001F509", "\U0001F50A", "\U0001F50B", "\U0001F50C", "\U0001F50D", "\U0001F50E", "\U0001F50F",
 "\U0001F510", "\U0001F511", "\U0001F512", "\U0001F513", "\U0001F514", "\U0001F515", "\U0001F516", "\U0001F517", "\U0001F518", "\U0001F519", "\U0001F51A", "\U0001F51B", "\U0001F51C", "\U0001F51D", "\U0001F51E", "\U0001F51F",
 "\U0001F520", "\U0001F521", "\U0001F522", "\U0001F523", "\U0001F524", "\U0001F525", "\U0001F526", "\U0001F527", "\U0001F528", "\U0001F529", "\U0001F52A", "\U0001F52B", "\U0001F52C", "\U0001F52D", "\U0001F52E", "\U0001F52F",
 "\U0001F530", "\U0001F531", "\U0001F532", "\U0001F533", "\U0001F534", "\U0001F535", "\U0001F536", "\U0001F537", "\U0001F538", "\U0001F539", "\U0001F53A", "\U0001F53B", "\U0001F53C", "\U0001F53D", "\U0001F53E", "\U0001F53F",
 "\U0001F540", "\U0001F541", "\U0001F542", "\U0001F543", "\U0001F544", "\U0001F545", "\U0001F546", "\U0001F547", "\U0001F548", "\U0001F549", "\U0001F54A", "\U0001F54B", "\U0001F54C", "\U0001F54D", "\U0001F54E", "\U0001F54F",
 "\U0001F550", "\U0001F551", "\U0001F552", "\U0001F553", "\U0001F554", "\U0001F555", "\U0001F556", "\U0001F557", "\U0001F558", "\U0001F559", "\U0001F55A", "\U0001F55B", "\U0001F55C", "\U0001F55D", "\U0001F55E", "\U0001F55F",
 "\U0001F560", "\U0001F561", "\U0001F562", "\U0001F563", "\U0001F564", "\U0001F565", "\U0001F566", "\U0001F567", "\U0001F568", "\U0001F569", "\U0001F56A", "\U0001F56B", "\U0001F56C", "\U0001F56D", "\U0001F56E", "\U0001F56F",
 "\U0001F570", "\U0001F571", "\U0001F572", "\U0001F573", "\U0001F574", "\U0001F575", "\U0001F576", "\U0001F577", "\U0001F578", "\U0001F579", "\U0001F57A", "\U0001F57B", "\U0001F57C", "\U0001F57D", "\U0001F57E", "\U0001F57F",
 "\U0001F580", "\U0001F581", "\U0001F582", "\U0001F583", "\U0001F584", "\U0001F585", "\U0001F586", "\U0001F587", "\U0001F588", "\U0001F589", "\U0001F58A", "\U0001F58B", "\U0001F58C", "\U0001F58D", "\U0001F58E", "\U0001F58F",
 "\U0001F590", "\U0001F591", "\U0001F592", "\U0001F593", "\U0001F594", "\U0001F595", "\U0001F596", "\U0001F597", "\U0001F598", "\U0001F599", "\U0001F59A", "\U0001F59B", "\U0001F59C", "\U0001F59D", "\U0001F59E", "\U0001F59F",
 "\U0001F5A0", "\U0001F5A1", "\U0001F5A2", "\U0001F5A3", "\U0001F5A4", "\U0001F5A5", "\U0001F5A6", "\U0001F5A7", "\U0001F5A8", "\U0001F5A9", "\U0001F5AA", "\U0001F5AB", "\U0001F5AC", "\U0001F5AD", "\U0001F5AE", "\U0001F5AF",
 "\U0001F5B0", "\U0001F5B1", "\U0001F5B2", "\U0001F5B3", "\U0001F5B4", "\U0001F5B5", "\U0001F5B6", "\U0001F5B7", "\U0001F5B8", "\U0001F5B9", "\U0001F5BA", "\U0001F5BB", "\U0001F5BC", "\U0001F5BD", "\U0001F5BE", "\U0001F5BF",
 "\U0001F5C0", "\U0001F5C1", "\U0001F5C2", "\U0001F5C3", "\U0001F5C4", "\U0001F5C5", "\U0001F5C6", "\U0001F5C7", "\U0001F5C8", "\U0001F5C9", "\U0001F5CA", "\U0001F5CB", "\U0001F5CC", "\U0001F5CD", "\U0001F5CE", "\U0001F5CF",
 "\U0001F5D0", "\U0001F5D1", "\U0001F5D2", "\U0001F5D3", "\U0001F5D4", "\U0001F5D5", "\U0001F5D6", "\U0001F5D7", "\U0001F5D8", "\U0001F5D9", "\U0001F5DA", "\U0001F5DB", "\U0001F5DC", "\U0001F5DD", "\U0001F5DE", "\U0001F5DF",
 "\U0001F5E0", "\U0001F5E1", "\U0001F5E2", "\U0001F5E3", "\U0001F5E4", "\U0001F5E5", "\U0001F5E6", "\U0001F5E7", "\U0001F5E8", "\U0001F5E9", "\U0001F5EA", "\U0001F5EB", "\U0001F5EC", "\U0001F5ED", "\U0001F5EE", "\U0001F5EF",
 "\U0001F5F0", "\U0001F5F1", "\U0001F5F2", "\U0001F5F3", "\U0001F5F4", "\U0001F5F5", "\U0001F5F6", "\U0001F5F7", "\U0001F5F8", "\U0001F5F9", "\U0001F5FA", "\U0001F5FB", "\U0001F5FC", "\U0001F5FD", "\U0001F5FE", "\U0001F5FF",
};
std::string s_unified_canadian_aboriginal_syllabics_font_sample[640] = {
 "\u1400", "\u1401", "\u1402", "\u1403", "\u1404", "\u1405", "\u1406", "\u1407", "\u1408", "\u1409", "\u140A", "\u140B", "\u140C", "\u140D", "\u140E", "\u140F",
 "\u1410", "\u1411", "\u1412", "\u1413", "\u1414", "\u1415", "\u1416", "\u1417", "\u1418", "\u1419", "\u141A", "\u141B", "\u141C", "\u141D", "\u141E", "\u141F",
 "\u1420", "\u1421", "\u1422", "\u1423", "\u1424", "\u1425", "\u1426", "\u1427", "\u1428", "\u1429", "\u142A", "\u142B", "\u142C", "\u142D", "\u142E", "\u142F",
 "\u1430", "\u1431", "\u1432", "\u1433", "\u1434", "\u1435", "\u1436", "\u1437", "\u1438", "\u1439", "\u143A", "\u143B", "\u143C", "\u143D", "\u143E", "\u143F",
 "\u1440", "\u1441", "\u1442", "\u1443", "\u1444", "\u1445", "\u1446", "\u1447", "\u1448", "\u1449", "\u144A", "\u144B", "\u144C", "\u144D", "\u144E", "\u144F",
 "\u1450", "\u1451", "\u1452", "\u1453", "\u1454", "\u1455", "\u1456", "\u1457", "\u1458", "\u1459", "\u145A", "\u145B", "\u145C", "\u145D", "\u145E", "\u145F",
 "\u1460", "\u1461", "\u1462", "\u1463", "\u1464", "\u1465", "\u1466", "\u1467", "\u1468", "\u1469", "\u146A", "\u146B", "\u146C", "\u146D", "\u146E", "\u146F",
 "\u1470", "\u1471", "\u1472", "\u1473", "\u1474", "\u1475", "\u1476", "\u1477", "\u1478", "\u1479", "\u147A", "\u147B", "\u147C", "\u147D", "\u147E", "\u147F",
 "\u1480", "\u1481", "\u1482", "\u1483", "\u1484", "\u1485", "\u1486", "\u1487", "\u1488", "\u1489", "\u148A", "\u148B", "\u148C", "\u148D", "\u148E", "\u148F",
 "\u1490", "\u1491", "\u1492", "\u1493", "\u1494", "\u1495", "\u1496", "\u1497", "\u1498", "\u1499", "\u149A", "\u149B", "\u149C", "\u149D", "\u149E", "\u149F",
 "\u14A0", "\u14A1", "\u14A2", "\u14A3", "\u14A4", "\u14A5", "\u14A6", "\u14A7", "\u14A8", "\u14A9", "\u14AA", "\u14AB", "\u14AC", "\u14AD", "\u14AE", "\u14AF",
 "\u14B0", "\u14B1", "\u14B2", "\u14B3", "\u14B4", "\u14B5", "\u14B6", "\u14B7", "\u14B8", "\u14B9", "\u14BA", "\u14BB", "\u14BC", "\u14BD", "\u14BE", "\u14BF",
 "\u14C0", "\u14C1", "\u14C2", "\u14C3", "\u14C4", "\u14C5", "\u14C6", "\u14C7", "\u14C8", "\u14C9", "\u14CA", "\u14CB", "\u14CC", "\u14CD", "\u14CE", "\u14CF",
 "\u14D0", "\u14D1", "\u14D2", "\u14D3", "\u14D4", "\u14D5", "\u14D6", "\u14D7", "\u14D8", "\u14D9", "\u14DA", "\u14DB", "\u14DC", "\u14DD", "\u14DE", "\u14DF",
 "\u14E0", "\u14E1", "\u14E2", "\u14E3", "\u14E4", "\u14E5", "\u14E6", "\u14E7", "\u14E8", "\u14E9", "\u14EA", "\u14EB", "\u14EC", "\u14ED", "\u14EE", "\u14EF",
 "\u14F0", "\u14F1", "\u14F2", "\u14F3", "\u14F4", "\u14F5", "\u14F6", "\u14F7", "\u14F8", "\u14F9", "\u14FA", "\u14FB", "\u14FC", "\u14FD", "\u14FE", "\u14FF",
 "\u1500", "\u1501", "\u1502", "\u1503", "\u1504", "\u1505", "\u1506", "\u1507", "\u1508", "\u1509", "\u150A", "\u150B", "\u150C", "\u150D", "\u150E", "\u150F",
 "\u1510", "\u1511", "\u1512", "\u1513", "\u1514", "\u1515", "\u1516", "\u1517", "\u1518", "\u1519", "\u151A", "\u151B", "\u151C", "\u151D", "\u151E", "\u151F",
 "\u1520", "\u1521", "\u1522", "\u1523", "\u1524", "\u1525", "\u1526", "\u1527", "\u1528", "\u1529", "\u152A", "\u152B", "\u152C", "\u152D", "\u152E", "\u152F",
 "\u1530", "\u1531", "\u1532", "\u1533", "\u1534", "\u1535", "\u1536", "\u1537", "\u1538", "\u1539", "\u153A", "\u153B", "\u153C", "\u153D", "\u153E", "\u153F",
 "\u1540", "\u1541", "\u1542", "\u1543", "\u1544", "\u1545", "\u1546", "\u1547", "\u1548", "\u1549", "\u154A", "\u154B", "\u154C", "\u154D", "\u154E", "\u154F",
 "\u1550", "\u1551", "\u1552", "\u1553", "\u1554", "\u1555", "\u1556", "\u1557", "\u1558", "\u1559", "\u155A", "\u155B", "\u155C", "\u155D", "\u155E", "\u155F",
 "\u1560", "\u1561", "\u1562", "\u1563", "\u1564", "\u1565", "\u1566", "\u1567", "\u1568", "\u1569", "\u156A", "\u156B", "\u156C", "\u156D", "\u156E", "\u156F",
 "\u1570", "\u1571", "\u1572", "\u1573", "\u1574", "\u1575", "\u1576", "\u1577", "\u1578", "\u1579", "\u157A", "\u157B", "\u157C", "\u157D", "\u157E", "\u157F",
 "\u1580", "\u1581", "\u1582", "\u1583", "\u1584", "\u1585", "\u1586", "\u1587", "\u1588", "\u1589", "\u158A", "\u158B", "\u158C", "\u158D", "\u158E", "\u158F",
 "\u1590", "\u1591", "\u1592", "\u1593", "\u1594", "\u1595", "\u1596", "\u1597", "\u1598", "\u1599", "\u159A", "\u159B", "\u159C", "\u159D", "\u159E", "\u159F",
 "\u15A0", "\u15A1", "\u15A2", "\u15A3", "\u15A4", "\u15A5", "\u15A6", "\u15A7", "\u15A8", "\u15A9", "\u15AA", "\u15AB", "\u15AC", "\u15AD", "\u15AE", "\u15AF",
 "\u15B0", "\u15B1", "\u15B2", "\u15B3", "\u15B4", "\u15B5", "\u15B6", "\u15B7", "\u15B8", "\u15B9", "\u15BA", "\u15BB", "\u15BC", "\u15BD", "\u15BE", "\u15BF",
 "\u15C0", "\u15C1", "\u15C2", "\u15C3", "\u15C4", "\u15C5", "\u15C6", "\u15C7", "\u15C8", "\u15C9", "\u15CA", "\u15CB", "\u15CC", "\u15CD", "\u15CE", "\u15CF",
 "\u15D0", "\u15D1", "\u15D2", "\u15D3", "\u15D4", "\u15D5", "\u15D6", "\u15D7", "\u15D8", "\u15D9", "\u15DA", "\u15DB", "\u15DC", "\u15DD", "\u15DE", "\u15DF",
 "\u15E0", "\u15E1", "\u15E2", "\u15E3", "\u15E4", "\u15E5", "\u15E6", "\u15E7", "\u15E8", "\u15E9", "\u15EA", "\u15EB", "\u15EC", "\u15ED", "\u15EE", "\u15EF",
 "\u15F0", "\u15F1", "\u15F2", "\u15F3", "\u15F4", "\u15F5", "\u15F6", "\u15F7", "\u15F8", "\u15F9", "\u15FA", "\u15FB", "\u15FC", "\u15FD", "\u15FE", "\u15FF",
 "\u1600", "\u1601", "\u1602", "\u1603", "\u1604", "\u1605", "\u1606", "\u1607", "\u1608", "\u1609", "\u160A", "\u160B", "\u160C", "\u160D", "\u160E", "\u160F",
 "\u1610", "\u1611", "\u1612", "\u1613", "\u1614", "\u1615", "\u1616", "\u1617", "\u1618", "\u1619", "\u161A", "\u161B", "\u161C", "\u161D", "\u161E", "\u161F",
 "\u1620", "\u1621", "\u1622", "\u1623", "\u1624", "\u1625", "\u1626", "\u1627", "\u1628", "\u1629", "\u162A", "\u162B", "\u162C", "\u162D", "\u162E", "\u162F",
 "\u1630", "\u1631", "\u1632", "\u1633", "\u1634", "\u1635", "\u1636", "\u1637", "\u1638", "\u1639", "\u163A", "\u163B", "\u163C", "\u163D", "\u163E", "\u163F",
 "\u1640", "\u1641", "\u1642", "\u1643", "\u1644", "\u1645", "\u1646", "\u1647", "\u1648", "\u1649", "\u164A", "\u164B", "\u164C", "\u164D", "\u164E", "\u164F",
 "\u1650", "\u1651", "\u1652", "\u1653", "\u1654", "\u1655", "\u1656", "\u1657", "\u1658", "\u1659", "\u165A", "\u165B", "\u165C", "\u165D", "\u165E", "\u165F",
 "\u1660", "\u1661", "\u1662", "\u1663", "\u1664", "\u1665", "\u1666", "\u1667", "\u1668", "\u1669", "\u166A", "\u166B", "\u166C", "\u166D", "\u166E", "\u166F",
 "\u1670", "\u1671", "\u1672", "\u1673", "\u1674", "\u1675", "\u1676", "\u1677", "\u1678", "\u1679", "\u167A", "\u167B", "\u167C", "\u167D", "\u167E", "\u167F",
};
std::string s_hebrew_font_sample[88] = {
 "\u0591", "\u0592", "\u0593", "\u0594", "\u0595", "\u0596", "\u0597", "\u0598", "\u0599", "\u059A", "\u059B", "\u059C", "\u059D", "\u059E", "\u059F",
 "\u05A0", "\u05A1", "\u05A2", "\u05A3", "\u05A4", "\u05A5", "\u05A6", "\u05A7", "\u05A8", "\u05A9", "\u05AA", "\u05AB", "\u05AC", "\u05AD", "\u05AE", "\u05AF",
 "\u05B0", "\u05B1", "\u05B2", "\u05B3", "\u05B4", "\u05B5", "\u05B6", "\u05B7", "\u05B8", "\u05B9", "\u05BA", "\u05BB", "\u05BC", "\u05BD", "\u05BE", "\u05BF",
 "\u05C0", "\u05C1", "\u05C2", "\u05C3", "\u05C4", "\u05C5", "\u05C6", "\u05C7",
 "\u05D0", "\u05D1", "\u05D2", "\u05D3", "\u05D4", "\u05D5", "\u05D6", "\u05D7", "\u05D8", "\u05D9", "\u05DA", "\u05DB", "\u05DC", "\u05DD", "\u05DE", "\u05DF",
 "\u05E0", "\u05E1", "\u05E2", "\u05E3", "\u05E4", "\u05E5", "\u05E6", "\u05E7", "\u05E8", "\u05E9", "\u05EA", "\u05EF",
 "\u05F0", "\u05F1", "\u05F2", "\u05F3", "\u05F4",
};
std::string s_lao_font_sample[67] = {
 "\u0E81", "\u0E82", "\u0E84", "\u0E87", "\u0E88", "\u0E8A", "\u0E8D",
 "\u0E94", "\u0E95", "\u0E96", "\u0E97", "\u0E99", "\u0E9A", "\u0E9B", "\u0E9C", "\u0E9D", "\u0E9E", "\u0E9F",
 "\u0EA1", "\u0EA2", "\u0EA3", "\u0EA5", "\u0EA7", "\u0EAA", "\u0EAB", "\u0EAD", "\u0EAE", "\u0EAF",
 "\u0EB0", "\u0EB1", "\u0EB2", "\u0EB3", "\u0EB4", "\u0EB5", "\u0EB6", "\u0EB7", "\u0EB8", "\u0EB9", "\u0EBB", "\u0EBC", "\u0EBD",
 "\u0EC0", "\u0EC1", "\u0EC2", "\u0EC3", "\u0EC4", "\u0EC6", "\u0EC8", "\u0EC9", "\u0ECA", "\u0ECB", "\u0ECC", "\u0ECD",
 "\u0ED0", "\u0ED1", "\u0ED2", "\u0ED3", "\u0ED4", "\u0ED5", "\u0ED6", "\u0ED7", "\u0ED8", "\u0ED9", "\u0EDC", "\u0EDD", "\u0EDE", "\u0EDF",
};
std::string s_kannada_font_sample[89] = {
 "\u0C81", "\u0C82", "\u0C83", "\u0C85", "\u0C86", "\u0C87", "\u0C88", "\u0C89", "\u0C8A", "\u0C8B", "\u0C8C", "\u0C8E", "\u0C8F",
 "\u0C90", "\u0C92", "\u0C93", "\u0C94", "\u0C95", "\u0C96", "\u0C97", "\u0C98", "\u0C99", "\u0C9A", "\u0C9B", "\u0C9C", "\u0C9D", "\u0C9E", "\u0C9F",
 "\u0CA0", "\u0CA1", "\u0CA2", "\u0CA3", "\u0CA4", "\u0CA5", "\u0CA6", "\u0CA7", "\u0CA8", "\u0CAA", "\u0CAB", "\u0CAC", "\u0CAD", "\u0CAE", "\u0CAF",
 "\u0CB0", "\u0CB1", "\u0CB2", "\u0CB3", "\u0CB5", "\u0CB6", "\u0CB7", "\u0CB8", "\u0CB9", "\u0CBC", "\u0CBD", "\u0CBE", "\u0CBF",
 "\u0CC0", "\u0CC1", "\u0CC2", "\u0CC3", "\u0CC4", "\u0CC6", "\u0CC7", "\u0CC8", "\u0CCA", "\u0CCB", "\u0CCC", "\u0CCD",
 "\u0CD5", "\u0CD6", "\u0CDE",
 "\u0CE0", "\u0CE1", "\u0CE2", "\u0CE3", "\u0CE6", "\u0CE7", "\u0CE8", "\u0CE9", "\u0CEA", "\u0CEB", "\u0CEC", "\u0CED", "\u0CEE", "\u0CEF",
 "\u0CF1", "\u0CF2",
};
std::string s_combining_diacritical_marks_for_symbols_font_sample[33] = {
 "\u20D0", "\u20D1", "\u20D2", "\u20D3", "\u20D4", "\u20D5", "\u20D6", "\u20D7", "\u20D8", "\u20D9", "\u20DA", "\u20DB", "\u20DC", "\u20DD", "\u20DE", "\u20DF",
 "\u20E0", "\u20E1", "\u20E2", "\u20E3", "\u20E4", "\u20E5", "\u20E6", "\u20E7", "\u20E8", "\u20E9", "\u20EA", "\u20EB", "\u20EC", "\u20ED", "\u20EE", "\u20EF",
 "\u20F0",
};
std::string s_combining_diacritical_marks_supplement_font_sample[63] = {
 "\u1DC0", "\u1DC1", "\u1DC2", "\u1DC3", "\u1DC4", "\u1DC5", "\u1DC6", "\u1DC7", "\u1DC8", "\u1DC9", "\u1DCA", "\u1DCB", "\u1DCC", "\u1DCD", "\u1DCE", "\u1DCF",
 "\u1DD0", "\u1DD1", "\u1DD2", "\u1DD3", "\u1DD4", "\u1DD5", "\u1DD6", "\u1DD7", "\u1DD8", "\u1DD9", "\u1DDA", "\u1DDB", "\u1DDC", "\u1DDD", "\u1DDE", "\u1DDF",
 "\u1DE0", "\u1DE1", "\u1DE2", "\u1DE3", "\u1DE4", "\u1DE5", "\u1DE6", "\u1DE7", "\u1DE8", "\u1DE9", "\u1DEA", "\u1DEB", "\u1DEC", "\u1DED", "\u1DEE", "\u1DEF",
 "\u1DF0", "\u1DF1", "\u1DF2", "\u1DF3", "\u1DF4", "\u1DF5", "\u1DF6", "\u1DF7", "\u1DF8", "\u1DF9", "\u1DFB", "\u1DFC", "\u1DFD", "\u1DFE", "\u1DFF",
};
std::string s_sinhala_font_sample[90] = {
 "\u0D82", "\u0D83", "\u0D85", "\u0D86", "\u0D87", "\u0D88", "\u0D89", "\u0D8A", "\u0D8B", "\u0D8C", "\u0D8D", "\u0D8E", "\u0D8F",
 "\u0D90", "\u0D91", "\u0D92", "\u0D93", "\u0D94", "\u0D95", "\u0D96", "\u0D9A", "\u0D9B", "\u0D9C", "\u0D9D", "\u0D9E", "\u0D9F",
 "\u0DA0", "\u0DA1", "\u0DA2", "\u0DA3", "\u0DA4", "\u0DA5", "\u0DA6", "\u0DA7", "\u0DA8", "\u0DA9", "\u0DAA", "\u0DAB", "\u0DAC", "\u0DAD", "\u0DAE", "\u0DAF",
 "\u0DB0", "\u0DB1", "\u0DB3", "\u0DB4", "\u0DB5", "\u0DB6", "\u0DB7", "\u0DB8", "\u0DB9", "\u0DBA", "\u0DBB", "\u0DBD",
 "\u0DC0", "\u0DC1", "\u0DC2", "\u0DC3", "\u0DC4", "\u0DC5", "\u0DC6", "\u0DCA", "\u0DCF",
 "\u0DD0", "\u0DD1", "\u0DD2", "\u0DD3", "\u0DD4", "\u0DD6", "\u0DD8", "\u0DD9", "\u0DDA", "\u0DDB", "\u0DDC", "\u0DDD", "\u0DDE", "\u0DDF",
 "\u0DE6", "\u0DE7", "\u0DE8", "\u0DE9", "\u0DEA", "\u0DEB", "\u0DEC", "\u0DED", "\u0DEE", "\u0DEF",
 "\u0DF2", "\u0DF3", "\u0DF4",
};
std::string s_yi_syllables_font_sample[1165] = {
 "\uA000", "\uA001", "\uA002", "\uA003", "\uA004", "\uA005", "\uA006", "\uA007", "\uA008", "\uA009", "\uA00A", "\uA00B", "\uA00C", "\uA00D", "\uA00E", "\uA00F",
 "\uA010", "\uA011", "\uA012", "\uA013", "\uA014", "\uA015", "\uA016", "\uA017", "\uA018", "\uA019", "\uA01A", "\uA01B", "\uA01C", "\uA01D", "\uA01E", "\uA01F",
 "\uA020", "\uA021", "\uA022", "\uA023", "\uA024", "\uA025", "\uA026", "\uA027", "\uA028", "\uA029", "\uA02A", "\uA02B", "\uA02C", "\uA02D", "\uA02E", "\uA02F",
 "\uA030", "\uA031", "\uA032", "\uA033", "\uA034", "\uA035", "\uA036", "\uA037", "\uA038", "\uA039", "\uA03A", "\uA03B", "\uA03C", "\uA03D", "\uA03E", "\uA03F",
 "\uA040", "\uA041", "\uA042", "\uA043", "\uA044", "\uA045", "\uA046", "\uA047", "\uA048", "\uA049", "\uA04A", "\uA04B", "\uA04C", "\uA04D", "\uA04E", "\uA04F",
 "\uA050", "\uA051", "\uA052", "\uA053", "\uA054", "\uA055", "\uA056", "\uA057", "\uA058", "\uA059", "\uA05A", "\uA05B", "\uA05C", "\uA05D", "\uA05E", "\uA05F",
 "\uA060", "\uA061", "\uA062", "\uA063", "\uA064", "\uA065", "\uA066", "\uA067", "\uA068", "\uA069", "\uA06A", "\uA06B", "\uA06C", "\uA06D", "\uA06E", "\uA06F",
 "\uA070", "\uA071", "\uA072", "\uA073", "\uA074", "\uA075", "\uA076", "\uA077", "\uA078", "\uA079", "\uA07A", "\uA07B", "\uA07C", "\uA07D", "\uA07E", "\uA07F",
 "\uA080", "\uA081", "\uA082", "\uA083", "\uA084", "\uA085", "\uA086", "\uA087", "\uA088", "\uA089", "\uA08A", "\uA08B", "\uA08C", "\uA08D", "\uA08E", "\uA08F",
 "\uA090", "\uA091", "\uA092", "\uA093", "\uA094", "\uA095", "\uA096", "\uA097", "\uA098", "\uA099", "\uA09A", "\uA09B", "\uA09C", "\uA09D", "\uA09E", "\uA09F",
 "\uA0A0", "\uA0A1", "\uA0A2", "\uA0A3", "\uA0A4", "\uA0A5", "\uA0A6", "\uA0A7", "\uA0A8", "\uA0A9", "\uA0AA", "\uA0AB", "\uA0AC", "\uA0AD", "\uA0AE", "\uA0AF",
 "\uA0B0", "\uA0B1", "\uA0B2", "\uA0B3", "\uA0B4", "\uA0B5", "\uA0B6", "\uA0B7", "\uA0B8", "\uA0B9", "\uA0BA", "\uA0BB", "\uA0BC", "\uA0BD", "\uA0BE", "\uA0BF",
 "\uA0C0", "\uA0C1", "\uA0C2", "\uA0C3", "\uA0C4", "\uA0C5", "\uA0C6", "\uA0C7", "\uA0C8", "\uA0C9", "\uA0CA", "\uA0CB", "\uA0CC", "\uA0CD", "\uA0CE", "\uA0CF",
 "\uA0D0", "\uA0D1", "\uA0D2", "\uA0D3", "\uA0D4", "\uA0D5", "\uA0D6", "\uA0D7", "\uA0D8", "\uA0D9", "\uA0DA", "\uA0DB", "\uA0DC", "\uA0DD", "\uA0DE", "\uA0DF",
 "\uA0E0", "\uA0E1", "\uA0E2", "\uA0E3", "\uA0E4", "\uA0E5", "\uA0E6", "\uA0E7", "\uA0E8", "\uA0E9", "\uA0EA", "\uA0EB", "\uA0EC", "\uA0ED", "\uA0EE", "\uA0EF",
 "\uA0F0", "\uA0F1", "\uA0F2", "\uA0F3", "\uA0F4", "\uA0F5", "\uA0F6", "\uA0F7", "\uA0F8", "\uA0F9", "\uA0FA", "\uA0FB", "\uA0FC", "\uA0FD", "\uA0FE", "\uA0FF",
 "\uA100", "\uA101", "\uA102", "\uA103", "\uA104", "\uA105", "\uA106", "\uA107", "\uA108", "\uA109", "\uA10A", "\uA10B", "\uA10C", "\uA10D", "\uA10E", "\uA10F",
 "\uA110", "\uA111", "\uA112", "\uA113", "\uA114", "\uA115", "\uA116", "\uA117", "\uA118", "\uA119", "\uA11A", "\uA11B", "\uA11C", "\uA11D", "\uA11E", "\uA11F",
 "\uA120", "\uA121", "\uA122", "\uA123", "\uA124", "\uA125", "\uA126", "\uA127", "\uA128", "\uA129", "\uA12A", "\uA12B", "\uA12C", "\uA12D", "\uA12E", "\uA12F",
 "\uA130", "\uA131", "\uA132", "\uA133", "\uA134", "\uA135", "\uA136", "\uA137", "\uA138", "\uA139", "\uA13A", "\uA13B", "\uA13C", "\uA13D", "\uA13E", "\uA13F",
 "\uA140", "\uA141", "\uA142", "\uA143", "\uA144", "\uA145", "\uA146", "\uA147", "\uA148", "\uA149", "\uA14A", "\uA14B", "\uA14C", "\uA14D", "\uA14E", "\uA14F",
 "\uA150", "\uA151", "\uA152", "\uA153", "\uA154", "\uA155", "\uA156", "\uA157", "\uA158", "\uA159", "\uA15A", "\uA15B", "\uA15C", "\uA15D", "\uA15E", "\uA15F",
 "\uA160", "\uA161", "\uA162", "\uA163", "\uA164", "\uA165", "\uA166", "\uA167", "\uA168", "\uA169", "\uA16A", "\uA16B", "\uA16C", "\uA16D", "\uA16E", "\uA16F",
 "\uA170", "\uA171", "\uA172", "\uA173", "\uA174", "\uA175", "\uA176", "\uA177", "\uA178", "\uA179", "\uA17A", "\uA17B", "\uA17C", "\uA17D", "\uA17E", "\uA17F",
 "\uA180", "\uA181", "\uA182", "\uA183", "\uA184", "\uA185", "\uA186", "\uA187", "\uA188", "\uA189", "\uA18A", "\uA18B", "\uA18C", "\uA18D", "\uA18E", "\uA18F",
 "\uA190", "\uA191", "\uA192", "\uA193", "\uA194", "\uA195", "\uA196", "\uA197", "\uA198", "\uA199", "\uA19A", "\uA19B", "\uA19C", "\uA19D", "\uA19E", "\uA19F",
 "\uA1A0", "\uA1A1", "\uA1A2", "\uA1A3", "\uA1A4", "\uA1A5", "\uA1A6", "\uA1A7", "\uA1A8", "\uA1A9", "\uA1AA", "\uA1AB", "\uA1AC", "\uA1AD", "\uA1AE", "\uA1AF",
 "\uA1B0", "\uA1B1", "\uA1B2", "\uA1B3", "\uA1B4", "\uA1B5", "\uA1B6", "\uA1B7", "\uA1B8", "\uA1B9", "\uA1BA", "\uA1BB", "\uA1BC", "\uA1BD", "\uA1BE", "\uA1BF",
 "\uA1C0", "\uA1C1", "\uA1C2", "\uA1C3", "\uA1C4", "\uA1C5", "\uA1C6", "\uA1C7", "\uA1C8", "\uA1C9", "\uA1CA", "\uA1CB", "\uA1CC", "\uA1CD", "\uA1CE", "\uA1CF",
 "\uA1D0", "\uA1D1", "\uA1D2", "\uA1D3", "\uA1D4", "\uA1D5", "\uA1D6", "\uA1D7", "\uA1D8", "\uA1D9", "\uA1DA", "\uA1DB", "\uA1DC", "\uA1DD", "\uA1DE", "\uA1DF",
 "\uA1E0", "\uA1E1", "\uA1E2", "\uA1E3", "\uA1E4", "\uA1E5", "\uA1E6", "\uA1E7", "\uA1E8", "\uA1E9", "\uA1EA", "\uA1EB", "\uA1EC", "\uA1ED", "\uA1EE", "\uA1EF",
 "\uA1F0", "\uA1F1", "\uA1F2", "\uA1F3", "\uA1F4", "\uA1F5", "\uA1F6", "\uA1F7", "\uA1F8", "\uA1F9", "\uA1FA", "\uA1FB", "\uA1FC", "\uA1FD", "\uA1FE", "\uA1FF",
 "\uA200", "\uA201", "\uA202", "\uA203", "\uA204", "\uA205", "\uA206", "\uA207", "\uA208", "\uA209", "\uA20A", "\uA20B", "\uA20C", "\uA20D", "\uA20E", "\uA20F",
 "\uA210", "\uA211", "\uA212", "\uA213", "\uA214", "\uA215", "\uA216", "\uA217", "\uA218", "\uA219", "\uA21A", "\uA21B", "\uA21C", "\uA21D", "\uA21E", "\uA21F",
 "\uA220", "\uA221", "\uA222", "\uA223", "\uA224", "\uA225", "\uA226", "\uA227", "\uA228", "\uA229", "\uA22A", "\uA22B", "\uA22C", "\uA22D", "\uA22E", "\uA22F",
 "\uA230", "\uA231", "\uA232", "\uA233", "\uA234", "\uA235", "\uA236", "\uA237", "\uA238", "\uA239", "\uA23A", "\uA23B", "\uA23C", "\uA23D", "\uA23E", "\uA23F",
 "\uA240", "\uA241", "\uA242", "\uA243", "\uA244", "\uA245", "\uA246", "\uA247", "\uA248", "\uA249", "\uA24A", "\uA24B", "\uA24C", "\uA24D", "\uA24E", "\uA24F",
 "\uA250", "\uA251", "\uA252", "\uA253", "\uA254", "\uA255", "\uA256", "\uA257", "\uA258", "\uA259", "\uA25A", "\uA25B", "\uA25C", "\uA25D", "\uA25E", "\uA25F",
 "\uA260", "\uA261", "\uA262", "\uA263", "\uA264", "\uA265", "\uA266", "\uA267", "\uA268", "\uA269", "\uA26A", "\uA26B", "\uA26C", "\uA26D", "\uA26E", "\uA26F",
 "\uA270", "\uA271", "\uA272", "\uA273", "\uA274", "\uA275", "\uA276", "\uA277", "\uA278", "\uA279", "\uA27A", "\uA27B", "\uA27C", "\uA27D", "\uA27E", "\uA27F",
 "\uA280", "\uA281", "\uA282", "\uA283", "\uA284", "\uA285", "\uA286", "\uA287", "\uA288", "\uA289", "\uA28A", "\uA28B", "\uA28C", "\uA28D", "\uA28E", "\uA28F",
 "\uA290", "\uA291", "\uA292", "\uA293", "\uA294", "\uA295", "\uA296", "\uA297", "\uA298", "\uA299", "\uA29A", "\uA29B", "\uA29C", "\uA29D", "\uA29E", "\uA29F",
 "\uA2A0", "\uA2A1", "\uA2A2", "\uA2A3", "\uA2A4", "\uA2A5", "\uA2A6", "\uA2A7", "\uA2A8", "\uA2A9", "\uA2AA", "\uA2AB", "\uA2AC", "\uA2AD", "\uA2AE", "\uA2AF",
 "\uA2B0", "\uA2B1", "\uA2B2", "\uA2B3", "\uA2B4", "\uA2B5", "\uA2B6", "\uA2B7", "\uA2B8", "\uA2B9", "\uA2BA", "\uA2BB", "\uA2BC", "\uA2BD", "\uA2BE", "\uA2BF",
 "\uA2C0", "\uA2C1", "\uA2C2", "\uA2C3", "\uA2C4", "\uA2C5", "\uA2C6", "\uA2C7", "\uA2C8", "\uA2C9", "\uA2CA", "\uA2CB", "\uA2CC", "\uA2CD", "\uA2CE", "\uA2CF",
 "\uA2D0", "\uA2D1", "\uA2D2", "\uA2D3", "\uA2D4", "\uA2D5", "\uA2D6", "\uA2D7", "\uA2D8", "\uA2D9", "\uA2DA", "\uA2DB", "\uA2DC", "\uA2DD", "\uA2DE", "\uA2DF",
 "\uA2E0", "\uA2E1", "\uA2E2", "\uA2E3", "\uA2E4", "\uA2E5", "\uA2E6", "\uA2E7", "\uA2E8", "\uA2E9", "\uA2EA", "\uA2EB", "\uA2EC", "\uA2ED", "\uA2EE", "\uA2EF",
 "\uA2F0", "\uA2F1", "\uA2F2", "\uA2F3", "\uA2F4", "\uA2F5", "\uA2F6", "\uA2F7", "\uA2F8", "\uA2F9", "\uA2FA", "\uA2FB", "\uA2FC", "\uA2FD", "\uA2FE", "\uA2FF",
 "\uA300", "\uA301", "\uA302", "\uA303", "\uA304", "\uA305", "\uA306", "\uA307", "\uA308", "\uA309", "\uA30A", "\uA30B", "\uA30C", "\uA30D", "\uA30E", "\uA30F",
 "\uA310", "\uA311", "\uA312", "\uA313", "\uA314", "\uA315", "\uA316", "\uA317", "\uA318", "\uA319", "\uA31A", "\uA31B", "\uA31C", "\uA31D", "\uA31E", "\uA31F",
 "\uA320", "\uA321", "\uA322", "\uA323", "\uA324", "\uA325", "\uA326", "\uA327", "\uA328", "\uA329", "\uA32A", "\uA32B", "\uA32C", "\uA32D", "\uA32E", "\uA32F",
 "\uA330", "\uA331", "\uA332", "\uA333", "\uA334", "\uA335", "\uA336", "\uA337", "\uA338", "\uA339", "\uA33A", "\uA33B", "\uA33C", "\uA33D", "\uA33E", "\uA33F",
 "\uA340", "\uA341", "\uA342", "\uA343", "\uA344", "\uA345", "\uA346", "\uA347", "\uA348", "\uA349", "\uA34A", "\uA34B", "\uA34C", "\uA34D", "\uA34E", "\uA34F",
 "\uA350", "\uA351", "\uA352", "\uA353", "\uA354", "\uA355", "\uA356", "\uA357", "\uA358", "\uA359", "\uA35A", "\uA35B", "\uA35C", "\uA35D", "\uA35E", "\uA35F",
 "\uA360", "\uA361", "\uA362", "\uA363", "\uA364", "\uA365", "\uA366", "\uA367", "\uA368", "\uA369", "\uA36A", "\uA36B", "\uA36C", "\uA36D", "\uA36E", "\uA36F",
 "\uA370", "\uA371", "\uA372", "\uA373", "\uA374", "\uA375", "\uA376", "\uA377", "\uA378", "\uA379", "\uA37A", "\uA37B", "\uA37C", "\uA37D", "\uA37E", "\uA37F",
 "\uA380", "\uA381", "\uA382", "\uA383", "\uA384", "\uA385", "\uA386", "\uA387", "\uA388", "\uA389", "\uA38A", "\uA38B", "\uA38C", "\uA38D", "\uA38E", "\uA38F",
 "\uA390", "\uA391", "\uA392", "\uA393", "\uA394", "\uA395", "\uA396", "\uA397", "\uA398", "\uA399", "\uA39A", "\uA39B", "\uA39C", "\uA39D", "\uA39E", "\uA39F",
 "\uA3A0", "\uA3A1", "\uA3A2", "\uA3A3", "\uA3A4", "\uA3A5", "\uA3A6", "\uA3A7", "\uA3A8", "\uA3A9", "\uA3AA", "\uA3AB", "\uA3AC", "\uA3AD", "\uA3AE", "\uA3AF",
 "\uA3B0", "\uA3B1", "\uA3B2", "\uA3B3", "\uA3B4", "\uA3B5", "\uA3B6", "\uA3B7", "\uA3B8", "\uA3B9", "\uA3BA", "\uA3BB", "\uA3BC", "\uA3BD", "\uA3BE", "\uA3BF",
 "\uA3C0", "\uA3C1", "\uA3C2", "\uA3C3", "\uA3C4", "\uA3C5", "\uA3C6", "\uA3C7", "\uA3C8", "\uA3C9", "\uA3CA", "\uA3CB", "\uA3CC", "\uA3CD", "\uA3CE", "\uA3CF",
 "\uA3D0", "\uA3D1", "\uA3D2", "\uA3D3", "\uA3D4", "\uA3D5", "\uA3D6", "\uA3D7", "\uA3D8", "\uA3D9", "\uA3DA", "\uA3DB", "\uA3DC", "\uA3DD", "\uA3DE", "\uA3DF",
 "\uA3E0", "\uA3E1", "\uA3E2", "\uA3E3", "\uA3E4", "\uA3E5", "\uA3E6", "\uA3E7", "\uA3E8", "\uA3E9", "\uA3EA", "\uA3EB", "\uA3EC", "\uA3ED", "\uA3EE", "\uA3EF",
 "\uA3F0", "\uA3F1", "\uA3F2", "\uA3F3", "\uA3F4", "\uA3F5", "\uA3F6", "\uA3F7", "\uA3F8", "\uA3F9", "\uA3FA", "\uA3FB", "\uA3FC", "\uA3FD", "\uA3FE", "\uA3FF",
 "\uA400", "\uA401", "\uA402", "\uA403", "\uA404", "\uA405", "\uA406", "\uA407", "\uA408", "\uA409", "\uA40A", "\uA40B", "\uA40C", "\uA40D", "\uA40E", "\uA40F",
 "\uA410", "\uA411", "\uA412", "\uA413", "\uA414", "\uA415", "\uA416", "\uA417", "\uA418", "\uA419", "\uA41A", "\uA41B", "\uA41C", "\uA41D", "\uA41E", "\uA41F",
 "\uA420", "\uA421", "\uA422", "\uA423", "\uA424", "\uA425", "\uA426", "\uA427", "\uA428", "\uA429", "\uA42A", "\uA42B", "\uA42C", "\uA42D", "\uA42E", "\uA42F",
 "\uA430", "\uA431", "\uA432", "\uA433", "\uA434", "\uA435", "\uA436", "\uA437", "\uA438", "\uA439", "\uA43A", "\uA43B", "\uA43C", "\uA43D", "\uA43E", "\uA43F",
 "\uA440", "\uA441", "\uA442", "\uA443", "\uA444", "\uA445", "\uA446", "\uA447", "\uA448", "\uA449", "\uA44A", "\uA44B", "\uA44C", "\uA44D", "\uA44E", "\uA44F",
 "\uA450", "\uA451", "\uA452", "\uA453", "\uA454", "\uA455", "\uA456", "\uA457", "\uA458", "\uA459", "\uA45A", "\uA45B", "\uA45C", "\uA45D", "\uA45E", "\uA45F",
 "\uA460", "\uA461", "\uA462", "\uA463", "\uA464", "\uA465", "\uA466", "\uA467", "\uA468", "\uA469", "\uA46A", "\uA46B", "\uA46C", "\uA46D", "\uA46E", "\uA46F",
 "\uA470", "\uA471", "\uA472", "\uA473", "\uA474", "\uA475", "\uA476", "\uA477", "\uA478", "\uA479", "\uA47A", "\uA47B", "\uA47C", "\uA47D", "\uA47E", "\uA47F",
 "\uA480", "\uA481", "\uA482", "\uA483", "\uA484", "\uA485", "\uA486", "\uA487", "\uA488", "\uA489", "\uA48A", "\uA48B", "\uA48C",
};
std::string s_font_samples[62] = {
 "\u0080","\u0400","\u0800","\u0C00","\u1000","\u1400","\u1800","\u1C00","\u2000","\u2400","\u2800","\u2C00","\u3000","\u3400","\u3800","\u3C00",
 "\u4000","\u4400","\u4800","\u4C00","\u5000","\u5400","\u5800","\u5C00","\u6000","\u6400","\u6800","\u6C00","\u7000","\u7400","\u7800","\u7C00",
 "\u8000","\u8400","\u8800","\u8C00","\u9000","\u9400","\u9800","\u9C00","\uA000","\uA400","\uA800","\uAC00","\uB000","\uB400","\uB800","\uBC00",
 "\uC000","\uC400","\uC800","\uCC00","\uD000","\uD400",/*uD800    uDC00*/"\uE000","\uE400","\uE800","\uEC00","\uF000","\uF400","\uF800","\uFC00",
};
std::string s_font_file_name[46] = {
 "basic_latin", 
 "latin_1_supplement", 
 "ipa_extensions",
 "spacing_modifier_letters",
 "combining_diacritical_marks",
 "greek_and_coptic",
 "cyrillic",
 "cyrillic_supplement",
 "armenian",
 "hebrew",
 "arabic",
 "devanagari",
 "gurmukhi",
 "tamil",
 "telugu",
 "kannada",
 "sinhala",
 "thai",
 "lao",
 "tibetan",
 "georgian",
 "unified_canadian_aboriginal_syllabics",
 "phonetic_extensions",
 "combining_diacritical_marks_supplement",
 "greek_extended",
 "general_punctuation",
 "superscripts_and_subscripts",
 "combining_diacritical_marks_for_symbols",
 "arrows",
 "mathematical_operators",
 "miscellaneous_technical",
 "optical_character_recognition",
 "box_drawing",
 "block_elements",
 "geometric_shapes",
 "miscellaneous_symbols",
 "dingbats",
 "cjk_symbol_and_punctuation",
 "hiragana",
 "katakana",
 "hangul_compatibility_jamo",
 "yi_syllables",
 "yi_radicals",
 "cjk_compatibility_forms",
 "halfwidth_and_fullwidth_forms",
 "miscellaneous_symbols_and_pictographs",
};

/*
 0 Basic latin
 1 Latin 1 supplement
 2 Ipa extensions
 3 Spacing modifier letters
 4 Combining diacritical marks
 5 Greek and coptic
 6 Cyrillic
 7 Cyrillic supplement
 8 Armenian
 9 Hebrew
10 Arabic
11 Devanagari
12 Gurmukhi
13 Tamil
14 Telugu
15 Kannada
16 Sinhala
17 Thai
18 Lao
19 Tibetan
20 Georgian
21 Unified canadian aboriginal syllabics
22 Phonetic extensions
23 Combining diacritical marks supplement
24 Greek extended
25 General punctuation
26 Superscripts and subscripts
27 Combining diacritical marks for symbols
28 Arrows
29 Mathematical operators
30 Miscellaneous technical
31 Optical character recognition
32 Box drawing
33 Block elements
34 Geometric shapes
35 Miscellaneous symbols
36 Dingbats
37 Cjk symbol and punctuation
38 Hiragana
39 Katakana
40 Hangul compatibility jamo
41 Yi syllables
42 Yi radicals
43 Cjk compatibility forms
44 Halfwidth and fullwidth forms
45Miscellaneous symbols and pictographs
*/

C2D_Image Background_image[2]; 
C2D_Image Wifi_icon_image[9]; 
C2D_Image Battery_level_icon_image[21]; 
C2D_Image Battery_charge_icon_image[1]; 
C2D_Image Square_image[14]; 
C2D_Image sem_help_image[7];
C2D_Image stickers_images[121];
/*
   0 ~   94   (95) Basic latin
  95 ~  222  (128) Latin 1 supplement
 223 ~  314   (92) Ipa extensions
 315 ~  394   (80) Spacing modifier letters
 395 ~  506  (112) Combining diacritical marks
 507 ~  641  (135) Greek and coptic
 642 ~  897  (256) Cyrillic
 898 ~  945   (48) Cyrillic supplement
 946 ~ 1037   (92) Armenian
1038 ~ 1125   (88) Hebrew
1126 ~ 1390  (255) Arabic
1391 ~ 1508  (128) Devanagari
1509 ~ 1587   (79) Gurmukhi
1588 ~ 1659   (72) Tamil
1660 ~ 1755   (96) Telugu
1756 ~ 1844   (89) Kannada
1845 ~ 1934   (90) Sinhala
1935 ~ 2021   (87) Thai
2022 ~ 2088   (67) Lao
2089 ~ 2299  (211) Tibetan
2300 ~ 2387   (88) Georgian
2388 ~ 3027  (640) Unified canadian aboriginal syllabics
3028 ~ 3155  (128) Phonetic extensions
3156 ~ 3218   (63) Combining diacritical marks supplement
3219 ~ 3451  (233) Greek extended
3452 ~ 3562  (111) General punctuation
3563 ~ 3604   (42) Superscripts and subscripts
3605 ~ 3637   (33) Combining diacritical marks for symbols
3638 ~ 3749  (112) Arrows
3750 ~ 4005  (256) Mathematical operators
4006 ~ 4261  (256) Miscellaneous technical
4262 ~ 4272   (11) Optical character recognition
4273 ~ 4400  (128) Box drawing
4401 ~ 4432   (32) Block elements
4433 ~ 4528   (96) Geometric shapes
4529 ~ 4784  (256) Miscellaneous_symbols
4785 ~ 4976  (192) Dingbats
4977 ~ 5040   (64) Cjk symbol and punctuation
5041 ~ 5133   (93) Hiragana
5134 ~ 5229   (96) Katakana
5230 ~ 5323   (94) Hangul compatibility jamo
5324 ~ 6488 (1165) Yi syllables
6489 ~ 6543   (55) Yi radicals
6544 ~ 6575   (32) Cjk compatibility forms
6576 ~ 6800  (225) Halfwidth and fullwidth forms
6801 ~ 7568  (768) Miscellaneous symbols and pictographs
*/
C2D_Image font_images[7569];

C2D_ImageTint texture_tint, dammy_tint;

TickCounter s_tcount_up_time, s_tcount_frame_time;
Thread s_connect_test_thread, s_hid_thread, s_update_thread, s_send_app_info_thread;
SwkbdState s_swkb;
SwkbdLearningData s_swkb_learn_data;
SwkbdDictWord s_swkb_words[8];
SwkbdButton press_button, s_swkb_press_button;

void Share_send_app_info_thread(void* arg)
{
	Share_app_log_save("Share/Send app info thread", "Thread started.", 1234567890, false);
	httpcContext send_app_info_httpc;
	OS_VersionBin os_ver;
	bool is_new3ds = false;
	char system_ver_char[0x50] = " ";
	std::string new3ds;

	osGetSystemVersionDataString(&os_ver, &os_ver, system_ver_char, 0x50);
	std::string system_ver = system_ver_char;
	system_ver = system_ver.substr(0, (system_ver.length() - 1));

	if (s_apt_success)
		APT_CheckNew3DS(&is_new3ds);

	if (is_new3ds)
		new3ds = "yes";
	else
		new3ds = "no";

	std::string send_data = "{ \"app_ver\": \"" + s_app_ver + "\",\"system_ver\" : \"" + system_ver + "\",\"start_num_of_app\" : \"" + std::to_string(s_num_of_app_start) + "\",\"language\" : \"" + s_setting[1] + "\",\"new3ds\" : \"" + new3ds + "\",\"time_to_enter_sleep\" : \"" + std::to_string(s_time_to_enter_afk) + "\",\"scroll_speed\" : \"" + std::to_string(s_scroll_speed) + "\" }";

	httpcOpenContext(&send_app_info_httpc, HTTPC_METHOD_POST, "https://script.google.com/macros/s/AKfycbyn_blFyKWXCgJr6NIF8x6ETs7CHRN5FXKYEAAIrzV6jPYcCkI/exec", 0);
	httpcSetSSLOpt(&send_app_info_httpc, SSLCOPT_DisableVerify);
	httpcSetKeepAlive(&send_app_info_httpc, HTTPC_KEEPALIVE_ENABLED);
	httpcAddRequestHeaderField(&send_app_info_httpc, "Connection", "Keep-Alive");
	httpcAddRequestHeaderField(&send_app_info_httpc, "User-Agent", s_httpc_user_agent.c_str());
	httpcAddPostDataRaw(&send_app_info_httpc, (u32*)send_data.c_str(), strlen(send_data.c_str()));
	httpcBeginRequest(&send_app_info_httpc);
	httpcCloseContext(&send_app_info_httpc);

	Share_app_log_save("Share/Send app info thread", "Thread exit.", 1234567890, false);
}

void Share_connectivity_check_thread(void* arg)
{
	Share_app_log_save("Share/Connectivity check thread", "Thread started.", 1234567890, false);
	u32 connect_check_response_code = 0;
	int count = 100;
	std::string connect_check_url = "https://connectivitycheck.gstatic.com/generate_204";
	httpcContext connect_check_httpc;

	while (s_connect_test_thread_run)
	{
		if (count >= 100 && !s_hid_disabled)
		{
			count = 0;
			httpcOpenContext(&connect_check_httpc, HTTPC_METHOD_GET, connect_check_url.c_str(), 0);
			httpcSetSSLOpt(&connect_check_httpc, SSLCOPT_DisableVerify);
			httpcSetKeepAlive(&connect_check_httpc, HTTPC_KEEPALIVE_ENABLED);
			httpcAddRequestHeaderField(&connect_check_httpc, "Connection", "Keep-Alive");
			httpcAddRequestHeaderField(&connect_check_httpc, "User-Agent", s_httpc_user_agent.c_str());
			httpcAddRequestHeaderField(&connect_check_httpc, "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
			httpcAddRequestHeaderField(&connect_check_httpc, "Accept-Encoding", "en,en-US;q=1,ja;q=0.9");
			httpcAddRequestHeaderField(&connect_check_httpc, "Accept-Language", "gzip, deflate, br");
			httpcAddRequestHeaderField(&connect_check_httpc, "Upgrade-Insecure-Requests", "1");
			httpcBeginRequest(&connect_check_httpc);
			httpcGetResponseStatusCode(&connect_check_httpc, &connect_check_response_code);
			httpcCloseContext(&connect_check_httpc);

			if (connect_check_response_code == 204)
				s_connect_test_succes = true;
			else
				s_connect_test_succes = false;
		}
		usleep(100000);
		count++;
	}
	Share_app_log_save("Share/Connectivity check thread", "Thread exit.", 1234567890, false);
}

void Share_get_system_info(void)
{
	PTMU_GetBatteryChargeState(&s_battery_charge);//battery charge
	if (s_mcu_success)
	{
		MCUHWC_GetBatteryLevel(&s_battery_level);//battery level(%)
		s_battery_level_string = std::to_string(s_battery_level);
	}
	else
	{
		PTMU_GetBatteryLevel(&s_battery_level);
		
		if((int)s_battery_level == 0)
			s_battery_level = 0;
		else if((int)s_battery_level == 1)
			s_battery_level = 5;
		else if ((int)s_battery_level == 2)
			s_battery_level = 10;
		else if ((int)s_battery_level == 3)
			s_battery_level = 30;
		else if ((int)s_battery_level == 4)
			s_battery_level = 60;
		else if ((int)s_battery_level == 5)
			s_battery_level = 100;

		s_battery_level_string = "?";
	}

	s_wifi_signal = osGetWifiStrength();

	//Get wifi state from shared memory #0x1FF81067
	memcpy((void*)wifi_state, (void*)0x1FF81067, 0x1);
	if (memcmp(wifi_state, wifi_state_internet_sample, 0x1) == 0)
	{
		if (!s_connect_test_succes)
			s_wifi_signal = s_wifi_signal + 4;
	}
	else
	{
		s_wifi_signal = 8;
		s_connect_test_succes = false;
	}

	//Get time
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t*)&unixTime);
	s_months = timeStruct->tm_mon;
	s_months = s_months + 1;
	s_days = timeStruct->tm_mday;
	s_hours = timeStruct->tm_hour;
	s_minutes = timeStruct->tm_min;
	s_seconds = timeStruct->tm_sec;
}

int Share_check_free_ram(void)
{
	u8* malloc_check[200];
	int count;

	for (int i = 0; i < 200; i++)
		malloc_check[i] = NULL;

	for (count = 0; count < 200; count++)
	{
		malloc_check[count] = (u8*)malloc(0x100000);// 1MB
		if (malloc_check[count] == NULL)
			break;
	}

	for (int i = 0; i <= count; i++)
		free(malloc_check[i]);

	return count;
}

bool Share_exit_check(void)
{
	while (true)
	{
		Draw_set_draw_mode(1);
		if (s_night_mode)
		{
			Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
			Draw("Do you want to exit this software?", 90.0, 105.0f, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
		}
		else
		{
			Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);
			Draw("Do you want to exit this software?", 90.0, 105.0f, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
		}

		Draw("A to close", 130.0, 140.0f, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0);
		Draw("B to back", 210.0, 140.0f, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);

		Draw_apply_draw();
		if (s_key_A_press)
			return true;
		else if (s_key_B_press)
			return false;
	}
}

void Share_update_thread(void* arg)
{
	int update_thread_count = 0;
	Share_app_log_save("Share/Update thread", "Thread started.", 1234567890, false);
	while (s_update_thread_run)
	{
		usleep(50000);
		update_thread_count++;

		if ((update_thread_count == 10 || update_thread_count == 20) && s_debug_mode)
		{
			//check free RAM
			s_free_ram = Share_check_free_ram();
			s_free_linear_ram = linearSpaceFree() / 1024 / 1024;
		}

		if (update_thread_count >= 20)
		{
			//fps
			s_fps_show = s_fps;
			s_fps = 0;
			update_thread_count = 0;
		}

		if (s_flash_mode)
		{
			if (s_night_mode)
			{
				C2D_PlainImageTint(&texture_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);
				s_night_mode = false;
			}
			else
			{
				C2D_PlainImageTint(&texture_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.75), true);
				s_night_mode = true;
			}
		}
		//screen control
		s_afk_time++;
		if (s_disabled_enter_afk_mode)
			Change_brightness(true, true, s_lcd_brightness);
		else
		{
			if (s_afk_time > (s_time_to_enter_afk + 100) * 2)
				Change_screen_state(true, true, false);
			else if (s_afk_time >= s_time_to_enter_afk * 2)
			{
				Change_screen_state(true, true, true);
				Change_brightness(true, true, s_afk_lcd_brightness);
			}
			else if (s_afk_time <= (s_time_to_enter_afk - 1) * 2)
			{
				Change_screen_state(true, true, true);
				Change_brightness(true, true, s_lcd_brightness);
			}
		}
	}
	Share_app_log_save("Share/Update thread", "Thread exit.", 1234567890, false);
}

void Share_app_log_draw(void)
{
	Draw_set_draw_mode(1);
	if (s_night_mode)
		Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);

	s_app_log_view_num = s_app_log_view_num_cache;
	for (int i = 0; i < 23; i++)
		Draw(s_app_logs[s_app_log_view_num + i], s_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
	Draw_apply_draw();
}

int Share_app_log_save(std::string type, std::string text, Result result, bool draw)
{
	double time_cache;
	char app_log_cache[4096];
	osTickCounterUpdate(&s_tcount_up_time);
	time_cache = osTickCounterRead(&s_tcount_up_time);
	s_app_up_time_ms = s_app_up_time_ms + time_cache;
	s_log_time[s_app_log_num] = s_app_up_time_ms;
	time_cache = s_app_up_time_ms / 1000;
	memset(app_log_cache, 0x0, 4096);

	if (result == 1234567890)
		sprintf(app_log_cache, "[%.5f][%s] %s", time_cache, type.c_str(), text.c_str());
	else
		sprintf(app_log_cache, "[%.5f][%s] %s0x%lx", time_cache, type.c_str(), text.c_str(), result);

	s_app_logs[s_app_log_num] = app_log_cache;
	s_app_log_num++;
	if (s_app_log_num >= 4096)
		s_app_log_num = 0;

	if (s_app_log_num < 23)
		s_app_log_view_num = 0;
	else
		s_app_log_view_num_cache = s_app_log_num - 23;

	if (draw)
		Share_app_log_draw();
	/*
	for (int i = 0; i <= 2; i++)
	{
		function_result = Share_save_to_file("Log.txt", app_log_cache, "/Line/", false, Share_log_fs_handle, Share_log_fs_archive);
		if (function_result == 0)
		{
			break;
		}
	}*/

	return (s_app_log_num - 1);
}

void Share_app_log_add_result(int add_log_num, std::string add_text, Result result, bool draw)
{
	double time_cache;
	char app_log_add_cache[4096];
	osTickCounterUpdate(&s_tcount_up_time);
	time_cache = osTickCounterRead(&s_tcount_up_time);
	s_app_up_time_ms = s_app_up_time_ms + time_cache;
	time_cache = s_app_up_time_ms - s_log_time[add_log_num];
	memset(app_log_add_cache, 0x0, 4096);

	if (result != 1234567890)
		sprintf(app_log_add_cache, "%s0x%lx (%.2fms)", add_text.c_str(), result, time_cache);
	else
		sprintf(app_log_add_cache, "%s (%.2fms)", add_text.c_str(), time_cache);

	if (draw)
		Share_app_log_draw();

	s_app_logs[add_log_num] += app_log_add_cache;
}

void Share_key_flag_reset(void)
{
	s_key_A_press = false;
	s_key_B_press = false;
	s_key_X_press = false;
	s_key_Y_press = false;
	s_key_CPAD_UP_press = false;
	s_key_CPAD_DOWN_press = false;
	s_key_CPAD_RIGHT_press = false;
	s_key_CPAD_LEFT_press = false;
	s_key_DUP_press = false;
	s_key_DDOWN_press = false;
	s_key_DRIGHT_press = false;
	s_key_DLEFT_press = false;
	s_key_L_press = false;
	s_key_R_press = false;
	s_key_ZL_press = false;
	s_key_ZR_press = false;
	s_key_START_press = false;
	s_key_SELECT_press = false;
	s_key_touch_press = false;
	s_key_A_held = false;
	s_key_B_held = false;
	s_key_X_held = false;
	s_key_Y_held = false;
	s_key_CPAD_UP_held = false;
	s_key_CPAD_DOWN_held = false;
	s_key_CPAD_RIGHT_held = false;
	s_key_CPAD_LEFT_held = false;
	s_key_DUP_held = false;
	s_key_DDOWN_held = false;
	s_key_DRIGHT_held = false;
	s_key_DLEFT_held = false;
	s_key_L_held = false;
	s_key_R_held = false;
	s_key_ZL_held = false;
	s_key_ZR_held = false;
	s_key_touch_held = false;
	s_touch_pos_x = 0;
	s_touch_pos_y = 0;
}

void Share_draw_init_progress(std::string message)
{
	Draw_set_draw_mode(s_draw_vsync_mode);
	if (s_night_mode)
		Draw_screen_ready_to_draw(0, true, 1, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 1, 1.0, 1.0, 1.0);

	Draw(message, 100.0, 120.0, 0.75, 0.75, 0.0, 0.5, 1.0, 1.0);

	Draw_apply_draw();
}

std::string Share_dec_to_hex_string(long dec)
{
	char hex_string[128];
	sprintf(hex_string, "0x%lx", dec);
	return hex_string;
}

void Share_clear_error_message(void)
{
	s_error_summary = "N/A";
	s_error_description = "N/A";
	s_error_place = "N/A";
	s_error_code = Share_dec_to_hex_string(0);
}

void Share_set_error_message(std::string summary, std::string description, std::string place, long error_code)
{
	s_error_summary = summary;
	s_error_description = description;
	s_error_place = place;
	if(error_code == 1234567890)
		s_error_code = "N/A";
	else
		s_error_code = Share_dec_to_hex_string(error_code);
}

void Share_draw_error(void)
{
	Draw_texture(Square_image, dammy_tint, 10, 20.0, 30.0, 280.0, 150.0);
	Draw_texture(Square_image, dammy_tint, 5, 150.0, 150.0, 20.0, 20.0);

	Draw("Summary : ", 22.5, 40.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(s_error_summary, 22.5, 50.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	Draw("Description : ", 22.5, 60.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(s_error_description, 22.5, 70.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
	Draw("Place : ", 22.5, 90.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(s_error_place, 22.5, 100.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	Draw("Error code : ", 22.5, 110.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
	Draw(s_error_code, 22.5, 120.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
	Draw("OK", 152.5, 152.5, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
}

void Share_scan_hid_thread(void* arg)
{
	Share_app_log_save("Share/Scan hid thread", "Thread started.", 1234567890, false);

	int scan_hid_log_num_return;
	int scan_hid_load_font_num;
	bool scroll_bar_selected = false;
	bool bar_selected[4] = { false, false, false, false, };
	bool scroll_mode = false;
	u32 kDown;
	u32 kHeld;
	touchPosition touch_pos;
	circlePosition circle_pos;
	Result_with_string scan_hid_result;

	while (s_hid_thread_run)
	{
		if (s_hid_disabled)
			Share_key_flag_reset();

		hidScanInput();
		hidTouchRead(&touch_pos);
		hidCircleRead(&circle_pos);
		kHeld = hidKeysHeld();
		kDown = hidKeysDown();


		if (kDown & KEY_A)
			s_key_A_press = true;
		if (kDown & KEY_B)
			s_key_B_press = true;
		if (kDown & KEY_X)
			s_key_X_press = true;
		if (kDown & KEY_Y)
			s_key_Y_press = true;
		if (kDown & KEY_DUP)
			s_key_DUP_press = true;
		if (kDown & KEY_DDOWN)
			s_key_DDOWN_press = true;
		if (kDown & KEY_DRIGHT)
			s_key_DRIGHT_press = true;
		if (kDown & KEY_DLEFT)
			s_key_DLEFT_press = true;
		if (kDown & KEY_CPAD_UP)
			s_key_CPAD_UP_press = true;
		if (kDown & KEY_CPAD_DOWN)
			s_key_CPAD_DOWN_press = true;
		if (kDown & KEY_CPAD_RIGHT)
			s_key_CPAD_RIGHT_press = true;
		if (kDown & KEY_CPAD_LEFT)
			s_key_CPAD_LEFT_press = true;
		if (kDown & KEY_SELECT)
			s_key_SELECT_press = true;
		if (kDown & KEY_START)
			s_key_START_press = true;
		if (kDown & KEY_L)
			s_key_L_press = true;
		if (kDown & KEY_R)
			s_key_R_press = true;
		if (kDown & KEY_ZL)
			s_key_ZL_press = true;
		if (kDown & KEY_ZR)
			s_key_ZR_press = true;
		if (kHeld & KEY_DRIGHT)
			s_key_DRIGHT_held = true;
		if (kHeld & KEY_DLEFT)
			s_key_DLEFT_held = true;
		if (kHeld & KEY_DDOWN)
			s_key_DDOWN_held = true;
		if (kHeld & KEY_DUP)
			s_key_DUP_held = true;
		if (kHeld & KEY_A)
			s_key_A_held = true;
		if (kHeld & KEY_B)
			s_key_B_held = true;
		if (kHeld & KEY_Y)
			s_key_Y_held = true;
		if (kHeld & KEY_X)
			s_key_X_held = true;
		if (kHeld & KEY_CPAD_UP)
			s_key_CPAD_UP_held = true;
		if (kHeld & KEY_CPAD_DOWN)
			s_key_CPAD_DOWN_held = true;
		if (kHeld & KEY_CPAD_RIGHT)
			s_key_CPAD_RIGHT_held = true;
		if (kHeld & KEY_CPAD_LEFT)
			s_key_CPAD_LEFT_held = true;
		if (kHeld & KEY_L)
			s_key_L_held = true;
		if (kHeld & KEY_R)
			s_key_R_held = true;
		if (kDown & KEY_ZL)
			s_key_ZL_held = true;
		if (kDown & KEY_ZR)
			s_key_ZR_held = true;
		if (kDown & KEY_TOUCH || kHeld & KEY_TOUCH)
		{
			if (kDown & KEY_TOUCH)
			{
				s_key_touch_press = true;
				s_touch_pos_x_before = touch_pos.px;
				s_touch_pos_y_before = touch_pos.py;
				s_touch_pos_x = touch_pos.px;
				s_touch_pos_y = touch_pos.py;
			}
			if (kHeld & KEY_TOUCH)
			{
				s_key_touch_held = true;
				s_touch_pos_x = touch_pos.px;
				s_touch_pos_y = touch_pos.py;
				s_touch_pos_x_moved = s_touch_pos_x_before - s_touch_pos_x;
				s_touch_pos_y_moved = s_touch_pos_y_before - s_touch_pos_y;
				s_touch_pos_x_before = touch_pos.px;
				s_touch_pos_y_before = touch_pos.py;
			}
		}
		else
		{
			s_touch_pos_x_moved = 0;
			s_touch_pos_y_moved = 0;
			s_touch_pos_x_before = 0;
			s_touch_pos_y_before = 0;
		}

		if (s_key_X_held && s_key_Y_press)
		{
			if (s_system_setting_menu_show)
				s_system_setting_menu_show = false;
			else
				s_system_setting_menu_show = true;
		}

		if (s_key_CPAD_UP_held && !s_hid_disabled)
		{
			if (s_app_logs_show)
			{
				s_app_log_view_num_cache--;
				if (s_app_log_view_num_cache < 0)
					s_app_log_view_num_cache = 0;
			}
		}
		if (s_key_CPAD_DOWN_held && !s_hid_disabled)
		{
			if (s_app_logs_show)
			{
				s_app_log_view_num_cache++;
				if (s_app_log_view_num_cache > 512)
					s_app_log_view_num_cache = 512;
			}
		}
		if (s_key_CPAD_LEFT_held && !s_hid_disabled)
		{
			if (s_app_logs_show)
			{
				app_log_x_cache += 5.0f;
				if (app_log_x_cache > 0.0)
					app_log_x_cache = 0.0f;
			}
		}
		if (s_key_CPAD_RIGHT_held && !s_hid_disabled)
		{
			if (s_app_logs_show)
			{
				app_log_x_cache -= 10.0f;
				if (app_log_x_cache < -1000.0)
					app_log_x_cache = -1000.0f;
			}
		}

		if (s_system_setting_menu_show)
		{
			if (s_key_touch_held)
			{
				if (s_touch_pos_x > 0 && s_touch_pos_x < 300 && s_touch_pos_y > 0 && s_touch_pos_y < 30)
				{
					s_lcd_brightness = (s_touch_pos_x + 20) / 2;
					Change_brightness(true, true, s_lcd_brightness);
				}
			}
			if (s_key_touch_press)
			{
				if (s_touch_pos_x > 300 && s_touch_pos_x < 320 && s_touch_pos_y > 0 && s_touch_pos_y < 30)
				{
					if (s_wifi_enabled)
					{
						scan_hid_result.code = Wifi_disable();
						Share_app_log_save("Share/Scan hid thread/nwm", "Wifi_disable...", scan_hid_result.code, false);
						if (scan_hid_result.code == 0)
							s_wifi_enabled = false;
					}
					else
					{
						scan_hid_result.code = Wifi_enable();
						Share_app_log_save("Share/Scan hid thread/nwm", "Wifi_enable...", scan_hid_result.code, false);
						if (scan_hid_result.code == 0)
							s_wifi_enabled = true;
					}
				}
				else if (s_touch_pos_x > 300 && s_touch_pos_x < 320 && s_touch_pos_y > 30 && s_touch_pos_y < 60)
				{
					if (s_disabled_enter_afk_mode)
						s_disabled_enter_afk_mode = false;
					else
						s_disabled_enter_afk_mode = true;
				}
			}
		}

		if (s_error_display)
		{
			if (s_key_touch_press && s_key_touch_press && s_touch_pos_x >= 150 && s_touch_pos_x <= 170 && s_touch_pos_y >= 150 && s_touch_pos_y < 170)
				s_error_display = false;
		}
		else if (s_sem_main_run && !s_hid_disabled)
		{
			if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
			{
				s_sem_main_run = false;
				s_menu_main_run = true;
			}
			if (s_sem_show_newest_ver_data)
			{
				if (s_key_B_press || (s_key_touch_press && s_touch_pos_x >= 160 && s_touch_pos_x <= 304 && s_touch_pos_y >= 200 && s_touch_pos_y < 215))
					s_sem_show_newest_ver_data = false;
				if (s_key_A_press || (s_key_touch_press && s_touch_pos_x >= 15 && s_touch_pos_x <= 159 && s_touch_pos_y >= 200 && s_touch_pos_y < 215))
				{
					s_sem_show_newest_ver_data = false;
					s_sem_select_ver = true;
				}
			}
			else if (s_sem_select_ver)
			{
				if (!s_sem_file_download_request)
				{
					if (s_key_B_press || (s_key_touch_press && s_touch_pos_x >= 15 && s_touch_pos_x <= 159 && s_touch_pos_y >= 200 && s_touch_pos_y < 215))
					{
						s_sem_show_newest_ver_data = true;
						s_sem_select_ver = false;
					}
					else if ((s_key_X_press || (s_key_touch_press && s_touch_pos_x >= 160 && s_touch_pos_x <= 304 && s_touch_pos_y >= 200 && s_touch_pos_y < 215)) && s_sem_available_ver[s_sem_selected_edition_num])
						s_sem_file_download_request = true;

					for (int i = 0; i < 8; i++)
					{
						if (s_key_touch_press && s_touch_pos_x >= 17 && s_touch_pos_x <= 250 && s_touch_pos_y >= 15 + (i * 10) && s_touch_pos_y <= 24 + (i * 10))
						{
							s_sem_selected_edition_num = i;
							break;
						}
					}
				}
			}
			else
			{
				if (s_key_touch_press || s_key_touch_held)
				{
					s_touch_pos_x_move_left = 0;
					s_touch_pos_y_move_left = 0;

					if (scroll_bar_selected)
						s_sem_y_offset = -1500.0 * ((s_touch_pos_y - 15.0) / 195.0);
					else if (scroll_mode)
					{
						s_touch_pos_x_move_left += s_touch_pos_x_moved;
						s_touch_pos_y_move_left += s_touch_pos_y_moved;
					}
					else if (bar_selected[0])
						s_lcd_brightness = (s_touch_pos_x / 2) + 10;
					else if (bar_selected[1])
						s_time_to_enter_afk = s_touch_pos_x * 10;
					else if (bar_selected[2])
						s_afk_lcd_brightness = (s_touch_pos_x / 2) + 10;
					else if (bar_selected[3])
						s_scroll_speed = (double)s_touch_pos_x / 300;
					else if (s_key_touch_press && s_touch_pos_y <= 219)
					{
						if (s_touch_pos_x >= 305 && s_touch_pos_x <= 320 && s_touch_pos_y >= 15)
							scroll_bar_selected = true;
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 209 && s_touch_pos_y >= 15 + s_sem_y_offset && s_touch_pos_y <= 34 + s_sem_y_offset)
						{
							s_sem_update_check_request = true;
							s_sem_show_newest_ver_data = true;
						}
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 55 + s_sem_y_offset && s_touch_pos_y <= 74 + s_sem_y_offset)
							s_setting[1] = "en";
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 55 + s_sem_y_offset && s_touch_pos_y <= 74 + s_sem_y_offset)
							s_setting[1] = "jp";

						/*else if (s_touch_pos_x >= 130 && s_touch_pos_x <= 180 && s_touch_pos_y >= 45 + s_sem_y_offset && s_touch_pos_y <= 54 + s_sem_y_offse)
						{
							if (s_sem_help_mode_num == 0)
								s_sem_help_mode_num = -1;
							else
								s_sem_help_mode_num = 0;
						}*/
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 95 + s_sem_y_offset && s_touch_pos_y <= 114 + s_sem_y_offset)
						{
							C2D_PlainImageTint(&texture_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.75), true);
							s_night_mode = true;
						}
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 95 + s_sem_y_offset && s_touch_pos_y <= 114 + s_sem_y_offset)
						{
							C2D_PlainImageTint(&texture_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);
							s_night_mode = false;
						}
						else if (s_touch_pos_x >= 210 && s_touch_pos_x <= 249 && s_touch_pos_y >= 95 + s_sem_y_offset && s_touch_pos_y <= 114 + s_sem_y_offset)
						{
							if (s_flash_mode)
								s_flash_mode = false;
							else
								s_flash_mode = true;
						}
						/*else if (s_touch_pos_x >= 130 && s_touch_pos_x <= 180 && s_touch_pos_y >= 125 + s_sem_y_offset && s_touch_pos_y <= 134 + s_sem_y_offse)
						{
							if (s_sem_help_mode_num == 1)
								s_sem_help_mode_num = -1;
							else
								s_sem_help_mode_num = 1;
						}*/
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 135 + s_sem_y_offset && s_touch_pos_y <= 154 + s_sem_y_offset)
							s_draw_vsync_mode = true;
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 135 + s_sem_y_offset && s_touch_pos_y <= 154 + s_sem_y_offset)
							s_draw_vsync_mode = false;
						/*else if (s_touch_pos_x >= 150 && s_touch_pos_x <= 200 && s_touch_pos_y >= 155 + s_sem_y_offset && s_touch_pos_y <= 164 + s_sem_y_offset)
						{
							if (s_sem_help_mode_num == 2)
								s_sem_help_mode_num = -1;
							else
								s_sem_help_mode_num = 2;
						}*/
						else if (s_touch_pos_x >= 0 && s_touch_pos_x <= 319 && s_touch_pos_y >= 170 + s_sem_y_offset && s_touch_pos_y <= 189 + s_sem_y_offset)
							bar_selected[0] = true;
						/*else if (s_touch_pos_x >= 240 && s_touch_pos_x <= 290 && s_touch_pos_y >= 205 + s_sem_y_offset && s_touch_pos_y <= 214 + s_sem_y_offset)
						{
							if (s_sem_help_mode_num == 3)
								s_sem_help_mode_num = -1;
							else
								s_sem_help_mode_num = 3;
						}*/
						else if (s_touch_pos_x >= 0 && s_touch_pos_x <= 319 && s_touch_pos_y >= 210 + s_sem_y_offset && s_touch_pos_y <= 229 + s_sem_y_offset)
							bar_selected[1] = true;
						/*else if (s_touch_pos_x >= 240 && s_touch_pos_x <= 290 && s_touch_pos_y >= 245 + s_sem_y_offset && s_touch_pos_y <= 254 + s_sem_y_offset)
						{
							if (s_sem_help_mode_num == 4)
								s_sem_help_mode_num = -1;
							else
								s_sem_help_mode_num = 4;
						}*/
						else if (s_touch_pos_x >= 0 && s_touch_pos_x <= 319 && s_touch_pos_y >= 250 + s_sem_y_offset && s_touch_pos_y <= 269 + s_sem_y_offset)
							bar_selected[2] = true;
						else if (s_touch_pos_x >= 0 && s_touch_pos_x <= 319 && s_touch_pos_y >= 290 + s_sem_y_offset && s_touch_pos_y <= 309 + s_sem_y_offset)
							bar_selected[3] = true;
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 335 + s_sem_y_offset && s_touch_pos_y <= 354 + s_sem_y_offset)
							s_allow_send_app_info = true;
						else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 199 && s_touch_pos_y >= 335 + s_sem_y_offset && s_touch_pos_y <= 354 + s_sem_y_offset)
							s_allow_send_app_info = false;
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 375 + s_sem_y_offset && s_touch_pos_y <= 394 + s_sem_y_offset)
							s_debug_mode = true;
						else if (s_touch_pos_x >= 100 && s_touch_pos_x <= 199 && s_touch_pos_y >= 375 + s_sem_y_offset && s_touch_pos_y <= 394 + s_sem_y_offset)
							s_debug_mode = false;
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 415 + s_sem_y_offset && s_touch_pos_y <= 434 + s_sem_y_offset)
						{
							s_use_specific_system_font = false;
							s_use_external_font[0] = false;
						}
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 415 + s_sem_y_offset && s_touch_pos_y <= 434 + s_sem_y_offset)
						{
							s_use_specific_system_font = false;
							s_use_external_font[0] = true;
						}
						else if (s_touch_pos_x >= 210 && s_touch_pos_x <= 299 && s_touch_pos_y >= 415 + s_sem_y_offset && s_touch_pos_y <= 434 + s_sem_y_offset)
						{
							s_use_specific_system_font = true;
							s_use_external_font[0] = false;
						}
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 79 && s_touch_pos_y >= 455 + s_sem_y_offset && s_touch_pos_y <= 474 + s_sem_y_offset)
						{
							Draw_free_system_font(s_lang_select_num);
							Draw_load_system_font(0);
							s_lang_select_num = 0;
						}
						else if (s_touch_pos_x >= 85 && s_touch_pos_x <= 154 && s_touch_pos_y >= 455 + s_sem_y_offset && s_touch_pos_y <= 474 + s_sem_y_offset)
						{
							Draw_free_system_font(s_lang_select_num);
							Draw_load_system_font(1);
							s_lang_select_num = 1;
						}
						else if (s_touch_pos_x >= 160 && s_touch_pos_x <= 229 && s_touch_pos_y >= 455 + s_sem_y_offset && s_touch_pos_y <= 474 + s_sem_y_offset)
						{
							Draw_free_system_font(s_lang_select_num);
							Draw_load_system_font(2);
							s_lang_select_num = 2;
						}
						else if (s_touch_pos_x >= 235 && s_touch_pos_x <= 304 && s_touch_pos_y >= 455 + s_sem_y_offset && s_touch_pos_y <= 474 + s_sem_y_offset)
						{
							Draw_free_system_font(s_lang_select_num);
							Draw_load_system_font(3);
							s_lang_select_num = 3;
						}
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 209 && s_touch_pos_y >= 515 + s_sem_y_offset && s_touch_pos_y <= 1434 + s_sem_y_offset)
						{
							scan_hid_load_font_num = -1;
							for (int i = 0; i < 46; i++)
							{
								if (s_touch_pos_y >= 515 + s_sem_y_offset + (i * 20) && s_touch_pos_y <= 534 + s_sem_y_offset + (i * 20))
									scan_hid_load_font_num = i;
							}

							if (scan_hid_load_font_num != -1)
							{
								if (s_use_external_font[scan_hid_load_font_num + 1])
								{
									Draw_free_texture(5 + scan_hid_load_font_num);
									for (int i = s_font_start_num[scan_hid_load_font_num]; i < s_font_characters[scan_hid_load_font_num]; i++)
										font_images[i].tex = NULL;

									s_use_external_font[scan_hid_load_font_num + 1] = false;
								}
								else
								{
									scan_hid_log_num_return = Share_app_log_save("Share/Scan hid/c2d", "Loading texture (" + s_font_file_name[scan_hid_load_font_num] + "_font.t3x)...", 1234567890, false);
									scan_hid_result = Draw_load_texture("romfs:/gfx/font/" + s_font_file_name[scan_hid_load_font_num] + "_font.t3x", scan_hid_load_font_num + 5, font_images, s_font_start_num[scan_hid_load_font_num], s_font_characters[scan_hid_load_font_num]);
									Share_app_log_add_result(scan_hid_log_num_return, scan_hid_result.string, scan_hid_result.code, false);

									if (scan_hid_result.code == 0)
										s_use_external_font[scan_hid_load_font_num + 1] = true;
									else
									{
										Draw_free_texture(5 + scan_hid_load_font_num);
										for (int i = s_font_start_num[scan_hid_load_font_num]; i < s_font_characters[scan_hid_load_font_num]; i++)
											font_images[i].tex = NULL;
									}
								}
							}
						}
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 109 && s_touch_pos_y >= 495 + s_sem_y_offset && s_touch_pos_y <= 514 + s_sem_y_offset)
						{
							for (int j = 0; j < 46; j++)
							{
								if (!s_use_external_font[j + 1])
								{
									scan_hid_log_num_return = Share_app_log_save("Share/Scan hid/c2d", "Loading texture (" + s_font_file_name[j] + "_font.t3x)...", 1234567890, false);
									scan_hid_result = Draw_load_texture("romfs:/gfx/font/" + s_font_file_name[j] + "_font.t3x", j + 5, font_images, s_font_start_num[j], s_font_characters[j]);
									Share_app_log_add_result(scan_hid_log_num_return, scan_hid_result.string, scan_hid_result.code, false);

									if (scan_hid_result.code == 0)
										s_use_external_font[j + 1] = true;
									else
									{
										Draw_free_texture(5 + j);
										for (int i = s_font_start_num[j]; i < s_font_characters[j]; i++)
											font_images[i].tex = NULL;
									}
								}
							}
						}
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 209 && s_touch_pos_y >= 495 + s_sem_y_offset && s_touch_pos_y <= 514 + s_sem_y_offset)
						{
							for (int j = 0; j < 46; j++)
							{
								if (s_use_external_font[j + 1])
								{
									Draw_free_texture(5 + j);
									for (int i = s_font_start_num[j]; i < s_font_characters[j]; i++)
										font_images[i].tex = NULL;

									s_use_external_font[j + 1] = false;
								}
							}
						}
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 1450 + s_sem_y_offset && s_touch_pos_y <= 1469 + s_sem_y_offset)
							s_line_log_httpc_buffer_size += 0x100000;
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 1450 + s_sem_y_offset && s_touch_pos_y <= 1469 + s_sem_y_offset)
							s_line_log_httpc_buffer_size -= 0x100000;
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 1490 + s_sem_y_offset && s_touch_pos_y <= 1509 + s_sem_y_offset)
							s_line_log_fs_buffer_size += 0x100000;
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 1490 + s_sem_y_offset && s_touch_pos_y <= 1509 + s_sem_y_offset)
							s_line_log_fs_buffer_size -= 0x100000;
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 1530 + s_sem_y_offset && s_touch_pos_y <= 1549 + s_sem_y_offset)
							s_spt_spt_httpc_buffer_size += 0x100000;
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 1530 + s_sem_y_offset && s_touch_pos_y <= 1549 + s_sem_y_offset)
							s_spt_spt_httpc_buffer_size -= 0x100000;
						else if (s_touch_pos_x >= 10 && s_touch_pos_x <= 99 && s_touch_pos_y >= 1570 + s_sem_y_offset && s_touch_pos_y <= 1589 + s_sem_y_offset)
							s_imv_image_httpc_buffer_size += 0x100000;
						else if (s_touch_pos_x >= 110 && s_touch_pos_x <= 199 && s_touch_pos_y >= 1570 + s_sem_y_offset && s_touch_pos_y <= 1589 + s_sem_y_offset)
							s_imv_image_httpc_buffer_size -= 0x100000;
						else
							scroll_mode = true;
					}
				}
				else
				{
					for (int i = 0; i < 4; i++)
						bar_selected[i] = false;

					scroll_mode = false;
					scroll_bar_selected = false;
					s_touch_pos_x_move_left -= (s_touch_pos_x_move_left * 0.025);
					s_touch_pos_y_move_left -= (s_touch_pos_y_move_left * 0.025);
					if (s_touch_pos_x_move_left < 0.5 && s_touch_pos_x_move_left > -0.5)
						s_touch_pos_x_move_left = 0;
					if (s_touch_pos_y_move_left < 0.5 && s_touch_pos_y_move_left > -0.5)
						s_touch_pos_y_move_left = 0;
				}

				if (s_key_CPAD_DOWN_held || s_key_CPAD_UP_held)
					s_sem_y_offset += ((float)circle_pos.dy * s_scroll_speed) * 0.0625;

				s_sem_y_offset -= (s_touch_pos_y_move_left * s_scroll_speed);

				if (s_line_log_httpc_buffer_size > 0x1000000)
					s_line_log_httpc_buffer_size = 0x1000000;
				else if (s_line_log_httpc_buffer_size < 0x100000)
					s_line_log_httpc_buffer_size = 0x100000;
				if (s_line_log_fs_buffer_size > 0x1000000)
					s_line_log_fs_buffer_size = 0x1000000;
				else if (s_line_log_fs_buffer_size < 0x100000)
					s_line_log_fs_buffer_size = 0x100000;
				if (s_spt_spt_httpc_buffer_size > 0x1000000)
					s_spt_spt_httpc_buffer_size = 0x1000000;
				else if (s_spt_spt_httpc_buffer_size < 0x100000)
					s_spt_spt_httpc_buffer_size = 0x100000;
				if (s_imv_image_httpc_buffer_size > 0x1000000)
					s_imv_image_httpc_buffer_size = 0x1000000;
				else if (s_imv_image_httpc_buffer_size < 0x100000)
					s_imv_image_httpc_buffer_size = 0x100000;
				if (s_sem_y_offset >= 0)
					s_sem_y_offset = 0;
				else if (s_sem_y_offset <= -1500)
					s_sem_y_offset = -1500;
			}
		}
		else if (s_line_main_run && !s_hid_disabled)
		{
			if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
			{
				s_line_main_run = false;
				s_menu_main_run = true;
				s_line_thread_suspend = true;
			}

			if (s_line_send_message_check[0] || s_line_send_message_check[1])
			{
				if (s_key_A_press || (s_key_touch_press && s_touch_pos_x >= 100 && s_touch_pos_x <= 149 && s_touch_pos_y >= 150 && s_touch_pos_y <= 164))
				{				
					if (s_line_send_message_check[0])
					{
						s_line_send_message_request = true;
						s_line_send_message_check[0] = false;
					}
					else if (s_line_send_message_check[1])
					{
						s_line_send_sticker_request = true;
						s_line_send_message_check[1] = false;
					}
				}
				else if (s_key_B_press || (s_key_touch_press && s_touch_pos_x >= 170 && s_touch_pos_x <= 219 && s_touch_pos_y >= 150 && s_touch_pos_y <= 164))
				{
					if (s_line_send_message_check[0])
						s_line_send_message_check[0] = false;
					else if (s_line_send_message_check[1])
						s_line_send_message_check[1] = false;
				}
			}
			else if (s_line_select_sticker_request)
			{
				for (int i = 0; i < 10; i++)
				{
					if (s_key_touch_press && s_touch_pos_x >= 10 + (i * 30) && s_touch_pos_x <= 39 + (i * 30) && s_touch_pos_y > 140 && s_touch_pos_y < 149)
						s_line_sticker_tab_select_num = i;
				}

				for (int i = 0; i < 7; i++)
				{
					if (s_key_touch_press && s_touch_pos_x >= 20 + (i * 50) && s_touch_pos_x <= 49 + (i * 50) && s_touch_pos_y > 150 && s_touch_pos_y < 179)
					{
						s_line_sticker_select_num = (s_line_sticker_tab_select_num * 12) + i + 1;
						s_line_send_message_check[1] = true;
						s_line_select_sticker_request = false;
					}
					else if (s_key_touch_press && s_touch_pos_x >= 20 + (i * 50) && s_touch_pos_x <= 49 + (i * 50) && s_touch_pos_y > 190 && s_touch_pos_y < 219)
					{
						s_line_sticker_select_num = (s_line_sticker_tab_select_num * 12) + i + 7;
						s_line_send_message_check[1] = true;
						s_line_select_sticker_request = false;
					}
				}

				if (s_key_B_press || (s_key_touch_press && s_touch_pos_x >= 290 && s_touch_pos_x <= 309 && s_touch_pos_y > 120 && s_touch_pos_y < 139))
					s_line_select_sticker_request = false;
			}
			else
			{
				if (s_key_touch_press || s_key_touch_held)
				{
					s_touch_pos_x_move_left = 0;
					s_touch_pos_y_move_left = 0;

					if (scroll_bar_selected)
					{
						s_touch_pos_x_move_left = 0;
						s_touch_pos_y_move_left = 0;
						text_y_cache = s_line_bottom_y * ((s_touch_pos_y - 15.0) / 195.0);
					}
					else if (scroll_mode)
					{
						s_touch_pos_x_move_left += s_touch_pos_x_moved;
						s_touch_pos_y_move_left += s_touch_pos_y_moved;
					}
					else if (s_key_touch_press && s_touch_pos_y <= 219)
					{
						if (s_touch_pos_x >= 305 && s_touch_pos_x <= 320 && s_touch_pos_y >= 15 && s_touch_pos_y < 220)
							scroll_bar_selected = true;
						else if (s_touch_pos_x > 260 && s_touch_pos_x < 300 && s_touch_pos_y > 140 && s_touch_pos_y < 150 && s_line_menu_mode != 1)
						{
							if (s_line_hide_id)
								s_line_hide_id = false;
							else
								s_line_hide_id = true;
						}	
						else if (s_touch_pos_x > 10 && s_touch_pos_x < 60 && s_touch_pos_y > 170 && s_touch_pos_y < 180)
							s_line_menu_mode = 0;
						else if (s_touch_pos_x > 60 && s_touch_pos_x < 110 && s_touch_pos_y > 170 && s_touch_pos_y < 180)
							s_line_menu_mode = 1;
						else if (s_touch_pos_x > 110 && s_touch_pos_x < 210 && s_touch_pos_y > 170 && s_touch_pos_y < 180)
							s_line_menu_mode = 2;
						else if (s_touch_pos_x > 210 && s_touch_pos_x < 310 && s_touch_pos_y > 170 && s_touch_pos_y < 180)
							s_line_menu_mode = 3;
						else if (s_touch_pos_y <= 169)
							scroll_mode = true;
					}
				}
				else
				{
					scroll_mode = false;
					scroll_bar_selected = false;
					s_touch_pos_x_move_left -= (s_touch_pos_x_move_left * 0.025);
					s_touch_pos_y_move_left -= (s_touch_pos_y_move_left * 0.025);
					if (s_touch_pos_x_move_left < 0.5 && s_touch_pos_x_move_left > -0.5)
						s_touch_pos_x_move_left = 0;
					if (s_touch_pos_y_move_left < 0.5 && s_touch_pos_y_move_left > -0.5)
						s_touch_pos_y_move_left = 0;
				}

				if (s_key_CPAD_DOWN_held || s_key_CPAD_UP_held)
				{
					if (s_held_time > 600)
						text_y_cache += ((float)circle_pos.dy * s_scroll_speed) * 0.5;
					else if (s_held_time > 240)
						text_y_cache += ((float)circle_pos.dy * s_scroll_speed) * 0.125;
					else
						text_y_cache += ((float)circle_pos.dy * s_scroll_speed) * 0.0625;
				}
				if (s_key_CPAD_LEFT_held || s_key_CPAD_RIGHT_held)
				{
					if (s_held_time > 240)
						text_x_cache -= ((float)circle_pos.dx * s_scroll_speed) * 0.125;
					else
						text_x_cache -= ((float)circle_pos.dx * s_scroll_speed) * 0.0625;
				}
				if (s_key_DRIGHT_press || (s_key_touch_press && s_touch_pos_x >= 210 && s_touch_pos_x <= 320 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
				{
					s_line_room_select_num += 1;
					if (s_line_room_select_num > 99)
						s_line_room_select_num = 99;

					s_line_log_load_request = true;
				}
				if (s_key_DLEFT_press || (s_key_touch_press && s_touch_pos_x >= 0 && s_touch_pos_x <= 110 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
				{
					s_line_room_select_num -= 1;
					if (s_line_room_select_num < 0)
						s_line_room_select_num = 0;

					s_line_log_load_request = true;
				}

				if (s_line_menu_mode == 0)
				{
					if (s_key_A_press || (s_key_touch_press && s_touch_pos_x >= 20 && s_touch_pos_x <= 149 && s_touch_pos_y >= 185 && s_touch_pos_y <= 197))
						s_line_type_message_request = true;
					if (s_key_B_press || (s_key_touch_press && s_touch_pos_x >= 20 && s_touch_pos_x <= 149 && s_touch_pos_y >= 205 && s_touch_pos_y <= 217))
						s_line_log_update_request = true;
					if (s_key_Y_press || (s_key_touch_press && s_touch_pos_x >= 170 && s_touch_pos_x <= 299 && s_touch_pos_y >= 185 && s_touch_pos_y <= 197))
						s_line_select_sticker_request = true;
					if (s_key_touch_press && s_touch_pos_x >= 170 && s_touch_pos_x <= 299 && s_touch_pos_y >= 205 && s_touch_pos_y <= 217)
					{
						if (s_line_auto_update_mode)
							s_line_auto_update_mode = false;
						else
							s_line_auto_update_mode = true;
					}
				}
				else if (s_line_menu_mode == 1)
				{
					if (s_key_DUP_press || (s_key_touch_press && s_touch_pos_x > 170 && s_touch_pos_x < 230 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
						message_select_num += 0.5f;
					if (s_key_DDOWN_press || (s_key_touch_press && s_touch_pos_x > 240 && s_touch_pos_x < 300 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
						message_select_num -= 0.5f;
					if (s_key_DUP_held || (s_key_touch_held && s_touch_pos_x > 170 && s_touch_pos_x < 230 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
					{
						if (s_held_time > 180)
							message_select_num += 1.0f;
						else
							message_select_num += 0.0625f;
					}
					if (s_key_DDOWN_held || (s_key_touch_held && s_touch_pos_x > 240 && s_touch_pos_x < 300 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
					{
						if (s_held_time > 180)
							message_select_num -= 1.0f;
						else
							message_select_num -= 0.0625f;
					}
					if ((s_key_ZR_press || (s_key_touch_press && s_touch_pos_x > 20 && s_touch_pos_x < 150 && s_touch_pos_y > 185 && s_touch_pos_y < 215)))
						s_clipboards[0] = s_line_message_log[(int)message_select_num];

					if ((int)message_select_num > 299)
						message_select_num = 299.0;
					else if ((int)message_select_num < 0)
						message_select_num = 0.0;
				}
				else if (s_line_menu_mode == 2)
				{
					if (s_key_DUP_held || (s_key_touch_held && s_touch_pos_x > 20 && s_touch_pos_x < 80 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
						text_interval_cache += 0.5;
					if (s_key_DDOWN_held || (s_key_touch_held && s_touch_pos_x > 90 && s_touch_pos_x < 150 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
						text_interval_cache -= 0.5;
					if (s_key_L_held || (s_key_touch_held && s_touch_pos_x > 170 && s_touch_pos_x < 230 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
						text_size_cache -= 0.003f;
					if (s_key_R_held || (s_key_touch_held && s_touch_pos_x > 240 && s_touch_pos_x < 300 && s_touch_pos_y > 185 && s_touch_pos_y < 215))
						text_size_cache += 0.003f;

					if (text_interval_cache > 250)
						text_interval_cache = 250;
					else if (text_interval_cache < 10)
						text_interval_cache = 10;
					if (text_size_cache > 3.0)
						text_size_cache = 3.0f;
					else if (text_size_cache < 0.25)
						text_size_cache = 0.25f;
				}
				else if (s_line_menu_mode == 3)
				{
					if (s_key_Y_press || (s_key_touch_press && s_touch_pos_x >= 20 && s_touch_pos_x <= 149 && s_touch_pos_y >= 185 && s_touch_pos_y <= 197))
						s_line_type_id_request = true;
					if (s_key_X_press || (s_key_touch_press && s_touch_pos_x >= 20 && s_touch_pos_x <= 149 && s_touch_pos_y >= 205 && s_touch_pos_y <= 217))
						s_line_type_main_url_request = true;
					if (s_key_A_press || (s_key_touch_press && s_touch_pos_x >= 170 && s_touch_pos_x <= 299 && s_touch_pos_y >= 185 && s_touch_pos_y <= 197))
						s_line_type_app_ps_request = true;
					if (s_key_B_press || (s_key_touch_press && s_touch_pos_x >= 170 && s_touch_pos_x <= 299 && s_touch_pos_y >= 205 && s_touch_pos_y <= 217))
						s_line_type_script_ps_request = true;
				}

				text_x_cache -= (s_touch_pos_x_move_left * s_scroll_speed);
				text_y_cache -= (s_touch_pos_y_move_left * s_scroll_speed);

				if (text_y_cache > 0.0)
					text_y_cache = 0.0;
				if (text_y_cache < s_line_bottom_y)
					text_y_cache = s_line_bottom_y;
				if (text_x_cache > 40.0)
					text_x_cache = 40.0f;
				if (text_x_cache < -500.0)
					text_x_cache = -500.0f;
			}
		}
		else if (s_gtr_main_run && !s_hid_disabled)
		{
			if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
			{
				s_gtr_thread_suspend = true;
				s_menu_main_run = true;
				s_gtr_main_run = false;
			}
			if (s_key_A_press)
				s_gtr_type_text_request = true;
			if (s_key_DRIGHT_held)
				s_gtr_text_x -= 1.0;
			if (s_key_DLEFT_held)
				s_gtr_text_x += 1.0;
			if (s_key_DUP_press)
			{
				s_gtr_selected_history_num--;
				if (s_gtr_selected_history_num <= -1)
					s_gtr_selected_history_num = 0;
			}
			if (s_key_DDOWN_press)
			{
				s_gtr_selected_history_num++;
				if (s_gtr_selected_history_num >= 16)
					s_gtr_selected_history_num = 15;
			}
			if (s_key_ZL_press)
				s_clipboards[0] = s_gtr_history[s_gtr_selected_history_num];

		}
		else if (s_spt_main_run && !s_hid_disabled)
		{
			if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
			{
				s_spt_thread_suspend = true;
				s_menu_main_run = true;
				s_spt_main_run = false;
			}
			if (s_key_touch_press)
			{
				for (int i = 0; i < 7; i++)
				{
					if (s_touch_pos_x >= 100 && s_touch_pos_x <= 230 && s_touch_pos_y >= 40 + (i * 20) && s_touch_pos_y <= 59 + (i * 20))
						s_spt_data_size = i;
				}
			}
			if (s_key_A_press || (s_touch_pos_x >= 150 && s_touch_pos_x <= 170 && s_touch_pos_y >= 190 && s_touch_pos_y <= 209))
				s_spt_start_request = true;
		}
		else if (s_imv_main_run && !s_hid_disabled)
		{
			if (s_key_START_press || (s_key_touch_press && s_touch_pos_x >= 110 && s_touch_pos_x <= 230 && s_touch_pos_y >= 220 && s_touch_pos_y <= 240))
			{
				s_imv_thread_suspend = true;
				s_menu_main_run = true;
				s_imv_main_run = false;
			}
			if (s_key_touch_press || s_key_touch_held)
			{
				s_touch_pos_x_move_left = 0;
				s_touch_pos_y_move_left = 0;

				if (scroll_mode)
				{
					s_touch_pos_x_move_left += s_touch_pos_x_moved;
					s_touch_pos_y_move_left += s_touch_pos_y_moved;
				}
				else if (s_key_touch_press && s_touch_pos_y <= 219)
					scroll_mode = true;
			}
			else
			{
				scroll_mode = false;
				s_touch_pos_x_move_left -= (s_touch_pos_x_move_left * 0.025);
				s_touch_pos_y_move_left -= (s_touch_pos_y_move_left * 0.025);
				if (s_touch_pos_x_move_left < 0.5 && s_touch_pos_x_move_left > -0.5)
					s_touch_pos_x_move_left = 0;
				if (s_touch_pos_y_move_left < 0.5 && s_touch_pos_y_move_left > -0.5)
					s_touch_pos_y_move_left = 0;
			}
			
			if (s_key_A_press || (s_key_touch_press && s_touch_pos_x > 15 && s_touch_pos_x < 65 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
				s_imv_image_parse_request = true;
			if (s_key_B_press || (s_key_touch_press && s_touch_pos_x > 75 && s_touch_pos_x < 125 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
				s_imv_image_dl_request = true;
			if (s_key_Y_press || (s_key_touch_press && s_touch_pos_x > 135 && s_touch_pos_x < 185 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
				s_imv_adjust_url_request = true;
			if (s_key_DUP_press || (s_key_touch_press && s_touch_pos_x > 195 && s_touch_pos_x < 245 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
				s_imv_clipboard_select_num++;
			if (s_key_DDOWN_press || (s_key_touch_press && s_touch_pos_x > 255 && s_touch_pos_x < 305 && s_touch_pos_y > 175 && s_touch_pos_y < 195))
				s_imv_clipboard_select_num--;
			if (s_key_CPAD_UP_held || s_key_CPAD_DOWN_held)
				s_imv_image_pos_y += ((float)circle_pos.dy * s_scroll_speed) * 0.0625;
			if (s_key_CPAD_LEFT_held || s_key_CPAD_RIGHT_held)
				s_imv_image_pos_x -= ((float)circle_pos.dx * s_scroll_speed) * 0.0625;
			if (s_key_L_held || (s_key_touch_held && s_touch_pos_x > 145 && s_touch_pos_x < 220 && s_touch_pos_y > 200 && s_touch_pos_y < 220))
			{
				s_imv_image_zoom -= 0.05f;
				if (s_imv_image_zoom < 0.25)
					s_imv_image_zoom = 0.25f;
			}
			if (s_key_R_held || (s_key_touch_held && s_touch_pos_x > 245 && s_touch_pos_x < 320 && s_touch_pos_y > 200 && s_touch_pos_y < 220))
				s_imv_image_zoom += 0.05f;

			s_imv_image_pos_x -= (s_touch_pos_x_move_left * s_scroll_speed);
			s_imv_image_pos_y -= (s_touch_pos_y_move_left * s_scroll_speed);
			if (s_imv_clipboard_select_num < 0)
				s_imv_clipboard_select_num = 0;
			if (s_imv_clipboard_select_num > 14)
				s_imv_clipboard_select_num = 14;
		}

		if (s_key_A_press || s_key_B_press || s_key_X_press || s_key_Y_press || s_key_DRIGHT_press
			|| s_key_DLEFT_press || s_key_ZL_press || s_key_ZR_press || s_key_START_press
			|| s_key_SELECT_press || s_key_touch_press || s_key_A_held || s_key_B_held
			|| s_key_X_held || s_key_Y_held || s_key_DDOWN_held || s_key_DRIGHT_held
			|| s_key_DLEFT_held || s_key_CPAD_UP_held || s_key_CPAD_DOWN_held || s_key_CPAD_RIGHT_held
			|| s_key_CPAD_LEFT_held || s_key_DUP_held || s_key_touch_held)
			s_afk_time = 0;
		if (s_key_DUP_held || s_key_DDOWN_held || s_key_DRIGHT_held || s_key_DLEFT_held
			|| s_key_CPAD_UP_held || s_key_CPAD_DOWN_held || s_key_CPAD_RIGHT_held
			|| s_key_CPAD_LEFT_held || s_key_touch_held)
			s_held_time++;
		else
			s_held_time = 0;

		usleep(16500);
	}
	Share_app_log_save("Share/Scan hid thread", "Thread exit", 1234567890, false);
}

std::string Share_text_sort(std::string sorce_part_string[])
{
	int arabic_pos = -1;
	bool arabic_found = false;
	std::string result_string = "";
	for (int i = 0; i < 256; i++)
	{
		if (memcmp((void*)sorce_part_string[i].c_str(), (void*)s_basic_latin_font_sample[0].c_str(), 0x1) == 0)
			break;

		for (int j = 0; j < 235; j++)
		{
			arabic_found = false;
			if (memcmp((void*)sorce_part_string[i].c_str(), (void*)s_arabic_right_to_left_sample[j].c_str(), 0x2) == 0)
			{
				arabic_found = true;
				if (arabic_pos <= -1)
					arabic_pos = result_string.length();

				result_string.insert(arabic_pos, sorce_part_string[i]);
				break;
			}
		}

		if (!arabic_found)
		{
			result_string += sorce_part_string[i];
			arabic_pos = -1;
		}
	}

	return result_string;
}

void Share_text_parse(std::string sorce_string, std::string part_string[])
{
	int sorce_string_length = sorce_string.length();
	int i = 0;
	int std_num = 0;
	int parse_string_length = 0;

	char* sorce_string_char = (char*)malloc(sorce_string.length() + 10);

	memset(sorce_string_char, 0x0, sorce_string.length() + 10);
	strcpy(sorce_string_char, (char*)sorce_string.c_str());
	for (int i = 0; i < 256; i++)
		memset((void*)part_string[i].c_str(), 0x0, 0x4);

	while (true)
	{
		parse_string_length = mblen(&sorce_string_char[i], 4);

		if (i >= sorce_string_length)
			break;
		else if (parse_string_length >= 1)
		{
			part_string[std_num] = sorce_string.substr(i, parse_string_length);
			i += parse_string_length;
			std_num++;
		}
		else
			i++;
	}
	free(sorce_string_char);
}

void Share_draw_external_fonts(std::string string, float texture_x, float texture_y, float texture_size_x, float texture_size_y, bool follow_new_line)
{
	float texture_x_offset = 0;
	float interval_offset = 2.5;
	bool unknown_char = false;
	std::string part_string[256];

	Share_text_parse(string, part_string);
	Share_text_parse(Share_text_sort(part_string), part_string);

	for (int i = 0; i < 256; i++)
	{
		unknown_char = true;

		if (memcmp((void*)part_string[i].c_str(), (void*)s_basic_latin_font_sample[0].c_str(), 0x1) == 0)
			break;

		if (part_string[i].length() == 1)
		{
			if (s_use_external_font[0])
			{
				for (int j = 1; j < 128; j++) //basic latin
				{
					if (memcmp((void*)part_string[i].c_str(), (void*)s_basic_latin_font_sample[j].c_str(), 0x1) == 0)
					{
						if (follow_new_line)
						{
							if (j == 10)
							{
								texture_y += 40.0 * texture_size_y;
								texture_x_offset = 0;
								unknown_char = false;
								break;
							}
						}

						if (j <= 32 || j == 127)
							j = 1;
						else
							j -= 31;

						texture_x_offset += (s_basic_latin_font_interval[j] + interval_offset) * texture_size_x / 2;
						Draw_texture(font_images, texture_tint, s_font_start_num[0] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
						unknown_char = false;
						texture_x_offset += (s_basic_latin_font_interval[j] + interval_offset) * texture_size_x / 2;
						break;
					}
				}
			}
		}
		else if (part_string[i].length() == 2)
		{
			s_font_pos = -1;
			for (int k = 0; k < 2; k++)
			{
				if (memcmp((void*)part_string[i].c_str(), (void*)s_font_samples[k].c_str(), 0x2) >= 0)
					s_font_pos = k;
			}

			if (s_font_pos == 0)
			{
				if (s_use_external_font[1])
				{
					for (int j = 0; j < 128; j++) //latin 1 supplement
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_latin_1_supplement_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_latin_1_supplement_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[1] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_latin_1_supplement_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[2])
				{
					for (int j = 0; j < 92; j++) //ipa extensions
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_ipa_extensions_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_ipa_extensions_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[2] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_ipa_extensions_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[3])
				{
					for (int j = 0; j < 211; j++) //spacing modifier letters
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_spacing_modifier_letters_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_spacing_modifier_letters_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[3] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_spacing_modifier_letters_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[4])
				{
					for (int j = 0; j < 112; j++) //combining diacritical marks
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_combining_diacritical_marks_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_combining_diacritical_marks_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[4] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_combining_diacritical_marks_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[5])
				{
					for (int j = 0; j < 135; j++) //greek and coptic
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_greek_and_coptic_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_greek_and_coptic_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[5] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_greek_and_coptic_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 1)
			{
				if (s_use_external_font[6])
				{
					for (int j = 0; j < 256; j++) //cyrillic
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_cyrillic_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_cyrillic_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[6] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_cyrillic_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[7])
				{
					for (int j = 0; j < 48; j++) //cyrillic supplement
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_cyrillic_supplement_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_cyrillic_supplement_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[7] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_cyrillic_supplement_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[8])
				{
					for (int j = 0; j < 91; j++) //armenian
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_armenian_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_armenian_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[8] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_armenian_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[9])
				{
					for (int j = 0; j < 88; j++) //hebrew
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_hebrew_font_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_hebrew_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[9] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_hebrew_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[10])
				{
					for (int j = 0; j < 255; j++) //arabic
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_arabic_sample[j].c_str(), 0x2) == 0)
						{
							texture_x_offset += (s_arabic_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[10] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_arabic_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == -1)
				Share_app_log_save("", "2 bytes : unknown error", 1234567890, false);
		}
		else if (part_string[i].length() == 3)
		{
			s_font_pos = -1;
			for (int k = 2; k < 62; k++)
			{
				if (memcmp((void*)part_string[i].c_str(), (void*)s_font_samples[k].c_str(), 0x3) >= 0)
					s_font_pos = k;
			}

			if (s_font_pos == 2)
			{
				if (s_use_external_font[11])
				{
					for (int j = 0; j < 128; j++) //devanagari
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_devanagari_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_devanagari_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[11] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_devanagari_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[12])
				{
					for (int j = 0; j < 79; j++) //gurmukhi
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_gurmukhi_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_gurmukhi_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[12] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_gurmukhi_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[13])
				{
					for (int j = 0; j < 72; j++) //tamil
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_tamil_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_tamil_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[13] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_tamil_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 3)
			{
				if (s_use_external_font[14])
				{
					for (int j = 0; j < 96; j++) //telugu
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_telugu_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_telugu_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[14] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_telugu_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[15])
				{
					for (int j = 0; j < 88; j++) //kannada
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_kannada_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_kannada_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[15] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_kannada_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[16])
				{
					for (int j = 0; j < 90; j++) //sinhala
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_sinhala_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_sinhala_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[16] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_sinhala_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[17])
				{
					for (int j = 0; j < 87; j++) //thai
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_thai_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_thai_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[17] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_thai_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[18])
				{
					for (int j = 0; j < 67; j++) //lao
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_lao_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_lao_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[18] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_lao_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[19])
				{
					for (int j = 0; j < 211; j++) //tibetan
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_tibetan_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_tibetan_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[19] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_tibetan_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 4)
			{
				if (s_use_external_font[20])
				{
					for (int j = 0; j < 88; j++) //georgian
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_georgian_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_georgian_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[20] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_georgian_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 5)
			{
				if (s_use_external_font[21])
				{
					for (int j = 0; j < 640; j++) //unified canadian aboriginal syllabics
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_unified_canadian_aboriginal_syllabics_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_unified_canadian_aboriginal_syllabics_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[21] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_unified_canadian_aboriginal_syllabics_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 7)
			{
				if (s_use_external_font[22])
				{
					for (int j = 0; j < 128; j++) //phonetic extensions
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_phonetic_extensions_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_phonetic_extensions_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[22] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_phonetic_extensions_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[23])
				{
					for (int j = 0; j < 63; j++) //combining diacritical marks supplement
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_combining_diacritical_marks_supplement_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_combining_diacritical_marks_supplement_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[23] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_combining_diacritical_marks_supplement_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[24])
				{
					for (int j = 0; j < 233; j++) //greek extended
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_greek_extended_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_greek_extended_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[24] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_greek_extended_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 8)
			{
				if (s_use_external_font[25])
				{
					for (int j = 0; j < 111; j++) //general punctuation
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_general_punctuation_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_general_punctuation_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[25] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_general_punctuation_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[26])
				{
					for (int j = 0; j < 42; j++) //superscripts and subscripts
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_superscripts_and_subscripts_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_superscripts_and_subscripts_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[26] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_superscripts_and_subscripts_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[27])
				{
					for (int j = 0; j < 33; j++) //combining diacritical marks for symbols
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_combining_diacritical_marks_for_symbols_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_combining_diacritical_marks_for_symbols_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[27] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_combining_diacritical_marks_for_symbols_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[28])
				{
					for (int j = 0; j < 112; j++) //arrows
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_arrows_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_arrows_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[28] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_arrows_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[29])
				{
					for (int j = 0; j < 256; j++) //mathematical operators
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_mathematical_operators_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_mathematical_operators_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[29] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_mathematical_operators_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[30])
				{
					for (int j = 0; j < 256; j++) //miscellaneous technical
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_miscellaneous_technical_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_miscellaneous_technical_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[30] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_miscellaneous_technical_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 9)
			{
				if (s_use_external_font[31])
				{
					for (int j = 0; j < 11; j++) //optical character recognition
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_optical_character_recognition_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_optical_character_recognition_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[31] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_optical_character_recognition_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[32])
				{
					for (int j = 0; j < 128; j++) //box drawing
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_box_drawing_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_box_drawing_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[32] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_box_drawing_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[33])
				{
					for (int j = 0; j < 32; j++) //block elements
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_block_elements_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_block_elements_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[33] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_block_elements_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[34])
				{
					for (int j = 0; j < 96; j++) //geometric shapes
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_geometric_shapes_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_geometric_shapes_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[34] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_geometric_shapes_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[35])
				{
					for (int j = 0; j < 256; j++) //miscellaneous symbols
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_miscellaneous_symbols_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_miscellaneous_symbols_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[35] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_miscellaneous_symbols_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[36])
				{
					for (int j = 0; j < 192; j++) //dingbats
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_dingbats_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_dingbats_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[36] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_dingbats_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 12)
			{
				if (s_use_external_font[37])
				{
					for (int j = 0; j < 64; j++) //symbol and punctuation
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_cjk_symbol_and_punctuation_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_cjk_symbol_and_punctuation_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[37] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_cjk_symbol_and_punctuation_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[38])
				{
					for (int j = 0; j < 93; j++) //hiragana
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_hiragana_font_sample[j].c_str(), 0x3) == 0)
						{

							texture_x_offset += (s_hiragana_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[38] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_hiragana_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[39])
				{
					for (int j = 0; j < 96; j++) //katakana
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_katakana_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_katakana_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[39] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_katakana_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[40])
				{
					for (int j = 0; j < 94; j++) //hangul compatibility jamo
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_hangul_compatibility_jamo_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_hangul_compatibility_jamo_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[40] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_hangul_compatibility_jamo_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 40 || s_font_pos == 41)
			{
				if (s_use_external_font[41])
				{
					for (int j = 0; j < 1165; j++) //yi syllables
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_yi_syllables_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_yi_syllables_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[41] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_yi_syllables_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[42])
				{
					for (int j = 0; j < 55; j++) //yi radicals
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_yi_radicals_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_yi_radicals_font_interval[j] + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[42] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_yi_radicals_font_interval[j] + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == 61)
			{
				if (s_use_external_font[43])
				{
					for (int j = 0; j < 32; j++) //cjk compatibility forms
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_cjk_compatibility_forms_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_cjk_compatibility_forms_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[43] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_cjk_compatibility_forms_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}

				if (s_use_external_font[44])
				{
					for (int j = 0; j < 225; j++) //halfwidth and fullwidth forms
					{
						if (memcmp((void*)part_string[i].c_str(), (void*)s_halfwidth_and_fullwidth_forms_font_sample[j].c_str(), 0x3) == 0)
						{
							texture_x_offset += (s_halfwidth_and_fullwidth_forms_font_interval + interval_offset) * texture_size_x / 2;
							Draw_texture(font_images, texture_tint, s_font_start_num[44] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
							unknown_char = false;
							texture_x_offset += (s_halfwidth_and_fullwidth_forms_font_interval + interval_offset) * texture_size_x / 2;
							break;
						}
					}
				}
			}
			else if (s_font_pos == -1)
				Share_app_log_save("", "3 bytes : unknown error", 1234567890, false);
		}
		else if (part_string[i].length() == 4)
		{
			if (s_use_external_font[45])
			{
				for (int j = 0; j < 768; j++) //miscellaneous symbols and pictographs
				{
					if (memcmp((void*)part_string[i].c_str(), (void*)s_miscellaneous_symbols_and_pictographs_font_sample[j].c_str(), 0x4) == 0)
					{
						texture_x_offset += (s_miscellaneous_symbols_and_pictographs_font_interval + interval_offset) * texture_size_x / 2;
						Draw_texture(font_images, texture_tint, s_font_start_num[45] + j, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
						unknown_char = false;
						texture_x_offset += (s_miscellaneous_symbols_and_pictographs_font_interval + interval_offset) * texture_size_x / 2;
						break;
					}
				}
			}
		}

		if (unknown_char && s_use_external_font[0])
		{
			texture_x_offset += (25 + interval_offset) * texture_size_x / 2;
			Draw_texture(font_images, texture_tint, s_font_start_num[0] + 0, texture_x + texture_x_offset, texture_y, 25.0 * texture_size_x, 25.0 * texture_size_y);
			texture_x_offset += (25 + interval_offset) * texture_size_x / 2;
		}
	}
}

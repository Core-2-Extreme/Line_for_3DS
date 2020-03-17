#pragma once
#include "citro2d.h"
#define STACKSIZE (24 * 1024)
#define OUT_OF_MEMORY -1
#define BUFFER_SIZE_IS_TOO_SMALL -2


struct Result_with_string
{
	std::string string;
	std::string error_description;
	Result code;
};

//Svc init result flag
extern bool s_ac_success;
extern bool s_apt_success;
extern bool s_am_success;
extern bool s_mcu_success;
extern bool s_ptmu_success;
extern bool s_fs_success;
extern bool s_httpc_success;
extern bool s_rom_success;
extern bool s_cfg_success;

//Key flag
extern bool s_key_A_press;
extern bool s_key_B_press;
extern bool s_key_X_press;
extern bool s_key_Y_press;
extern bool s_key_CPAD_UP_press;
extern bool s_key_CPAD_DOWN_press;
extern bool s_key_CPAD_RIGHT_press;
extern bool s_key_CPAD_LEFT_press;
extern bool s_key_DUP_press;
extern bool s_key_DDOWN_press;
extern bool s_key_DRIGHT_press;
extern bool s_key_DLEFT_press;
extern bool s_key_L_press;
extern bool s_key_R_press;
extern bool s_key_ZL_press;
extern bool s_key_ZR_press;
extern bool s_key_START_press;
extern bool s_key_SELECT_press;
extern bool s_key_touch_press;
extern bool s_key_A_held;
extern bool s_key_B_held;
extern bool s_key_X_held;
extern bool s_key_Y_held;
extern bool s_key_CPAD_UP_held;
extern bool s_key_CPAD_DOWN_held;
extern bool s_key_CPAD_RIGHT_held;
extern bool s_key_CPAD_LEFT_held;
extern bool s_key_DUP_held;
extern bool s_key_DDOWN_held;
extern bool s_key_DRIGHT_held;
extern bool s_key_DLEFT_held;
extern bool s_key_L_held;
extern bool s_key_R_held;
extern bool s_key_ZL_held;
extern bool s_key_ZR_held;
extern bool s_key_touch_held;

//Already init flag
extern bool s_gtr_already_init;
extern bool s_imv_already_init;
extern bool s_line_already_init;
extern bool s_spt_already_init;
extern bool s_sem_already_init;

//App run flag
extern bool s_gtr_main_run;
extern bool s_imv_main_run;
extern bool s_line_main_run;
extern bool s_menu_main_run;
extern bool s_sem_main_run;
extern bool s_spt_main_run;

//Thread run flag
extern bool s_connect_test_thread_run;
extern bool s_gtr_tr_thread_run;
extern bool s_hid_thread_run;
extern bool s_imv_download_thread_run;
extern bool s_imv_parse_thread_run;
extern bool s_line_log_download_thread_run;
extern bool s_line_log_load_thread_run;
extern bool s_line_log_parse_thread_run;
extern bool s_line_message_send_thread_run;
extern bool s_line_update_thread_run;
extern bool s_spt_thread_run;
extern bool s_update_thread_run;
extern bool s_update_check_thread_run;
extern bool s_launch_app_thread_run;
extern bool s_destroy_app_thread_run;

//Thread suspend flag
extern bool s_gtr_thread_suspend;
extern bool s_imv_thread_suspend;
extern bool s_line_thread_suspend;
extern bool s_spt_thread_suspend;
extern bool s_sem_thread_suspend;

extern bool s_allow_send_app_info;
extern bool s_debug_mode;
extern bool s_connect_test_succes;
extern bool s_debug_slow;
extern bool s_gtr_type_text_request;
extern bool s_use_specific_system_font;
extern bool s_use_external_font[47];
extern bool s_imv_adjust_url_request;
extern bool s_imv_image_dl_request;
extern bool s_imv_image_parse_request;
extern bool s_line_auto_update_mode;
extern bool s_line_hide_id;
extern bool s_line_log_load_request;
extern bool s_line_log_update_request;
extern bool s_line_message_send_check;
extern bool s_line_message_send_request;
extern bool s_line_type_id_request;
extern bool s_line_type_message_request;
extern bool s_line_type_main_url_request;
extern bool s_app_logs_show;
extern bool s_wifi_enabled;
extern bool s_disabled_enter_afk_mode;
extern bool s_night_mode;
extern bool s_draw_vsync_mode;
extern bool s_hid_disabled;
extern bool s_system_setting_menu_show;
extern bool s_flash_mode;
extern bool s_sem_update_check_request;
extern bool s_sem_show_newest_ver_data;
extern bool s_sem_select_ver;
extern bool s_sem_available_ver[8]; 
extern bool s_sem_file_download_request;
extern bool s_spt_start_request;
extern bool s_error_display;

extern float message_select_num;
extern float text_x_cache;
extern float text_y_cache;
extern float text_size_cache;
extern float text_interval_cache;
extern float app_log_x_cache;
extern float s_line_bottom_y;
extern float s_gtr_text_x;

extern u8 s_wifi_signal;
extern u8 s_battery_level;
extern u8 s_battery_charge;
extern u8* wifi_state;
extern u8* wifi_state_internet_sample;

extern int s_afk_time;
extern int s_afk_lcd_brightness;
extern int s_app_log_num;
extern int s_app_log_view_num_cache;
extern int s_circle_pos_x;
extern int s_circle_pos_y;
extern int s_num_of_app_start;
extern int s_fps_show;
extern int s_free_ram;
extern int s_free_linear_ram;
extern int s_gtr_selected_history_num;
extern int s_hours;
extern int s_minutes;
extern int s_seconds;
extern int s_days;
extern int s_months;
extern int s_imv_image_pos_x;
extern int s_imv_image_pos_y;
extern int s_imv_clipboard_select_num;
extern int s_lang_select_num;
extern int s_line_menu_mode;
extern int s_line_room_select_num;
extern int s_held_time;
extern int s_circle_pos_x;
extern int s_circle_pos_y;
extern int s_touch_pos_x;
extern int s_touch_pos_y;
extern int s_touch_pos_x_before;
extern int s_touch_pos_x_moved;
extern int s_touch_pos_y_before;
extern int s_touch_pos_y_moved;
extern int s_sem_help_mode_num;
extern int s_app_log_view_num;
extern int s_fps;
extern int s_lcd_brightness;
extern int s_time_to_enter_afk;
extern int s_current_app_ver;
extern int s_current_gas_ver;
extern int s_sem_selected_edition_num;
extern int s_line_log_httpc_buffer_size;
extern int s_line_log_fs_buffer_size;
extern int s_spt_spt_httpc_buffer_size;
extern int s_imv_image_httpc_buffer_size;
extern int s_spt_data_size;
extern int s_font_characters[46];
extern int s_font_start_num[46];
extern float s_touch_pos_x_move_left;
extern float s_touch_pos_y_move_left;
extern float s_imv_image_zoom;
extern float s_sem_y_offset;
extern float s_frame_time;
extern float s_app_log_x;
extern float s_arabic_font_interval[255];
extern float s_armenian_font_interval[91];
extern float s_box_drawing_font_interval[128];
extern float s_cjk_symbol_and_punctuation_font_interval[64];
extern float s_devanagari_font_interval[128];
extern float s_dingbats_font_interval[192];
extern float s_basic_latin_font_interval[96];
extern float s_geometric_shapes_font_interval[96];
extern float s_telugu_font_interval[96];
extern float s_yi_radicals_font_interval[55];
extern float s_hiragana_font_interval[93];
extern float s_katakana_font_interval[96];
extern float s_halfwidth_and_fullwidth_forms_font_interval;
extern float s_mathematical_operators_font_interval;
extern float s_latin_1_supplement_font_interval;
extern float s_miscellaneous_symbols_font_interval;
extern float s_greek_and_coptic_font_interval;
extern float s_cyrillic_font_interval;
extern float s_ipa_extensions_font_interval;
extern float s_gurmukhi_font_interval;
extern float s_general_punctuation_font_interval;
extern float s_block_elements_font_interval;
extern float s_phonetic_extensions_font_interval;
extern float s_thai_font_interval;
extern float s_georgian_font_interval;
extern float s_hangul_compatibility_jamo_font_interval;
extern float s_miscellaneous_technical_font_interval;
extern float s_tamil_font_interval;
extern float s_spacing_modifier_letters_font_interval;
extern float s_tibetan_font_interval;
extern float s_arrows_font_interval;
extern float s_cyrillic_supplement_font_interval;
extern float s_superscripts_and_subscripts_font_interval;
extern float s_cjk_compatibility_forms_font_interval;
extern float s_combining_diacritical_marks_font_interval;
extern float s_optical_character_recognition_font_interval;
extern float s_greek_extended_font_interval;
extern float s_miscellaneous_symbols_and_pictographs_font_interval;
extern float s_unified_canadian_aboriginal_syllabics_font_interval;
extern float s_hebrew_font_interval;
extern float s_lao_font_interval;
extern float s_kannada_font_interval;
extern float s_combining_diacritical_marks_for_symbols_font_interval;
extern float s_combining_diacritical_marks_supplement_font_interval;
extern float s_sinhala_font_interval;
extern float s_yi_syllables_font_interval;
extern float s_font_pos;
extern double s_scroll_speed;
extern double s_app_up_time_ms;

extern char s_status[100];
extern char s_swkb_input_text[8192];
extern std::string s_app_logs[4096];
extern std::string s_clipboards[15];
extern std::string s_bot_button_string[2];
extern std::string s_square_string;
extern std::string s_circle_string;
extern std::string s_battery_level_string;
extern std::string s_error_summary;
extern std::string s_error_description;
extern std::string s_error_place;
extern std::string s_error_code;
extern std::string s_gtr_history[17];;
extern std::string s_setting[20];
extern std::string s_line_message_en[21];
extern std::string s_line_message_jp[21];
extern std::string s_line_message_log[300];
extern std::string s_spt_message_en[12];
extern std::string s_spt_message_jp[12];
extern std::string s_imv_message_en[8];
extern std::string s_imv_message_jp[8];
extern std::string s_spt_ver;
extern std::string s_gtr_ver;
extern std::string s_imv_ver;
extern std::string s_line_ver;
extern std::string s_app_ver;
extern std::string s_httpc_user_agent;


extern std::string s_arabic_sample[255];
extern std::string s_arabic_right_to_left_sample[235];
extern std::string s_armenian_sample[91];
extern std::string s_box_drawing_font_sample[128];
extern std::string s_cjk_symbol_and_punctuation_font_sample[64];
extern std::string s_devanagari_font_sample[128];
extern std::string s_dingbats_font_sample[192];
extern std::string s_basic_latin_font_sample[128];
extern std::string s_geometric_shapes_font_sample[96];
extern std::string s_telugu_font_sample[96];
extern std::string s_yi_radicals_font_sample[55];
extern std::string s_hiragana_font_sample[93];
extern std::string s_katakana_font_sample[96];
extern std::string s_halfwidth_and_fullwidth_forms_font_sample[225];
extern std::string s_mathematical_operators_font_sample[256];
extern std::string s_latin_1_supplement_font_sample[128];
extern std::string s_miscellaneous_symbols_font_sample[256];
extern std::string s_greek_and_coptic_font_sample[135];
extern std::string s_cyrillic_font_sample[256];
extern std::string s_ipa_extensions_font_sample[96];
extern std::string s_gurmukhi_font_sample[79];
extern std::string s_general_punctuation_font_sample[111];
extern std::string s_block_elements_font_sample[32];
extern std::string s_phonetic_extensions_font_sample[128];
extern std::string s_thai_font_sample[87];
extern std::string s_georgian_font_sample[88];
extern std::string s_hangul_compatibility_jamo_font_sample[94];
extern std::string s_miscellaneous_technical_font_sample[256];
extern std::string s_tamil_font_sample[72];
extern std::string s_spacing_modifier_letters_font_sample[80];
extern std::string s_tibetan_font_sample[211];
extern std::string s_arrows_font_sample[112];
extern std::string s_cyrillic_supplement_font_sample[48];
extern std::string s_superscripts_and_subscripts_font_sample[42];
extern std::string s_cjk_compatibility_forms_font_sample[42];
extern std::string s_combining_diacritical_marks_font_sample[112];
extern std::string s_optical_character_recognition_font_sample[11];
extern std::string s_greek_extended_font_sample[233];
extern std::string s_miscellaneous_symbols_and_pictographs_font_sample[768];
extern std::string s_unified_canadian_aboriginal_syllabics_font_sample[640];
extern std::string s_hebrew_font_sample[88];
extern std::string s_lao_font_sample[67];
extern std::string s_kannada_font_sample[89];
extern std::string s_combining_diacritical_marks_for_symbols_font_sample[33];
extern std::string s_combining_diacritical_marks_supplement_font_sample[63];
extern std::string s_sinhala_font_sample[90];
extern std::string s_yi_syllables_font_sample[1165];
extern std::string s_font_samples[62];
extern std::string s_font_file_name[46];

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

extern C2D_SpriteSheet Background_texture;
extern C2D_SpriteSheet Wifi_icon_texture;
extern C2D_SpriteSheet Battery_level_icon_texture;
extern C2D_SpriteSheet Battery_charge_icon_texture;
extern C2D_SpriteSheet Square_texture;

extern C2D_Image Background_image[2];
extern C2D_Image Wifi_icon_image[9];
extern C2D_Image Battery_level_icon_image[21];
extern C2D_Image Battery_charge_icon_image[1];
extern C2D_Image Square_image[14];
extern C2D_Image sem_help_image[7];
extern C2D_Image font_images[7569];

extern C2D_ImageTint texture_tint, dammy_tint;

extern TickCounter s_tcount_up_time, s_tcount_frame_time;
extern Thread s_connect_test_thread, s_hid_thread, s_update_thread, s_send_app_info_thread;
extern SwkbdState s_swkb;
extern SwkbdLearningData s_swkb_learn_data;
extern SwkbdDictWord s_swkb_words[8];
extern SwkbdButton s_swkb_press_button;

void Share_send_app_info_thread(void* arg);

void Share_connectivity_check_thread(void* arg);

void Share_get_system_info(void);

int Share_check_free_ram(void);

bool Share_exit_check(void);

void Share_update_thread(void* arg);

void Share_app_log_draw(void);

int Share_app_log_save(std::string type, std::string text, Result result, bool draw);

void Share_app_log_add_result(int add_log_num, std::string add_text, Result result, bool draw);

void Share_key_flag_reset(void);

std::string Share_dec_to_hex_string(long dec);

void Share_clear_error_message(void);

void Share_set_error_message(std::string summary, std::string description, std::string place, long error_code);

void Share_draw_error(void);

void Share_scan_hid_thread(void* arg);

std::string Share_text_sort(std::string sorce_part_string[]);

void Share_text_parse(std::string sorce_string, std::string part_string[]);

void Share_draw_external_fonts(std::string string, float texture_x, float texture_y, float texture_size_x, float texture_size_y, bool follow_new_line);

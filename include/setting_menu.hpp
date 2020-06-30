#pragma once
#include "types.hpp"

#define SEM_NUM_OF_MSG 73

#define SEM_CHECK_UPDATE_REQUEST 0
#define SEM_SHOW_PATCH_NOTE_REQUEST 1
#define SEM_SELECT_VER_REQUEST 2
#define SEM_DL_FILE_REQUEST 3
#define SEM_LOAD_SYSTEM_FONT_REQUEST 4
#define SEM_LOAD_EXTERNAL_FONT_REQUEST 5
#define SEM_UNLOAD_EXTERNAL_FONT_REQUEST 6
#define SEM_RELOAD_MSG_REQUEST 7
#define SEM_CHANGE_WIFI_STATE_REQUEST 8
#define SEM_DELETE_LINE_IMG_CACHE_REQUEST 9

#define SEM_USE_DEFAULT_FONT 0
#define SEM_USE_SYSTEM_SPEIFIC_FONT 1
#define SEM_USE_EXTERNAL_FONT 2

#define SEM_SELECTED_LANG_NUM 0
#define SEM_SELECTED_EDITION_NUM 1
#define SEM_SELECTED_MENU_MODE_NUM 2

#define SEM_NIGHT_MODE 0
#define SEM_VSYNC_MODE 1
#define SEM_FLASH_MODE 2
#define SEM_DEBUG_MODE 3
#define SEM_ALLOW_SEND_APP_INFO 4
#define SEM_WIFI_ENABLED 5
#define SEM_SYSTEM_SETTING_MENU_SHOW 6

#define SEM_LCD_BRIGHTNESS 0
#define SEM_TIME_TO_TURN_OFF_LCD 1
#define SEM_LCD_BRIGHTNESS_BEFORE_TURN_OFF 2
#define SEM_NUM_OF_APP_START 3

#define SEM_SCROLL_SPEED 0

Result_with_string Sem_load_setting(std::string file_name, std::string dir_name, int item_num, std::string out_data[]);

Result_with_string Sem_parse_file(std::string source_data, int num_of_items, std::string out_data[]);

bool Sem_query_init_flag(void);

bool Sem_query_running_flag(void);

bool Sem_query_available_edtion(int edtion_num);

int Sem_query_app_ver(void);

int Sem_query_gas_ver(void);

bool Sem_query_font_flag(int font_num);

bool Sem_query_loaded_external_font_flag(int external_font_num);

bool Sem_query_operation_flag(int operation_num);

int Sem_query_selected_num(int item_num);

bool Sem_query_settings(int item_num);

int Sem_query_settings_i(int item_num);

double Sem_query_settings_d(int item_num);

double Sem_query_y_max(void);

double Sem_query_y_offset(void);

void Sem_set_font_flag(int font_num, bool flag);

void Sem_set_msg(int msg_num, std::string msg);

void Sem_set_load_external_font_request(int external_font_num, bool flag);

void Sem_set_operation_flag(int operation_num, bool flag);

void Sem_set_selected_num(int item_num, int num);

void Sem_set_settings(int item_num, bool flag);

void Sem_set_settings_i(int item_num, int value);

void Sem_set_settings_d(int item_num, double value);

void Sem_set_y_max(double y_max);

void Sem_set_y_offset(double y);

void Sem_set_color(double in_red, double in_green, double in_blue, double in_alpha, double* out_red, double* out_green, double* out_blue, double* out_alpha, int num_of_out);

void Sem_suspend(void);

void Sem_resume(void);

void Sem_init(void);

void Sem_exit(void);

void Sem_main(void);

void Sem_worker_thread(void* arg);

void Sem_check_update_thread(void* arg);

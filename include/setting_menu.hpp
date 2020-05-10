#pragma once

#define SEM_CHECK_UPDATE_REQUEST 0
#define SEM_SHOW_PATCH_NOTE_REQUEST 1
#define SEM_SELECT_VER_REQUEST 2
#define SEM_DL_FILE_REQUEST 3
#define SEM_LOAD_SYSTEM_FONT_REQUEST 4
#define SEM_LOAD_EXTERNAL_FONT_REQUEST 5
#define SEM_UNLOAD_EXTERNAL_FONT_REQUEST 6

#define SEM_USE_DEFAULT_FONT 0
#define SEM_USE_SYSTEM_SPEIFIC_FONT 1
#define SEM_USE_EXTERNAL_FONT 2

#define SEM_SELECTED_LANG_NUM 0
#define SEM_SELECTED_EDITION_NUM 1

#define SEM_NIGHT_MODE 0
#define SEM_VSYNC_MODE 1
#define SEM_FLASH_MODE 2
#define SEM_DEBUG_MODE 3
#define SEM_ALLOW_SEND_APP_INFO 4

bool Sem_query_init_flag(void);

bool Sem_query_running_flag(void);

bool Sem_query_available_edtion(int edtion_num);

bool Sem_query_font_flag(int font_num);

bool Sem_query_loaded_external_font_flag(int external_font_num);

bool Sem_query_operation_flag(int operation_num);

int Sem_query_selected_num(int item_num);

bool Sem_query_settings(int item_num);

double Sem_query_y_offset(void);

void Sem_set_font_flag(int font_num, bool flag);

void Sem_set_load_external_font_request(int external_font_num, bool flag);

void Sem_set_operation_flag(int operation_num, bool flag);

void Sem_set_selected_num(int item_num, int num);

void Sem_set_settings(int item_num, bool flag);

void Sem_set_y_offset(double y);

void Sem_suspend(void);

void Sem_resume(void);

void Sem_init(void);

void Sem_exit(void);

void Sem_main(void);

void Sem_load_font_thread(void* arg);

void Sem_check_update_thread(void* arg);


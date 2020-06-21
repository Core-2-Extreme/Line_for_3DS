#pragma once

#define MENU_JUMP_TO_LINE_REQUEST 0
#define MENU_JUMP_TO_GTR_REQUEST 1
#define MENU_JUMP_TO_SPT_REQUEST 2
#define MENU_JUMP_TO_IMV_REQUEST 3
#define MENU_JUMP_TO_CAM_REQUEST 4
#define MENU_JUMP_TO_MIC_REQUEST 5
#define MENU_JUMP_TO_MUP_REQUEST 6
#define MENU_JUMP_TO_SEM_REQUEST 7
#define MENU_DESTROY_LINE_REQUEST 8
#define MENU_DESTROY_GTR_REQUEST 9
#define MENU_DESTROY_SPT_REQUEST 10
#define MENU_DESTROY_IMV_REQUEST 11
#define MENU_DESTROY_CAM_REQUEST 12
#define MENU_DESTROY_MIC_REQUEST 13
#define MENU_DESTROY_MUP_REQUEST 14
#define MENU_DESTROY_SEM_REQUEST 15
#define MENU_CHANGE_BRIGHTNESS_REQUEST 16
#define MENU_CHECK_EXIT_REQUEST 17

bool Menu_query_running_flag(void);

bool Menu_query_must_exit_flag(void);

std::string Menu_query_time(void);

void Menu_set_operation_flag(int operation_num, bool flag);

void Menu_resume(void);

void Menu_suspend(void);

void Menu_init(void);

void Menu_exit(void);

void Menu_main(void);

bool Menu_check_exit(void);

void Menu_get_system_info(void);

int Menu_check_free_ram(void);

void Menu_send_app_info_thread(void* arg);

void Menu_check_connectivity_thread(void* arg);

void Menu_update_thread(void* arg);

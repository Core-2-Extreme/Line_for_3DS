﻿#pragma once

#define MENU_CHANGE_BRIGHTNESS_REQUEST 0

#define MENU_HTTP_PORT0 0
#define MENU_HTTP_POST_PORT0 1

bool Menu_query_running_flag(void);

int Menu_query_afk_time(void);

bool Menu_query_battery_charge(void);

std::string Menu_query_battery_level(void);

int Menu_query_battery_level_raw(void);

std::string Menu_query_clipboard(void);

int Menu_query_free_ram(void);

int Menu_query_free_linear_ram(void);

bool Menu_query_must_exit_flag(void);

void Menu_set_must_exit_flag(bool flag);

std::string Menu_query_status(bool only_system_state);

std::string Menu_query_time(int mode);

std::string Menu_query_ver(void);

int Menu_query_wifi_state(void);

void Menu_reset_afk_time(void);

void Menu_set_clipboard(std::string data);

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

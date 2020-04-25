#pragma once

#include "share_function.hpp"

#define LINE_HTTPC_BUFFER 0
#define LINE_FS_BUFFER 1
#define LINE_SEND_FS_BUFFER 2
#define LINE_SEND_FS_CACHE_BUFFER 3

#define LINE_SEND_MSG_CHECK_REQUEST 0
#define LINE_SEND_STICKER_CHECK_REQUEST 1
#define LINE_SEND_CONTENT_CHECK_REQUEST 2
#define LINE_SEND_MSG_REQUEST 3
#define LINE_SEND_STICKER_REQUEST 4
#define LINE_SEND_CONTENT_REQUEST 5
#define LINE_DL_LOG_REQUEST 6
#define LINE_LOAD_LOG_REQUEST 7
#define LINE_TYPE_MSG_REQUEST 8
#define LINE_TYPE_ID_REQUEST 9
#define LINE_TYPE_MAIN_URL_REQUEST 10
#define LINE_TYPE_APP_PS_REQUEST 11
#define LINE_TYPE_SCRIPT_PS_REQUEST 12
#define LINE_SELECT_STICKER_REQUEST 13
#define LINE_SELECT_FILE_REQUEST 14
#define LINE_SENDING_MSG 15
#define LINE_SEND_SUCCESS 16

#define LINE_MENU_SEND 0 
#define LINE_MENU_RECEIVE 1
#define LINE_MENU_COPY 2
#define LINE_MENU_SETTINGS 3
#define LINE_MENU_ADVANCED_SETTINGS 4

#define LINE_HIDE_ID 0
#define LINE_AUTO_UPDATE 1

#define LINE_SELECTED_MENU_MODE_NUM 0
#define LINE_SELECTED_STICKER_NUM 1
#define LINE_SELECTED_STICKER_TAB_NUM 2
#define LINE_SELECTED_ROOM_NUM 3

#define LINE_SELECTED_MSG_NUM_D 0

#define LINE_TEXT_X 0
#define LINE_TEXT_Y 1
#define LINE_TEXT_SIZE 2
#define LINE_TEXT_INTERVAL 3

bool Line_query_init_flag(void);

bool Line_query_running_flag(void);

int Line_query_buffer_size(int buffer_num);

std::string Line_query_content_info(int log_num);

double Line_query_max_y(void);

std::string Line_query_msg_log(int log_num);

bool Line_query_operation_flag(int operation_num);

double Line_query_selected_num_d(int item_num);

int Line_query_selected_num(int item_num);

double Line_query_x_y_size_interval(int item_num);

bool Line_query_setting(int setting_num);

void Line_set_buffer_size(int buffer_num, int size);

void Line_set_send_file_name(std::string file_name);

void Line_set_send_dir_name(std::string dir_name);

void Line_set_operation_flag(int operation_num, bool flag);

void Line_set_selected_num_d(int item_num, double value);

void Line_set_selected_num(int item_num, int value);

void Line_set_x_y_size_interval(int item_num, double value);

void Line_set_setting(int setting_num, bool flag);

void Line_resume(void);

void Line_suspend(void);

void Line_init(void);

void Line_main(void);

void Line_log_download_thread(void* arg);

void Line_update_thread(void* arg);

void Line_send_message_thread(void* arg);

void Line_pic_parse_thread(void* arg);

Result_with_string Line_save_new_id(std::string id, std::string dir_path);

Result_with_string Line_read_id(std::string dir_path);

Result_with_string Line_load_log_from_sd(std::string file_name);

int Line_stickers_num_to_textures_num(int sticker_num);

void Line_log_parse_thread(void* arg);

void Line_log_load_thread(void* arg);

void Line_exit(void);

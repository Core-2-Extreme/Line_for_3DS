#pragma once

#include "types.hpp"

#define LINE_NUM_OF_MSG 55

#define LINE_HTTPC_BUFFER 0
#define LINE_FS_BUFFER 1
#define LINE_SEND_BUFFER 2

#define LINE_MENU_SEND 0
#define LINE_MENU_RECEIVE 1
#define LINE_MENU_COPY 2
#define LINE_MENU_SEARCH 3
#define LINE_MENU_SETTINGS 4
#define LINE_MENU_ADVANCED 5

#define LINE_HTTP_PORT0 2
#define LINE_HTTP_PORT1 3
#define LINE_HTTP_PORT2 4
#define LINE_HTTP_POST_PORT0 1

void Line_main(void);

void Line_init(void);

void Line_exit(void);

bool Line_query_init_flag(void);

bool Line_query_running_flag(void);

int Line_query_buffer_size(int buffer_num);

void Line_reset_id(void);

void Line_reset_msg(void);

void Line_set_buffer_size(int buffer_num, int size);

void Line_set_msg(int msg_num, std::string msg);

void Line_set_send_file(std::string file_name, std::string dir_name);

void Line_cancel_select_file(void);

void Line_resume(void);

void Line_suspend(void);

void Line_worker_thread(void* arg);

void Line_log_download_thread(void* arg);

void Line_update_thread(void* arg);

void Line_send_message_thread(void* arg);

void Line_pic_parse_thread(void* arg);

Result_with_string Line_save_new_id(std::string id, std::string dir_path);

Result_with_string Line_load_icon(int room_num);

Result_with_string Line_read_id(std::string dir_path);

Result_with_string Line_load_log_from_sd(std::string file_name);

int Line_stickers_num_to_textures_num(int sticker_num);

void Line_log_parse_thread(void* arg);

void Line_log_load_thread(void* arg);

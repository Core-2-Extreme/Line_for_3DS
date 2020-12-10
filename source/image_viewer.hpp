#pragma once

#include "citro2d.h"

#include "types.hpp"

#define IMV_NUM_OF_MSG 7

#define IMV_HTTPC_BUFFER 0
#define IMV_FS_BUFFER 1

#define IMV_HTTP_PORT0 6

bool Imv_query_init_flag(void);

bool Imv_query_running_flag(void);

int Imv_query_max_buffer_size(void);

void Imv_set_max_buffer_size(int size);

void Imv_set_msg(int msg_num, std::string msg);

void Imv_set_url(std::string url);

void Imv_set_load_file(std::string file_name, std::string dir_name);

void Imv_cancel_select_file(void);

void Imv_suspend(void);

void Imv_resume(void);

void Imv_init(void);

void Imv_main(void);

void Imv_img_parse_thread(void* arg);

void Imv_img_load_thread(void* arg);

void Imv_img_dl_thread(void* arg);

void Imv_exit(void);

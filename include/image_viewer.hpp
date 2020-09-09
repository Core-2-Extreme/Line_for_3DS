#pragma once

#include "citro2d.h"

#include "types.hpp"

#define IMV_NUM_OF_MSG 7

#define IMV_HTTPC_BUFFER 0
#define IMV_FS_BUFFER 1

#define IMV_IMG_LOAD_REQUEST 0
#define IMV_IMG_LOAD_AND_PARSE_REQUEST 1
#define IMV_IMG_DL_REQUEST 2
#define IMV_IMG_DL_AND_PARSE_REQUEST 3
#define IMV_IMG_PARSE_REQUEST 4
#define IMV_ADJUST_URL_REQUEST 5
#define IMV_SELECT_FILE_REQUEST 6

#define IMV_HTTP_PORT0 6

bool Imv_query_init_flag(void);

bool Imv_query_running_flag(void);

//0 = httpc buffer size, 1 = fs buffer size
int Imv_query_buffer_size(int buffer_num);

bool Imv_query_operation_flag(int operation_num);

double Imv_query_img_pos_x(void);

double Imv_query_img_pos_y(void);

double Imv_query_img_zoom(void);

//0 = httpc buffer size, 1 = fs buffer size
void Imv_set_buffer_size(int buffer_num, int size);

void Imv_set_msg(int msg_num, std::string msg);

void Imv_set_operation_flag(int operation_num, bool flag);

void Imv_set_url(std::string url);

void Imv_set_img_pos_x(double x);

void Imv_set_img_pos_y(double y);

void Imv_set_img_zoom(double size);

void Imv_set_load_file_name(std::string file_name);

void Imv_set_load_dir_name(std::string dir_name);

void Imv_suspend(void);

void Imv_resume(void);

void Imv_init(void);

void Imv_main(void);

void Imv_img_parse_thread(void* arg);

void Imv_img_load_thread(void* arg);

void Imv_img_dl_thread(void* arg);

void Imv_exit(void);

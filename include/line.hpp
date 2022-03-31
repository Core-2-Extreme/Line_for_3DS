#pragma once
#include "system/types.hpp"

bool Line_query_init_flag(void);

bool Line_query_running_flag(void);

void Line_hid(Hid_info key);

void Line_resume(void);

void Line_suspend(void);

Result_with_string Line_load_msg(std::string lang);

void Line_init(bool draw);

void Line_exit(bool draw);

void Line_main(void);









/*void Line_reset_id(void);

void Line_reset_msg(void);

void Line_set_buffer_size(int buffer_num, int size);

Result_with_string Line_save_new_id(std::string id, std::string dir_path);

Result_with_string Line_load_icon(int room_num);

Result_with_string Line_read_id(std::string dir_path);

Result_with_string Line_load_log_from_sd(std::string file_name);

int Line_stickers_num_to_textures_num(int sticker_num);*/

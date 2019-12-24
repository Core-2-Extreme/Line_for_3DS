#pragma once

#include "share_function.hpp"

void Line_init(void);

void Line_main(void);

void Line_log_download(void* arg);

void Line_update_thread(void* arg);

void Line_send_message(void* arg);

Result_with_string Line_save_new_id(std::string id, std::string dir_path);

Result_with_string Line_read_id(std::string dir_path);

int Line_load_from_log_sd(std::string file_name);

void Line_log_parse_thread(void* arg);

void Line_exit(void);

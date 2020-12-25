#pragma once

#define VID_NUM_OF_MSG 12

#define VID_HTTP_PORT0 7

bool Vid_query_init_flag(void);

bool Vid_query_running_flag(void);

void Vid_set_msg(int msg_num, std::string msg);

void Vid_resume(void);

void Vid_suspend(void);

void Vid_set_url(std::string url);

void Vid_set_load_file(std::string file_name, std::string dir_name);

void Vid_reset_fps(void);

void Vid_cancel_select_file(void);

void Vid_exit(void);

Result_with_string Vid_load_msg(std::string lang);

void Vid_init(void);

void Vid_main(void);

#pragma once
#include "system/types.hpp"

bool Vid_query_init_flag(void);

bool Vid_query_running_flag(void);

void Vid_set_load_file(std::string file, std::string dir);

void Vid_set_url(std::string url);

void Vid_hid(Hid_info key);

void Vid_resume(void);

void Vid_suspend(void);

Result_with_string Vid_load_msg(std::string lang);

void Vid_init(bool draw);

void Vid_exit(bool draw);

void Vid_main(void);

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

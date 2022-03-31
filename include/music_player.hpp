#pragma once
#include "system/types.hpp"

bool Mup_query_init_flag(void);

bool Mup_query_running_flag(void);

void Mup_hid(Hid_info key);

void Mup_resume(void);

void Mup_suspend(void);

void Mup_set_url(std::string url);

void Mup_set_load_file(std::string file_name, std::string dir_name);

Result_with_string Mup_load_msg(std::string lang);

void Mup_init(bool draw);

void Mup_exit(bool draw);

void Mup_main(void);

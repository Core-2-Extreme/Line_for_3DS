#pragma once
#include "system/types.hpp"

bool Spt_query_init_flag(void);

bool Spt_query_running_flag(void);

void Spt_hid(Hid_info key);

void Spt_resume(void);

void Spt_suspend(void);

Result_with_string Spt_load_msg(std::string lang);

void Spt_init(bool draw);

void Spt_exit(bool draw);

void Spt_main(void);

#pragma once
#include "system/types.hpp"

bool Gtr_query_init_flag(void);

bool Gtr_query_running_flag(void);

void Gtr_hid(Hid_info key);

void Gtr_resume(void);

void Gtr_suspend(void);

Result_with_string Gtr_load_msg(std::string lang);

void Gtr_init(bool draw);

void Gtr_exit(bool draw);

void Gtr_main(void);

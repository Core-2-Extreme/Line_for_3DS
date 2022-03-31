#pragma once
#include "system/types.hpp"

bool Mic_query_init_flag(void);

bool Mic_query_running_flag(void);

void Mic_hid(Hid_info key);

void Mic_resume(void);

void Mic_suspend(void);

Result_with_string Mic_load_msg(std::string lang);

void Mic_init(bool draw);

void Mic_exit(bool draw);

void Mic_main(void);

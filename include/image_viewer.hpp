#pragma once
#include "system/types.hpp"

bool Imv_query_init_flag(void);

bool Imv_query_running_flag(void);

void Imv_set_url(std::string url);

void Imv_set_load_file(std::string file_name, std::string dir_name);

void Imv_hid(Hid_info key);

void Imv_resume(void);

void Imv_suspend(void);

Result_with_string Imv_load_msg(std::string lang);

void Imv_init(bool draw);

void Imv_exit(bool draw);

void Imv_main(void);

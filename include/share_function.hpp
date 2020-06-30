#pragma once
#include "citro2d.h"

extern bool s_mcu_success;

extern bool s_debug_slow;
extern bool s_disabled_enter_afk_mode;

extern u8 s_wifi_signal;
extern u8 s_battery_level;
extern u8 s_battery_charge;
extern u8* wifi_state;
extern u8* wifi_state_internet_sample;

extern char s_status[100];
extern std::string s_clipboards[15];
extern std::string s_battery_level_string;
extern std::string s_setting[32];

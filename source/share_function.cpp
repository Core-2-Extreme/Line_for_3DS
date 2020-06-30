#include <3ds.h>
#include <string>
#include "share_function.hpp"

bool s_mcu_success = false;

bool s_debug_slow = false;
bool s_disabled_enter_afk_mode = false;

u8 s_wifi_signal = -1;
u8 s_battery_level = -1;
u8 s_battery_charge = -1;
u8* wifi_state;
u8* wifi_state_internet_sample;

char s_status[100];
std::string s_clipboards[15];
std::string s_battery_level_string = "?";
std::string s_setting[32];

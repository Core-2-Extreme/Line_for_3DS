#include <3ds.h>
#include <unistd.h>
#include <string>
#include "share_function.hpp"

bool s_mcu_success = false;

bool s_debug_slow = false;
bool s_disabled_enter_afk_mode = false;
bool s_system_setting_menu_show = false;

u8 s_wifi_signal = -1;
u8 s_battery_level = -1;
u8 s_battery_charge = -1;
u8* wifi_state;
u8* wifi_state_internet_sample;

int s_afk_time = 0;
int s_afk_lcd_brightness = 0;
int s_num_of_app_start = 0;
int s_fps_show = 0;
int s_free_ram = 0;
int s_free_linear_ram = 0;
int s_fps;
int s_lcd_brightness = 50;
int s_time_to_enter_afk = 300;
int s_current_app_ver = 14;
int s_current_gas_ver = 5;
double s_frame_time;
double s_scroll_speed = 0.5;

char s_status[100];
char s_swkb_input_text[8192];
std::string s_clipboards[15];
std::string s_bot_button_string[2] = { "<                \u25BD                >", "                  \u25BD                 " };
std::string s_circle_string = "\u25CF";
std::string s_battery_level_string = "?";
std::string s_setting[32];
std::string s_success = "[Success] ";
std::string s_error = "[Error] ";
std::string s_spt_ver = "v1.0.4";
std::string s_gtr_ver = "v1.1.1";
std::string s_imv_ver = "v1.0.3";
std::string s_line_ver = "v1.5.0";
std::string s_cam_ver = "v1.0.0";
std::string s_mic_ver = "v1.0.0";
std::string s_mup_ver = "v1.0.0";
std::string s_app_ver = "v1.5.0";
std::string s_httpc_user_agent = "Line for 3DS " + s_app_ver;

C2D_Image Wifi_icon_image[9]; 
C2D_Image Battery_level_icon_image[21]; 
C2D_Image Battery_charge_icon_image[1]; 
C2D_Image Square_image[1];

C2D_ImageTint texture_tint, dammy_tint, black_or_white_tint, white_or_black_tint, white_tint, weak_white_tint, red_tint, weak_red_tint, aqua_tint, weak_aqua_tint, yellow_tint, weak_yellow_tint, blue_tint, weak_blue_tint, black_tint, weak_black_tint;

TickCounter s_tcount_frame_time;
SwkbdState s_swkb;
SwkbdLearningData s_swkb_learn_data;
SwkbdDictWord s_swkb_words[8];
SwkbdButton press_button, s_swkb_press_button;


#include <3ds.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include "hid.hpp"
#include "change_setting.hpp"
#include "share_function.hpp"
#include "file.hpp"
#include "draw.hpp"
#include "httpc.hpp"
#include "line.hpp"
#include "image_viewer.hpp"
#include "setting_menu.hpp"

bool s_ac_success = false;
bool s_apt_success = false;
bool s_am_success = false;
bool s_mcu_success = false;
bool s_ptmu_success = false;
bool s_fs_success = false;
bool s_httpc_success = false;
bool s_rom_success = false;
bool s_cfg_success = false;

bool s_allow_send_app_info = false;
bool s_debug_mode = false;
bool s_connect_test_succes = false;
bool s_debug_slow = false;
bool s_app_logs_show = false;
bool s_wifi_enabled = false;
bool s_disabled_enter_afk_mode = false;
bool s_night_mode = false;
bool s_draw_vsync_mode = true;
bool s_system_setting_menu_show = false;
bool s_flash_mode = false;

float app_log_x_cache = 0.0;

u8 s_wifi_signal = -1;
u8 s_battery_level = -1;
u8 s_battery_charge = -1;
u8* wifi_state;
u8* wifi_state_internet_sample;

int s_afk_time = 0;
int s_afk_lcd_brightness = 0;
int log_app_log_num = 0;
int s_app_log_view_num_cache = 0;
int s_num_of_app_start = 0;
int s_fps_show = 0;
int s_free_ram = 0;
int s_free_linear_ram = 0;
int s_hours = -1;
int s_minutes = -1;
int s_seconds = -1;
int s_days = -1;
int s_months = -1;
int s_held_time = 0;
int s_app_log_view_num = 0;
int s_fps;
int s_lcd_brightness = 50;
int s_time_to_enter_afk = 300;
int s_current_app_ver = 11;
int s_current_gas_ver = 4;
double s_frame_time;
double s_app_log_x = 0.0;
double s_scroll_speed = 0.5;
double s_app_up_time_ms = 0.0;
double s_log_time[512];

char s_status[100];
char s_swkb_input_text[8192];
std::string s_clipboards[15];
std::string s_app_logs[512];
std::string s_bot_button_string[2] = { "<                \u25BD                >", "                  \u25BD                 " };
std::string s_circle_string = "\u25CF";
std::string s_battery_level_string = "?";
std::string s_setting[32];
std::string s_spt_ver = "v1.0.3";
std::string s_gtr_ver = "v1.0.2";
std::string s_imv_ver = "v1.0.3";
std::string s_line_ver = "v1.4.0";
std::string s_app_ver = "v1.4.0";
std::string s_httpc_user_agent = "Line for 3DS " + s_app_ver;

C2D_Image Wifi_icon_image[9]; 
C2D_Image Battery_level_icon_image[21]; 
C2D_Image Battery_charge_icon_image[1]; 
C2D_Image Square_image[1];

C2D_ImageTint texture_tint, dammy_tint, white_or_black_tint, white_tint, weak_white_tint, red_tint, weak_red_tint, aqua_tint, weak_aqua_tint, yellow_tint, weak_yellow_tint, blue_tint, weak_blue_tint, black_tint, weak_black_tint;

TickCounter s_tcount_up_time, s_tcount_frame_time;
SwkbdState s_swkb;
SwkbdLearningData s_swkb_learn_data;
SwkbdDictWord s_swkb_words[8];
SwkbdButton press_button, s_swkb_press_button;

int S_log_save(std::string type, std::string text, Result result, bool draw)
{
	double time_cache;
	char app_log_cache[4096];
	osTickCounterUpdate(&s_tcount_up_time);
	time_cache = osTickCounterRead(&s_tcount_up_time);
	s_app_up_time_ms = s_app_up_time_ms + time_cache;
	s_log_time[log_app_log_num] = s_app_up_time_ms;
	time_cache = s_app_up_time_ms / 1000;
	memset(app_log_cache, 0x0, 4096);

	if (result == 1234567890)
		sprintf(app_log_cache, "[%.5f][%s] %s", time_cache, type.c_str(), text.c_str());
	else
		sprintf(app_log_cache, "[%.5f][%s] %s0x%lx", time_cache, type.c_str(), text.c_str(), result);

	s_app_logs[log_app_log_num] = app_log_cache;
	log_app_log_num++;
	if (log_app_log_num >= 512)
		log_app_log_num = 0;

	if (log_app_log_num < 23)
		s_app_log_view_num = 0;
	else
		s_app_log_view_num_cache = log_app_log_num - 23;

	if (draw)
		Draw_log();

	return (log_app_log_num - 1);
}

void S_log_add(int add_log_num, std::string add_text, Result result, bool draw)
{
	double time_cache;
	char app_log_add_cache[4096];
	osTickCounterUpdate(&s_tcount_up_time);
	time_cache = osTickCounterRead(&s_tcount_up_time);
	s_app_up_time_ms = s_app_up_time_ms + time_cache;
	time_cache = s_app_up_time_ms - s_log_time[add_log_num];
	memset(app_log_add_cache, 0x0, 4096);

	if (result != 1234567890)
		sprintf(app_log_add_cache, "%s0x%lx (%.2fms)", add_text.c_str(), result, time_cache);
	else
		sprintf(app_log_add_cache, "%s (%.2fms)", add_text.c_str(), time_cache);

	if (draw)
		Draw_log();

	s_app_logs[add_log_num] += app_log_add_cache;
}

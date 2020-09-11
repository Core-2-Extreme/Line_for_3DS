#include <3ds.h>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "speedtest.hpp"
#include "image_viewer.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "setting_menu.hpp"
#include "httpc.hpp"
#include "line.hpp"
#include "external_font.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "types.hpp"
#include "music_player.hpp"
#include "camera.hpp"
#include "google_translation.hpp"
#include "change_setting.hpp"
#include "mic.hpp"

/*For draw*/
bool sem_need_reflesh = false;
bool sem_pre_use_default_font = true;
bool sem_pre_use_system_specific_font = false;
bool sem_pre_use_external_font = false;
bool sem_pre_loaded_external_font[46];
bool sem_pre_load_external_font_request = false;
bool sem_pre_unload_external_font_request = false;
bool sem_pre_delete_line_img_cache_request = false;
bool sem_pre_show_patch_note_request = false;
bool sem_pre_select_ver_request = false;
bool sem_pre_allow_send_app_info = false;
bool sem_pre_debug_mode = false;
bool sem_pre_night_mode = false;
bool sem_pre_vsync_mode = true;
bool sem_pre_eco_mode = true;
int sem_pre_lcd_brightness = 100;
int sem_pre_time_to_turn_off_lcd = 1500;
int sem_pre_lcd_brightness_before_turn_off = 20;
int sem_pre_update_progress = -1;
int sem_pre_check_update_progress = 0;
int sem_pre_selected_lang_num = 0;
int sem_pre_selected_edition_num = 0;
double sem_pre_scroll_speed = 0.5;
double sem_pre_y_offset = 0.0;
double sem_pre_buffer_size[10];
std::string sem_pre_lang = "en";
/*---------------------------------------------*/

bool sem_use_default_font = true;
bool sem_use_system_specific_font = false;
bool sem_use_external_font = false;
bool sem_loaded_external_font[46];
bool sem_load_external_font[46];
bool sem_check_update_thread_run = false;
bool sem_worker_thread_run = false;
bool sem_reload_msg_request = false;
bool sem_load_system_font_request = false;
bool sem_load_external_font_request = false;
bool sem_unload_external_font_request = false;
bool sem_change_wifi_state_request = false;
bool sem_delete_line_img_cache_request = false;
bool setting_main_run = false;
bool new_version_available = false;
bool need_gas_update = false;
bool sem_already_init = false;
bool sem_main_run = false;
bool sem_thread_suspend = false;
bool sem_check_update_request = false;
bool sem_show_patch_note_request = false;
bool sem_select_ver_request = false;
bool sem_available_ver[8];
bool sem_dl_file_request = false;
bool sem_allow_send_app_info = false;
bool sem_debug_mode = false;
bool sem_night_mode = false;
bool sem_vsync_mode = true;
bool sem_flash_mode = false;
bool sem_eco_mode = true;
bool sem_wifi_enabled = false;
bool sem_system_setting_menu_show = false;
int sem_lcd_brightness = 100;
int sem_time_to_turn_off_lcd = 1500;
int sem_lcd_brightness_before_turn_off = 20;
int sem_selected_menu_mode = 0;
int sem_update_progress = -1;
int sem_check_update_progress = 0;
int sem_selected_lang_num = 0;
int sem_selected_edition_num = 0;
int sem_current_app_ver = 17;
int sem_current_gas_ver = 6;
int sem_num_of_app_start = 0;
double sem_scroll_speed = 0.5;
double sem_y_offset = 0.0;
double sem_y_max = 0.0;
double sem_buffer_size[10];
std::string sem_lang = "en";
std::string sem_msg[SEM_NUM_OF_MSG];
std::string sem_newest_ver_data[11];
std::string sem_init_string = "Sem/Init";
std::string sem_exit_string = "Sem/Exit";
std::string sem_worker_thread_string = "Sem/Worker thread";
std::string sem_check_update_string = "Sem/Check update";

C2D_Image sem_help_image[7];
Thread sem_check_update_thread, sem_worker_thread;

Result_with_string Sem_load_setting(std::string file_name, std::string dir_name, int item_num, std::string out_data[])
{
	u32 read_size;
	Handle fs_handle = 0;
	FS_Archive fs_archive = 0;
	u8* fs_buffer;
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x2000);
	memset(fs_buffer, 0x0, 0x2000);

	result = File_load_from_file(file_name, fs_buffer, 0x2000, &read_size, dir_name, fs_handle, fs_archive);

	if (result.code == 0)
		result = Sem_parse_file((char*)fs_buffer, item_num, out_data);

	free(fs_buffer);
	return result;
}

Result_with_string Sem_parse_file(std::string source_data, int num_of_items, std::string out_data[])
{
	Result_with_string result;

	size_t parse_start_num = 0;
	size_t parse_end_num = 0;
	std::string parse_start_text;
	std::string parse_end_text;

	for (int i = 0; i < num_of_items; i++)
	{
		parse_start_text = "<" + std::to_string(i) + ">";
		parse_start_num = source_data.find(parse_start_text);
		parse_end_text = "</" + std::to_string(i) + ">";
		parse_end_num = source_data.find(parse_end_text);

		if (parse_end_num == std::string::npos || parse_start_num == std::string::npos)
		{
			result.code = -9948;
			result.string = "[Error] Failed to load settings. error pos : " + std::to_string(i) + " ";
			break;
		}

		parse_start_num += parse_start_text.length();
		parse_end_num -= parse_start_num;
		out_data[i] = source_data.substr(parse_start_num, parse_end_num);
	}

	return result;
}

std::string Sem_convert_seconds_to_time(double input_seconds)
{
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	long count = 0;
	std::string time = "";

	for(count = 0; count < input_seconds; count++)
	{
		if(seconds + 1 >= 60)
		{
			if(minutes + 1 >= 60)
			{
				seconds = 0;
				minutes = 0;
				hours++;
			}
			else
			{
				seconds = 0;
				minutes++;
			}
		}
		else
			seconds++;
	}

	if(hours != 0)
		time += std::to_string(hours) + ":";

	if(minutes < 10)
		time += "0" + std::to_string(minutes) + ":";
	else
		time += std::to_string(minutes) + ":";

	if(seconds < 10)
		time += "0" + std::to_string(seconds);
	else
		time += std::to_string(seconds);

	time += std::to_string(input_seconds - count + 1).substr(1, 2);
	return time;
}

bool Sem_query_init_flag(void)
{
	return sem_already_init;
}

bool Sem_query_running_flag(void)
{
	return sem_main_run;
}

bool Sem_query_available_edtion(int edtion_num)
{
	if (edtion_num >= 0 && edtion_num <= 7)
	{
		for (int i = 0; i < 8; i++)
		{
			if (i == edtion_num)
				return sem_available_ver[i];
		}
	}
	else
		return false;

	return false;
}

int Sem_query_app_ver(void)
{
	return sem_current_app_ver;
}

int Sem_query_gas_ver(void)
{
	return sem_current_gas_ver;
}

bool Sem_query_font_flag(int font_num)
{
	if (font_num == SEM_USE_DEFAULT_FONT)
		return sem_use_default_font;
	else if (font_num == SEM_USE_SYSTEM_SPEIFIC_FONT)
		return sem_use_system_specific_font;
	else if (font_num == SEM_USE_EXTERNAL_FONT)
		return sem_use_external_font;
	else
		return false;
}

std::string Sem_query_lang(void)
{
	return sem_lang;
}

bool Sem_query_loaded_external_font_flag(int external_font_num)
{
	if (external_font_num >= 0 && external_font_num <= 45)
		return sem_loaded_external_font[external_font_num];
	else
		return false;
}

bool Sem_query_operation_flag(int operation_num)
{
	if (operation_num == SEM_CHECK_UPDATE_REQUEST)
		return sem_check_update_request;
	else if (operation_num == SEM_SHOW_PATCH_NOTE_REQUEST)
		return sem_show_patch_note_request;
	else if (operation_num == SEM_SELECT_VER_REQUEST)
		return sem_select_ver_request;
	else if (operation_num == SEM_DL_FILE_REQUEST)
		return sem_dl_file_request;
	else if (operation_num == SEM_LOAD_SYSTEM_FONT_REQUEST)
		return sem_load_system_font_request;
	else if (operation_num == SEM_LOAD_EXTERNAL_FONT_REQUEST)
		return sem_load_external_font_request;
	else if (operation_num == SEM_UNLOAD_EXTERNAL_FONT_REQUEST)
		return sem_unload_external_font_request;
	else if (operation_num == SEM_RELOAD_MSG_REQUEST)
		return sem_reload_msg_request;
	else if (operation_num == SEM_CHANGE_WIFI_STATE_REQUEST)
		return sem_change_wifi_state_request;
	else if (operation_num == SEM_DELETE_LINE_IMG_CACHE_REQUEST)
		return sem_delete_line_img_cache_request;
	else
		return false;
}

int Sem_query_selected_num(int item_num)
{
	if (item_num == SEM_SELECTED_LANG_NUM)
		return sem_selected_lang_num;
	else if (item_num == SEM_SELECTED_EDITION_NUM)
		return sem_selected_edition_num;
	else if (item_num == SEM_SELECTED_MENU_MODE_NUM)
		return sem_selected_menu_mode;
	else
		return -1;
}

bool Sem_query_settings(int item_num)
{
	if (item_num == SEM_NIGHT_MODE)
		return sem_night_mode;
	else if (item_num == SEM_VSYNC_MODE)
		return sem_vsync_mode;
	else if (item_num == SEM_FLASH_MODE)
		return sem_flash_mode;
	else if (item_num == SEM_DEBUG_MODE)
		return sem_debug_mode;
	else if (item_num == SEM_ALLOW_SEND_APP_INFO)
		return sem_allow_send_app_info;
	else if (item_num == SEM_WIFI_ENABLED)
		return sem_wifi_enabled;
	else if(item_num == SEM_SYSTEM_SETTING_MENU_SHOW)
		return sem_system_setting_menu_show;
	else if(item_num == SEM_ECO_MODE)
		return sem_eco_mode;
	else
		return false;
}

int Sem_query_settings_i(int item_num)
{
	if(item_num == SEM_LCD_BRIGHTNESS)
		return sem_lcd_brightness;
	else if(item_num == SEM_TIME_TO_TURN_OFF_LCD)
		return sem_time_to_turn_off_lcd;
	else if(item_num == SEM_LCD_BRIGHTNESS_BEFORE_TURN_OFF)
		return sem_lcd_brightness_before_turn_off;
	else if(item_num == SEM_NUM_OF_APP_START)
		return sem_num_of_app_start;
	else
	return -1;
}

double Sem_query_settings_d(int item_num)
{
	if(item_num == SEM_SCROLL_SPEED)
		return sem_scroll_speed;
	else
		return -1;
}

double Sem_query_y_max(void)
{
	return sem_y_max;
}

double Sem_query_y_offset(void)
{
	return sem_y_offset;
}

void Sem_set_font_flag(int font_num, bool flag)
{
	if (font_num == SEM_USE_DEFAULT_FONT)
		sem_use_default_font = flag;
	else if (font_num == SEM_USE_SYSTEM_SPEIFIC_FONT)
		sem_use_system_specific_font = flag;
	else if (font_num == SEM_USE_EXTERNAL_FONT)
		sem_use_external_font = flag;
}

void Sem_set_lang(std::string lang)
{
	sem_lang = lang;
}

void Sem_set_load_external_font_request(int external_font_num, bool flag)
{
	if (external_font_num >= 0 && external_font_num <= 45)
		sem_load_external_font[external_font_num] = flag;
}

void Sem_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < SEM_NUM_OF_MSG)
		sem_msg[msg_num] = msg;
}

void Sem_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == SEM_CHECK_UPDATE_REQUEST)
		sem_check_update_request = flag;
	else if (operation_num == SEM_SHOW_PATCH_NOTE_REQUEST)
		sem_show_patch_note_request = flag;
	else if (operation_num == SEM_SELECT_VER_REQUEST)
		sem_select_ver_request = flag;
	else if (operation_num == SEM_DL_FILE_REQUEST)
		sem_dl_file_request = flag;
	else if (operation_num == SEM_LOAD_SYSTEM_FONT_REQUEST)
		sem_load_system_font_request = flag;
	else if (operation_num == SEM_LOAD_EXTERNAL_FONT_REQUEST)
		sem_load_external_font_request = flag;
	else if (operation_num == SEM_UNLOAD_EXTERNAL_FONT_REQUEST)
		sem_unload_external_font_request = flag;
	else if (operation_num == SEM_RELOAD_MSG_REQUEST)
		sem_reload_msg_request = flag;
	else if (operation_num == SEM_CHANGE_WIFI_STATE_REQUEST)
		sem_change_wifi_state_request = flag;
	else if (operation_num == SEM_DELETE_LINE_IMG_CACHE_REQUEST)
		sem_delete_line_img_cache_request = flag;
}

void Sem_set_selected_num(int item_num, int num)
{
	if (item_num == SEM_SELECTED_LANG_NUM)
		sem_selected_lang_num = num;
	else if (item_num == SEM_SELECTED_EDITION_NUM)
		sem_selected_edition_num = num;
	else if (item_num == SEM_SELECTED_MENU_MODE_NUM)
		sem_selected_menu_mode = num;
}

void Sem_set_settings(int item_num, bool flag)
{
	if (item_num == SEM_NIGHT_MODE)
		sem_night_mode = flag;
	else if (item_num == SEM_VSYNC_MODE)
		sem_vsync_mode = flag;
	else if (item_num == SEM_FLASH_MODE)
		sem_flash_mode = flag;
	else if (item_num == SEM_DEBUG_MODE)
		sem_debug_mode = flag;
	else if (item_num == SEM_ALLOW_SEND_APP_INFO)
		sem_allow_send_app_info = flag;
	else if (item_num == SEM_WIFI_ENABLED)
		sem_wifi_enabled = flag;
	else if(item_num == SEM_SYSTEM_SETTING_MENU_SHOW)
		sem_system_setting_menu_show = flag;
	else if(item_num == SEM_ECO_MODE)
		sem_eco_mode = flag;
}

void Sem_set_settings_i(int item_num, int value)
{
	if(item_num == SEM_LCD_BRIGHTNESS)
		sem_lcd_brightness = value;
	else if(item_num == SEM_TIME_TO_TURN_OFF_LCD)
		sem_time_to_turn_off_lcd = value;
	else if(item_num == SEM_LCD_BRIGHTNESS_BEFORE_TURN_OFF)
		sem_lcd_brightness_before_turn_off = value;
	else if(item_num == SEM_NUM_OF_APP_START)
		sem_num_of_app_start = value;
}

void Sem_set_settings_d(int item_num, double value)
{
	if(item_num == SEM_SCROLL_SPEED)
		sem_scroll_speed = value;
}

void Sem_set_y_max(double y_max)
{
	sem_y_max = y_max;
}

void Sem_set_y_offset(double y)
{
	sem_y_offset = y;
}

void Sem_set_color(double in_red, double in_green, double in_blue, double in_alpha, double* out_red, double* out_green, double* out_blue, double* out_alpha, int num_of_out)
{
	for (int i = 0; i < num_of_out; i++)
	{
		out_red[i] = in_red;
		out_blue[i] = in_blue;
		out_green[i] = in_green;
		out_alpha[i] = in_alpha;
	}
}

void Sem_suspend(void)
{
	Menu_resume();
	sem_thread_suspend = true;
	sem_main_run = false;
}

void Sem_resume(void)
{
	sem_thread_suspend = false;
	sem_main_run = true;
	sem_need_reflesh = true;
	Menu_suspend();
}

void Sem_init(void)
{
	Log_log_save(sem_init_string, "Initializing...", 1234567890, DEBUG);
	int log_num = 0;
	int buffer_size[9];
	int buffer_default_size[9] = { 0x100000, 0x100000, 0x200000, 0x500000, 0x200000, 0x200000, 0x200000, 0x300000, 0x100000, };
	int buffer_max_size[9] = { 0xA00000, 0xA00000, 0x4C0000, 0x1400000, 0x700000, 0x500000, 0x500000, 0xA00000, 0x200000, };
	std::string data[19];
	Result_with_string result;

	sem_lang = "en";
	sem_lcd_brightness = 100;
	sem_time_to_turn_off_lcd = 1500;
	sem_lcd_brightness_before_turn_off = 15;
	sem_system_setting_menu_show = false;
	sem_scroll_speed = 0.5;
	sem_allow_send_app_info = false;
	sem_num_of_app_start = 0;
	sem_night_mode = false;
	sem_vsync_mode = true;
	sem_eco_mode = true;

	Draw_progress("0/1 [Sem] Loading settings...");
	log_num = Log_log_save(sem_init_string , "Sem_load_setting()...", 1234567890, DEBUG);
	result = Sem_load_setting("Sem_setting.txt", "/Line/", 19, data);
	Log_log_add(log_num, result.string, result.code, DEBUG);
	if(result.code == 0)
	{
		sem_lang = data[0];
		sem_lcd_brightness = atoi(data[1].c_str());
		sem_time_to_turn_off_lcd = atoi(data[2].c_str());
		sem_lcd_brightness_before_turn_off = atoi(data[3].c_str());
		sem_scroll_speed = strtod(data[4].c_str(), NULL);
		sem_allow_send_app_info = (data[5] == "1");
		sem_num_of_app_start = atoi(data[6].c_str());
		sem_night_mode = (data[7] == "1");
		sem_vsync_mode = (data[8] == "1");
		sem_eco_mode = (data[9] == "1");

		if(sem_lang != "jp" && sem_lang != "en")
			sem_lang = "en";
		if(sem_lcd_brightness < 15 || sem_lcd_brightness > 163)
			sem_lcd_brightness = 100;
		if(sem_time_to_turn_off_lcd < 100 || sem_time_to_turn_off_lcd > 3090)
			sem_time_to_turn_off_lcd = 1500;
		if(sem_lcd_brightness_before_turn_off < 15 || sem_lcd_brightness_before_turn_off > 163)
			sem_lcd_brightness_before_turn_off = 15;
		if(sem_scroll_speed < 0.033 || sem_scroll_speed > 1.030)
			sem_scroll_speed = 0.5;
		if(sem_num_of_app_start < 0)
			sem_num_of_app_start = 0;
	}

	for(int i = 0; i < 9; i++)
	{
		buffer_size[i] = atoi(data[10 + i].c_str());
		if(buffer_size[i] < 0x40000 || buffer_size[i] > buffer_max_size[i])
			buffer_size[i] = buffer_default_size[i];
	}
	Line_set_buffer_size(LINE_HTTPC_BUFFER, buffer_size[0]);
	Line_set_buffer_size(LINE_FS_BUFFER, buffer_size[1]);
	Line_set_buffer_size(LINE_SEND_FS_CACHE_BUFFER, buffer_size[2]);
	Line_set_buffer_size(LINE_SEND_FS_BUFFER, buffer_size[3]);
	Spt_set_buffer_size(SPT_HTTPC_BUFFER, buffer_size[4]);
	Imv_set_buffer_size(IMV_HTTPC_BUFFER, buffer_size[5]);
	Imv_set_buffer_size(IMV_FS_BUFFER, buffer_size[6]);
	Mup_set_buffer_size(MUP_FS_OUT_BUFFER, buffer_size[7]);
	Mup_set_buffer_size(MUP_FS_IN_BUFFER, buffer_size[8]);

	Draw_progress("1/1 [Sem] Starting threads...");
	sem_check_update_thread_run = true;
	sem_worker_thread_run = true;
	sem_check_update_thread = threadCreate(Sem_check_update_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
	sem_worker_thread = threadCreate(Sem_worker_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);

	Sem_resume();
	sem_already_init = true;
	Log_log_save(sem_init_string, "Initialized.", 1234567890, DEBUG);
}

void Sem_exit(void)
{
	Log_log_save(sem_exit_string, "Exiting...", 1234567890, DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	std::string data = "<0>" + sem_lang + "</0><1>" + std::to_string(sem_lcd_brightness) + "</1><2>" + std::to_string(sem_time_to_turn_off_lcd)
	+ "</2><3>" + std::to_string(sem_lcd_brightness_before_turn_off) + "</3><4>" + std::to_string(sem_scroll_speed) + "</4><5>" + std::to_string(sem_allow_send_app_info)
	+ "</5><6>" + std::to_string(sem_num_of_app_start) + "</6><7>" + std::to_string(sem_night_mode) + "</7><8>" + std::to_string(sem_vsync_mode)
	+ "</8><9>" + std::to_string(sem_eco_mode) + "</9><10>" + std::to_string(Line_query_buffer_size(LINE_HTTPC_BUFFER))
	+ "</10><11>" + std::to_string(Line_query_buffer_size(LINE_FS_BUFFER)) + "</11><12>" + std::to_string(Line_query_buffer_size(LINE_SEND_FS_CACHE_BUFFER))
	+ "</12><13>" + std::to_string(Line_query_buffer_size(LINE_SEND_FS_BUFFER)) + "</13><14>" + std::to_string(Spt_query_buffer_size(SPT_HTTPC_BUFFER))
	+ "</14><15>" + std::to_string(Imv_query_buffer_size(IMV_HTTPC_BUFFER)) + "</15><16>" + std::to_string(Imv_query_buffer_size(IMV_FS_BUFFER))
	+ "</16><17>" + std::to_string(Mup_query_buffer_size(MUP_FS_OUT_BUFFER)) + "</17><18>" + std::to_string(Mup_query_buffer_size(MUP_FS_IN_BUFFER)) + "</18>";
	Handle fs_handle = 0;
	FS_Archive fs_archive = 0;
	Result_with_string result;

	Draw_progress("[Sem] Exiting...");
	sem_already_init = false;
	sem_thread_suspend = false;
	sem_check_update_thread_run = false;
	sem_worker_thread_run = false;

	log_num = Log_log_save(sem_exit_string, "File_save_to_file()...", 1234567890, DEBUG);
	result = File_save_to_file("Sem_setting.txt", (u8*)data.c_str(), data.length(), "/Line/", true, fs_handle, fs_archive);
	Log_log_add(log_num, result.string, result.code, DEBUG);

	log_num = Log_log_save(sem_exit_string, "threadJoin()0/1...", 1234567890, DEBUG);
	result.code = threadJoin(sem_check_update_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, DEBUG);
	}

	log_num = Log_log_save(sem_exit_string, "threadJoin()1/1...", 1234567890, DEBUG);
	result.code = threadJoin(sem_worker_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, DEBUG);
	}

	threadFree(sem_check_update_thread);
	threadFree(sem_worker_thread);

	if (failed)
		Log_log_save(sem_exit_string, "[Warn] Some function returned error.", 1234567890, DEBUG);

	Log_log_save(sem_exit_string, "Exited.", 1234567890, DEBUG);
}

void Sem_main(void)
{
	int msg_num = 0;
	double buffer_max_size[10] = { 0xA00000, 0x500000, 0x4C0000, 0x1400000, 0x700000, 0x500000, 0x500000, 0xA00000, 0x200000, };
	double draw_x;
	double draw_y;
	double size_x_offset;
	double text_red, text_green, text_blue, text_alpha;
	double red[46], green[46], blue[46], alpha[46];
	C2D_ImageTint draw_tint[2];

	if (sem_night_mode)
	{
		text_red = 1.0;
		text_green = 1.0;
		text_blue = 1.0;
		text_alpha = 0.75;
		white_or_black_tint = white_tint;
	}
	else
	{
		text_red = 0.0;
		text_green = 0.0;
		text_blue = 0.0;
		text_alpha = 1.0;
		white_or_black_tint = black_tint;
	}
	Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 46);
	sem_buffer_size[0] = Line_query_buffer_size(LINE_HTTPC_BUFFER);
	sem_buffer_size[1] = Line_query_buffer_size(LINE_FS_BUFFER);
	sem_buffer_size[2] = Line_query_buffer_size(LINE_SEND_FS_CACHE_BUFFER);
	sem_buffer_size[3] = Line_query_buffer_size(LINE_SEND_FS_BUFFER);
	sem_buffer_size[4] = Spt_query_buffer_size(SPT_HTTPC_BUFFER);
	sem_buffer_size[5] = Imv_query_buffer_size(IMV_HTTPC_BUFFER);
	sem_buffer_size[6] = Imv_query_buffer_size(IMV_FS_BUFFER);
	sem_buffer_size[7] = Mup_query_buffer_size(MUP_FS_OUT_BUFFER);
	sem_buffer_size[8] = Mup_query_buffer_size(MUP_FS_IN_BUFFER);

	for (int i = 0; i < 46; i++)
	{
		if (sem_pre_loaded_external_font[i] != sem_loaded_external_font[i])
		{
			sem_need_reflesh = true;
			break;
		}
	}

	for (int i = 0; i < 10; i++)
	{
		if (sem_pre_buffer_size[i] != sem_buffer_size[i])
		{
			sem_need_reflesh = true;
			break;
		}
	}

	if(sem_need_reflesh || sem_pre_y_offset != sem_y_offset || sem_pre_lang != sem_lang || sem_pre_night_mode != sem_night_mode
	|| sem_pre_vsync_mode != sem_vsync_mode || sem_pre_lcd_brightness != sem_lcd_brightness
	|| sem_pre_time_to_turn_off_lcd != sem_time_to_turn_off_lcd || sem_pre_lcd_brightness_before_turn_off != sem_lcd_brightness_before_turn_off
	|| sem_pre_scroll_speed != sem_scroll_speed || sem_pre_use_default_font != sem_use_default_font
	|| sem_pre_use_system_specific_font != sem_use_system_specific_font || sem_pre_use_external_font != sem_use_external_font
	|| sem_pre_selected_lang_num != sem_selected_lang_num || sem_pre_unload_external_font_request != sem_unload_external_font_request
	|| sem_pre_load_external_font_request != sem_load_external_font_request || sem_pre_allow_send_app_info != sem_allow_send_app_info
	|| sem_pre_debug_mode != sem_debug_mode || sem_pre_delete_line_img_cache_request != sem_delete_line_img_cache_request
	|| sem_pre_eco_mode != sem_eco_mode || sem_pre_select_ver_request != sem_select_ver_request || sem_pre_show_patch_note_request != sem_show_patch_note_request
	|| sem_pre_check_update_progress != sem_check_update_progress || sem_pre_update_progress != sem_update_progress
	|| sem_pre_selected_edition_num != sem_selected_edition_num)
	{
		for (int i = 0; i < 10; i++)
			sem_pre_buffer_size[i] = sem_buffer_size[i];

		for (int i = 0; i < 46; i++)
			sem_pre_loaded_external_font[i] = sem_loaded_external_font[i];

		sem_pre_y_offset = sem_y_offset;
		sem_pre_lang = sem_lang;
		sem_pre_night_mode = sem_night_mode;
		sem_pre_vsync_mode = sem_vsync_mode;
		sem_pre_lcd_brightness = sem_lcd_brightness;
		sem_pre_time_to_turn_off_lcd = sem_time_to_turn_off_lcd;
		sem_pre_lcd_brightness_before_turn_off = sem_lcd_brightness_before_turn_off;
		sem_pre_scroll_speed = sem_scroll_speed;
		sem_pre_use_default_font = sem_use_default_font;
		sem_pre_use_system_specific_font = sem_use_system_specific_font;
		sem_pre_use_external_font = sem_use_external_font;
		sem_pre_selected_lang_num = sem_selected_lang_num;
		sem_pre_unload_external_font_request = sem_unload_external_font_request;
		sem_pre_load_external_font_request = sem_load_external_font_request;
		sem_pre_allow_send_app_info = sem_allow_send_app_info;
		sem_pre_debug_mode = sem_debug_mode;
		sem_pre_delete_line_img_cache_request = sem_delete_line_img_cache_request;
		sem_pre_eco_mode = sem_eco_mode;
		sem_pre_select_ver_request = sem_select_ver_request;
		sem_pre_show_patch_note_request = sem_show_patch_note_request;
		sem_pre_check_update_progress = sem_check_update_progress;
		sem_pre_update_progress = sem_update_progress;
		sem_pre_selected_edition_num = sem_selected_edition_num;
		sem_need_reflesh = true;
	}

	if(Draw_query_need_reflesh() || !sem_eco_mode)
		sem_need_reflesh = true;

	if(sem_need_reflesh)
	{
		Draw_set_draw_mode(sem_vsync_mode);
		if (sem_night_mode)
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		Draw_top_ui();

		if (sem_night_mode)
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		if (sem_selected_menu_mode >= 1 && sem_selected_menu_mode <= 8)
		{
			draw_y = 0.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				Draw_texture(Square_image, weak_red_tint, 0, 0.0, draw_y + sem_y_offset, 40.0, 25.0);
				Draw(sem_msg[42], 0, 0.0, draw_y + sem_y_offset + 5.0, 0.6, 0.6, text_red, text_green, text_blue, text_alpha);
			}
		}

		if ((sem_selected_menu_mode == 5 && sem_use_external_font) || sem_selected_menu_mode == 6)
		{
			Draw_texture(Square_image, white_or_black_tint, 0, 312.5, 0.0, 7.5, 15.0);
			Draw_texture(Square_image, white_or_black_tint, 0, 312.5, 215.0, 7.5, 10.0);
			Draw_texture(Square_image, blue_tint, 0, 312.5, 15.0 + (195 * (sem_y_offset / sem_y_max)), 7.5, 5.0);
		}

		if (sem_selected_menu_mode == 0)
		{
			draw_y = 0.0;

			for (int i = 0; i < 8; i++)
			{
				msg_num = i;
				if(i == 7)
					msg_num = 73;

				Draw_texture(Square_image, weak_aqua_tint, 0, 0.0, draw_y + sem_y_offset, 240.0, 20.0);
				Draw(sem_msg[msg_num], 0, 0.0, draw_y + sem_y_offset - 2.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
				draw_y += 25.0;
			}
		}
		else if (sem_selected_menu_mode == 1)
		{
			//Check for updates
			draw_y = 25.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, draw_y + sem_y_offset, 240.0, 20.0);
				Draw(sem_msg[7], 0, 10.0, draw_y + sem_y_offset - 2.5, 0.75, 0.75, text_red, text_green, text_blue, text_alpha);
			}
		}
		else if (sem_selected_menu_mode == 2)
		{
			//Language
			draw_y = 25.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				if (sem_lang == "en")
				{
					red[0] = 1.0;
					blue[0] = 0.0;
					green[0] = 0.0;
					alpha[0] = 1.0;
				}
				else
				{
					red[1] = 1.0;
					blue[1] = 0.0;
					green[1] = 0.0;
					alpha[1] = 1.0;
				}

				for (int i = 0; i < 2; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, 10.0, draw_y + sem_y_offset + (25.0 * i), 240.0, 20.0);
					Draw(sem_msg[8 + i], 0, 10.0, draw_y + sem_y_offset + (25.0 * i), 0.75, 0.75, red[i], green[i], blue[i], alpha[i]);
				}
			}
		}
		else if (sem_selected_menu_mode == 3)
		{
			//Night mode
			draw_y = 25.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				if (sem_night_mode)
				{
					red[0] = 1.0;
					blue[0] = 0.0;
					green[0] = 0.0;
					alpha[0] = 1.0;
				}
				else
				{
					red[1] = 1.0;
					blue[1] = 0.0;
					green[1] = 0.0;
					alpha[1] = 1.0;
				}
				if (sem_flash_mode)
				{
					red[2] = 1.0;
					blue[2] = 0.0;
					green[2] = 0.0;
					alpha[2] = 1.0;
				}
				size_x_offset = 0.0;

				Draw(sem_msg[10], 0, 0.0, draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				for (int i = 0; i < 3; i++)
				{
					if (i == 2)
						size_x_offset = -50.0;

					Draw_texture(Square_image, weak_aqua_tint, 0, 10.0 + (i * 100.0), draw_y + sem_y_offset + 15.0, 90.0 + size_x_offset, 20.0);
					Draw(sem_msg[11 + i], 0, 10.0 + (i * 100.0), draw_y + sem_y_offset + 12.5, 0.65, 0.65, red[i], green[i], blue[i], alpha[i]);
				}
			}

			//Vsync
			draw_y = 65.0;
			Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 2);
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				if (sem_vsync_mode)
				{
					red[0] = 1.0;
					blue[0] = 0.0;
					green[0] = 0.0;
					alpha[0] = 1.0;
				}
				else
				{
					red[1] = 1.0;
					blue[1] = 0.0;
					green[1] = 0.0;
					alpha[1] = 1.0;
				}

				Draw(sem_msg[14], 0, 0.0, draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				for (int i = 0; i < 2; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, 10.0 + (i * 100.0), draw_y + sem_y_offset + 15.0, 90.0, 20.0);
					Draw(sem_msg[11 + i], 0, 10.0 + (i * 100.0), draw_y + sem_y_offset + 12.5, 0.75, 0.75, red[i], green[i], blue[i], alpha[i]);
				}
			}

			//Screen brightness
			draw_y = 105.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				Draw(sem_msg[15] + std::to_string(sem_lcd_brightness), 0, 0.0, draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				Draw_texture(Square_image, weak_red_tint, 0, 10.0, draw_y + sem_y_offset + 22.5, 300.0, 5.0);
				Draw_texture(Square_image, white_or_black_tint, 0, (sem_lcd_brightness - 10) * 2, draw_y + sem_y_offset + 15.0, 4.0, 20.0);
			}

			//Time to turn off LCDs
			draw_y = 145.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				Draw(sem_msg[16] + std::to_string(sem_time_to_turn_off_lcd / 10) + sem_msg[17], 0, 0.0, draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				Draw_texture(Square_image, weak_red_tint, 0, 10.0, draw_y + sem_y_offset + 22.5, 300.0, 5.0);
				Draw_texture(Square_image, white_or_black_tint, 0, (sem_time_to_turn_off_lcd / 10), draw_y + sem_y_offset + 15.0, 4.0, 20.0);
			}

			//Screen brightness before turn off LCDs
			draw_y = 185.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				Draw(sem_msg[18] + std::to_string(sem_lcd_brightness_before_turn_off), 0, 0.0, draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				Draw_texture(Square_image, weak_red_tint, 0, 10.0, draw_y + sem_y_offset + 22.5, 300.0, 5.0);
				Draw_texture(Square_image, white_or_black_tint, 0, (sem_lcd_brightness_before_turn_off - 10) * 2, draw_y + sem_y_offset + 15.0, 4.0, 20.0);
			}
		}
		else if (sem_selected_menu_mode == 4)
		{
			//Scroll speed
			draw_y = 25.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				Draw(sem_msg[19] + std::to_string(sem_scroll_speed), 0, 0.0, draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				Draw_texture(Square_image, weak_red_tint, 0, 10.0, draw_y + sem_y_offset + 22.5, 300.0, 5.0);
				Draw_texture(Square_image, white_or_black_tint, 0, (sem_scroll_speed * 300), draw_y + sem_y_offset + 15.0, 4.0, 20.0);
			}
		}
		else if (sem_selected_menu_mode == 5)
		{
			//Font
			draw_y = 25.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				if (sem_use_default_font)
				{
					red[0] = 1.0;
					blue[0] = 0.0;
					green[0] = 0.0;
					alpha[0] = 1.0;
				}
				else if (sem_use_system_specific_font)
				{
					red[2] = 1.0;
					blue[2] = 0.0;
					green[2] = 0.0;
					alpha[2] = 1.0;
				}
				else if (sem_use_external_font)
				{
					red[1] = 1.0;
					blue[1] = 0.0;
					green[1] = 0.0;
					alpha[1] = 1.0;
				}

				for (int i = 0; i < 3; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, 10.0 + (i * 100.0), draw_y + sem_y_offset + 15.0, 90.0, 20.0);
					Draw(sem_msg[20 + i], 0, 10.0 + (i * 100.0), draw_y + sem_y_offset + 12.5, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
				}
			}

			//Font, System specific
			if (sem_use_system_specific_font)
			{
				draw_x = 10.0;
				draw_y = 65.0;
				Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 4);
				if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
				{
					for (int i = 0; i < 4; i++)
					{

						if (sem_selected_lang_num == i)
						{
							red[i] = 1.0;
							green[i] = 0.0;
							blue[i] = 0.0;
							if (sem_load_system_font_request)
								alpha[i] = 0.3;
							else
								alpha[i] = 1.0;
						}
						else if (sem_load_system_font_request)
							alpha[i] = 0.3;

						Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y + sem_y_offset + 15.0, 70.0, 20.0);
						Draw(sem_msg[23 + i], 0, draw_x, draw_y + sem_y_offset + 12.5, 0.75, 0.75, red[i], green[i], blue[i], alpha[i]);

						draw_x += 75.0;
					}
				}
			}
			else if (sem_use_external_font)//Font, External
			{
				draw_x = 10.0;
				draw_y = 65.0;
				Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 1);
				if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
				{
					draw_tint[0] = weak_red_tint;
					draw_tint[1] = weak_yellow_tint;
					if (sem_unload_external_font_request || sem_load_external_font_request)
						alpha[0] = 0.3;

					for (int i = 0; i < 2; i++)
					{
						Draw_texture(Square_image, draw_tint[i], 0, draw_x, draw_y + sem_y_offset + 15.0, 100.0, 20.0);
						Draw(sem_msg[27 + i], 0, draw_x, draw_y + sem_y_offset + 12.5, 0.65, 0.65, red[0], green[0], blue[0], alpha[0]);

						draw_x += 100.0;
					}
				}

				draw_x = 10.0;
				draw_y = 100.0;
				Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 46);
				for (int i = 0; i < 46; i++)
				{
					if (sem_loaded_external_font[i])
					{
						red[i] = 1.0;
						green[i] = 0.0;
						blue[i] = 0.0;

						if(sem_unload_external_font_request || sem_load_external_font_request)
							alpha[i] = 0.3;
						else
							alpha[i] = 1.0;
					}
					else if (sem_unload_external_font_request || sem_load_external_font_request)
						alpha[i] = 0.3;

					if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
					{
						Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y + sem_y_offset, 200.0, 20.0);
						Draw(Exfont_query_font_name(i), 0, draw_x, draw_y + sem_y_offset - 2.5, 0.45, 0.45, red[i], green[i], blue[i], alpha[i]);
					}
					draw_y += 20.0;
				}
			}
		}
		else if (sem_selected_menu_mode == 6)
		{
			//Buffer size
			if (Line_query_init_flag())
			{
				for (int i = 0; i < 4; i++)
					alpha[i] = 0.25;
			}
			if (Spt_query_init_flag())
			{
				alpha[4] = 0.25;
			}
			if (Imv_query_init_flag())
			{
				for (int i = 5; i < 7; i++)
					alpha[i] = 0.25;
			}
			if (Mup_query_init_flag())
			{
				for (int i = 7; i < 9; i++)
					alpha[i] = 0.25;
			}

			draw_x = 10.0;
			draw_y = 25.0;
			for (int i = 0; i < 9; i++)
			{
				if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
				{
					Draw(sem_msg[29 + i] + std::to_string(sem_buffer_size[i] / 1024.0 / 1024.0).substr(0, 5) + "MB", 0, (draw_x - 10.0), draw_y + sem_y_offset, 0.5, 0.5, red[i], green[i], blue[i], alpha[i]);
					Draw_texture(Square_image, weak_red_tint, 0, draw_x, draw_y + sem_y_offset + 22.5, 300.0, 5.0);
					Draw_texture(Square_image, white_or_black_tint, 0, (((sem_buffer_size[i] - 0x40000) / (buffer_max_size[i] - 0x40000)) * 295.0) + draw_x, draw_y + sem_y_offset + 15.0, 5.0, 20.0);
				}
				draw_y += 40.0;
			}
		}
		else if (sem_selected_menu_mode == 7)
		{
			//Allow send app info
			draw_x = 10.0;
			draw_y = 25.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				if (sem_allow_send_app_info)
				{
					red[0] = 1.0;
					blue[0] = 0.0;
					green[0] = 0.0;
					alpha[0] = 1.0;
				}
				else
				{
					red[1] = 1.0;
					blue[1] = 0.0;
					green[1] = 0.0;
					alpha[1] = 1.0;
				}

				Draw(sem_msg[38], 0, (draw_x - 10.0), draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				for (int i = 0; i < 2; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y + sem_y_offset + 15.0, 90.0, 20.0);
					Draw(sem_msg[39 + i], 0, draw_x, draw_y + sem_y_offset + 12.5, 0.75, 0.75, red[i], green[i], blue[i], alpha[i]);

					draw_x += 100.0;
				}
			}

			//Debug mode
			draw_x = 10.0;
			draw_y = 65.0;
			Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 2);
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				if (sem_debug_mode)
				{
					red[0] = 1.0;
					blue[0] = 0.0;
					green[0] = 0.0;
					alpha[0] = 1.0;
				}
				else
				{
					red[1] = 1.0;
					blue[1] = 0.0;
					green[1] = 0.0;
					alpha[1] = 1.0;
				}

				Draw(sem_msg[41], 0, (draw_x - 10.0), draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				for (int i = 0; i < 2; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y + sem_y_offset + 15.0, 90.0, 20.0);
					Draw(sem_msg[11 + i], 0, draw_x, draw_y + sem_y_offset + 12.5, 0.75, 0.75, red[i], green[i], blue[i], alpha[i]);

					draw_x += 100.0;
				}
			}

			//Delete line img cache
			draw_x = 10.0;
			draw_y = 105.0;
			Sem_set_color(text_red, text_green, text_blue, text_alpha, red, green, blue, alpha, 1);
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				Draw(sem_msg[71], 0, (draw_x - 10.0), draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y + sem_y_offset + 15.0, 190.0, 20.0);

				if (sem_delete_line_img_cache_request)
					alpha[0] = 0.25;

				Draw(sem_msg[72], 0, draw_x, draw_y + sem_y_offset + 12.5, 0.75, 0.75, red[0], green[0], blue[0], alpha[0]);
			}
		}
		else if (sem_selected_menu_mode == 8)
		{
			//Eco mode
			draw_x = 10.0;
			draw_y = 25.0;
			if (draw_y + sem_y_offset >= -30 && draw_y + sem_y_offset <= 240)
			{
				if (sem_eco_mode)
				{
					red[0] = 1.0;
					blue[0] = 0.0;
					green[0] = 0.0;
					alpha[0] = 1.0;
				}
				else
				{
					red[1] = 1.0;
					blue[1] = 0.0;
					green[1] = 0.0;
					alpha[1] = 1.0;
				}

				Draw(sem_msg[74], 0, (draw_x - 10.0), draw_y + sem_y_offset, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
				for (int i = 0; i < 2; i++)
				{
					Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y + sem_y_offset + 15.0, 90.0, 20.0);
					Draw(sem_msg[11 + i], 0, draw_x, draw_y + sem_y_offset + 12.5, 0.75, 0.75, red[i], green[i], blue[i], alpha[i]);

					draw_x += 100.0;
				}
			}
		}

		if (sem_show_patch_note_request)
		{
			Draw_texture(Square_image, blue_tint, 0, 15.0, 15.0, 290.0, 200.0);
			Draw_texture(Square_image, weak_aqua_tint, 0, 15.0, 200.0, 145.0, 15.0);
			Draw_texture(Square_image, weak_white_tint, 0, 160.0, 200.0, 145.0, 15.0);

			if(sem_check_update_progress == 0)
				Draw(sem_msg[43], 0, 17.5, 15.0, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
			else if(sem_check_update_progress == 2)
				Draw(sem_msg[44], 0, 17.5, 15.0, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
			else if (sem_check_update_progress == 1)
			{
				Draw(sem_msg[45 + new_version_available], 0, 17.5, 15.0, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
				Draw(sem_msg[47 + need_gas_update], 0, 17.5, 30.0, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
				Draw(sem_newest_ver_data[10], 0, 17.5, 45.0, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);
			}
			Draw(sem_msg[50], 0, 17.5, 200.0, 0.4, 0.4, 1.0, 1.0, 1.0, 0.75);
			Draw(sem_msg[49], 0, 162.5, 200.0, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);
		}
		if (sem_select_ver_request)
		{
			Draw_texture(Square_image, blue_tint, 0, 15.0, 15.0, 290.0, 200.0);
			Draw_texture(Square_image, weak_white_tint, 0, 15.0, 200.0, 145.0, 15.0);
			Draw_texture(Square_image, weak_aqua_tint, 0, 160.0, 200.0, 145.0, 15.0);

			draw_y = 15.0;
			for (int i = 0; i < 8; i++)
			{
				if(sem_available_ver[i] && sem_selected_edition_num == i)
					Draw(sem_msg[51 + i], 0, 17.5, draw_y, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
				else if (sem_available_ver[i])
					Draw(sem_msg[51 + i], 0, 17.5, draw_y, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);
				else
					Draw(sem_msg[51 + i] + sem_msg[59], 0, 17.5, draw_y, 0.45, 0.45, 1.0, 1.0, 1.0, 0.25);

				draw_y += 10.0;
			}

			for(int i = 0; i < 4; i++)
				Draw(sem_msg[60 + i], 0, 17.5, 100.0 + (i * 10.0), 0.4, 0.4, 1.0, 1.0, 1.0, 0.75);

			if (sem_selected_edition_num == 0)
			{
				Draw(sem_msg[64], 0, 17.5, 140.0, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
				Draw("sdmc:/Line/ver_" + sem_newest_ver_data[0] + "/Line_for_3DS.3dsx", 0, 17.5, 150.0, 0.45, 0.45, 1.0, 0.0, 0.0, 1.0);
			}

			if (sem_update_progress >= 0 && sem_update_progress <= 3)
				Draw(sem_msg[65 + sem_update_progress], 0, 17.5, 160.0, 0.75, 0.75, 1.0, 1.0, 1.0, 0.75);

			if (sem_update_progress == 2)
				Draw(sem_msg[69], 0, 17.5, 180.0, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);

			if (sem_available_ver[sem_selected_edition_num])
				Draw(sem_msg[70], 0, 162.5, 200.0, 0.4, 0.4, 1.0, 1.0, 1.0, 0.75);
			else
				Draw(sem_msg[70], 0, 162.5, 200.0, 0.4, 0.4, 1.0, 1.0, 1.0, 0.25);

			Draw(sem_msg[49], 0, 17.5, 200.0, 0.45, 0.45, 1.0, 1.0, 1.0, 0.75);
		}
		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		sem_need_reflesh = false;
	}
	else
		gspWaitForVBlank();
}

void Sem_worker_thread(void* arg)
{
	Log_log_save(sem_worker_thread_string , "Thread started.", 1234567890, false);
	int log_num;
	int num_of_files;
	int num_of_msg[8] = { LINE_NUM_OF_MSG, SEM_NUM_OF_MSG, IMV_NUM_OF_MSG, SPT_NUM_OF_MSG, GTR_NUM_OF_MSG, CAM_NUM_OF_MSG, MUP_NUM_OF_MSG, MIC_NUM_OF_MSG, };
	u8* fs_buffer;
	u32 read_size;
	std::string setting_data[128];
	std::string file_name[256];
	std::string file_type[256];
	std::string load_file_name[8] = {"line_", "sem_", "imv_", "spt_", "gtr_", "cam_", "mup_", "mic_" };
	FS_Archive fs_archive = 0;
	Result_with_string result;

	fs_buffer = (u8*)malloc(0x2000);

	while (sem_worker_thread_run)
	{
		if (sem_load_system_font_request)
		{
			for (int i = 0; i < 4; i++)
				Draw_free_system_font(i);

			Draw_load_system_font(sem_selected_lang_num);
			sem_load_system_font_request = false;
		}
		else if (sem_unload_external_font_request)
		{
			for (int i = 0; i < 46; i++)
			{
				if (!sem_load_external_font[i] && sem_loaded_external_font[i])
				{
					Exfont_unload_exfont(i);
					sem_loaded_external_font[i] = false;
				}
			}
			sem_unload_external_font_request = false;
		}
		else if (sem_load_external_font_request)
		{
			for (int i = 0; i < 46; i++)
			{
				if (sem_load_external_font[i] && !sem_loaded_external_font[i])
				{
					log_num = Log_log_save(sem_worker_thread_string, "Exfont_load_exfont()...", 1234567890, false);
					result = Exfont_load_exfont(i);
					Log_log_add(log_num, result.string, result.code, false);

					if(result.code == 0)
						sem_loaded_external_font[i] = true;
					else
						sem_loaded_external_font[i] = false;
				}
			}

			sem_load_external_font_request = false;
		}
		else if (sem_reload_msg_request)
		{
			for (int i = 0; i < 8; i++)
			{
				log_num = Log_log_save(sem_worker_thread_string, "File_load_from_rom()...", 1234567890, false);
				result = File_load_from_rom(load_file_name[i] + sem_lang + ".txt", fs_buffer, 0x2000, &read_size, "romfs:/gfx/msg/");
				Log_log_add(log_num, result.string, result.code, false);
				log_num = Log_log_save(sem_worker_thread_string, "Sem_load_setting()...", 1234567890, false);
				result = Sem_parse_file((char*)fs_buffer, num_of_msg[i], setting_data);
				Log_log_add(log_num, result.string, result.code, false);
				if (result.code == 0)
				{
					for (int k = 0; k < num_of_msg[i]; k++)
					{
						if (i == 0)
							Line_set_msg(k, setting_data[k]);
						else if (i == 1)
							Sem_set_msg(k, setting_data[k]);
						else if (i == 2)
							Imv_set_msg(k, setting_data[k]);
						else if (i == 3)
							Spt_set_msg(k, setting_data[k]);
						else if (i == 4)
							Gtr_set_msg(k, GTR_MSG, setting_data[k]);
						else if (i == 5)
							Cam_set_msg(k, CAM_MSG, setting_data[k]);
						else if (i == 6)
							Mup_set_msg(k, setting_data[k]);
						else if (i == 7)
							Mic_set_msg(k, setting_data[k]);
					}
				}
			}

			sem_reload_msg_request = false;
		}
		else if (sem_change_wifi_state_request)
		{
			if (sem_wifi_enabled)
			{
				result.code = Wifi_disable();
				if (result.code == 0 || result.code == (int)0xC8A06C0D)
					sem_wifi_enabled = false;
			}
			else
			{
				result.code = Wifi_enable();
				if (result.code == 0 || result.code == (int)0xC8A06C0D)
					sem_wifi_enabled = true;
			}


			sem_change_wifi_state_request= false;
		}
		else if (sem_delete_line_img_cache_request)
		{
			num_of_files = 0;
			for (int i = 0; i < 256; i++)
			{
				file_name[i] = "";
				file_type[i] = "";
			}

			log_num = Log_log_save(sem_worker_thread_string, "File_read_dir()...", 1234567890, false);
			result = File_read_dir(&num_of_files, file_name, 256, file_type, 256, "/Line/images/");
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code == 0)
			{
				for (int i = 0; i < num_of_files; i++)
				{
					if (file_type[i] == "file")
					{
						log_num = Log_log_save(sem_worker_thread_string, "File_file_delete()...", 1234567890, false);
						result = File_delete_file(file_name[i], "/Line/images/", fs_archive);
						Log_log_add(log_num, result.string, result.code, false);
					}
				}
			}
			else
			{
				Err_set_error_message(result.string, result.error_description, sem_worker_thread_string, result.code);
				Err_set_error_show_flag(true);
			}

			sem_delete_line_img_cache_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);
	}

	Log_log_save(sem_worker_thread_string , "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Sem_check_update_thread(void* arg)
{
	Log_log_save(sem_check_update_string, "Thread started.", 1234567890, false);

	u8* httpc_buffer;
	u32 downloaded_size;
	u32 status_code;
	u32 write_size;
	int check_update_log_num_return;
	int newest_ver;
	size_t parse_start_pos;
	size_t parse_end_pos;
	std::string url;
	std::string last_url;
	std::string parse_cache;
	std::string parse_start[11] = {"<newest>", "<3dsx_available>", "<cia_32mb_ram_available>",
	"<cia_64mb_ram_available>", "<cia_72mb_ram_available>", "<cia_80mb_ram_available>",
	"<cia_96mb_ram_available>", "<cia_124mb_ram_available>", "<cia_178mb_ram_available>",
	"<gas_ver>", "<patch_note>", };
	std::string parse_end[11] = { "</newest>", "</3dsx_available>", "</cia_32mb_ram_available>",
	"</cia_64mb_ram_available>", "</cia_72mb_ram_available>", "</cia_80mb_ram_available>",
	"</cia_96mb_ram_available>", "</cia_124mb_ram_available>", "</cia_178mb_ram_available>",
	"</gas_ver>", "</patch_note>", };
	std::string editions[8] = { ".3dsx", "_32mb.cia", "_64mb.cia", "_72mb.cia",
	"_80mb.cia", "_96mb.cia", "_124mb.cia", "_178mb.cia", };
	FS_Archive check_update_fs_archive = 0;
	Handle check_update_fs_handle = 0;
	Handle check_update_am_handle = 0;
	Result_with_string check_update_result;

	while (sem_check_update_thread_run)
	{
		if (sem_check_update_request || sem_dl_file_request)
		{
			if (sem_check_update_request)
			{
				url = "https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/version/newest.txt";
				sem_check_update_progress = 0;
				new_version_available = false;
				for(int i = 0; i < 8; i++)
					sem_available_ver[i] = false;
				for (int i = 0; i < 11; i++)
					sem_newest_ver_data[i] = "";
			}
			else if (sem_dl_file_request)
			{
				url = "https://raw.githubusercontent.com/Core-2-Extreme/Line_for_3DS/master/version/Line_for_3DS" + editions[sem_selected_edition_num];
				sem_update_progress = 0;
			}

			newest_ver = -1;
			httpc_buffer = (u8*)malloc(0x300000);
			if (httpc_buffer == NULL)
			{
				Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), sem_check_update_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(sem_check_update_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
			}
			else
			{
				check_update_log_num_return = Log_log_save(sem_check_update_string, "Httpc_dl_data()...", 1234567890, false);
				check_update_result = Httpc_dl_data(url, httpc_buffer, 0x300000, &downloaded_size, &status_code, true, &last_url, false, 100, SEM_HTTP_PORT0);
				Log_log_add(check_update_log_num_return, check_update_result.string + std::to_string(downloaded_size / 1024) + "KB (" + std::to_string(downloaded_size) + "B)", check_update_result.code, false);

				if (check_update_result.code != 0)
				{
					Err_set_error_message(check_update_result.string, check_update_result.error_description, sem_check_update_string, check_update_result.code);
					Err_set_error_show_flag(true);

					if (sem_check_update_request)
						sem_check_update_progress = 2;
					else if (sem_dl_file_request)
						sem_update_progress = 3;
				}
				else
				{
					if (sem_check_update_request)
					{
						parse_cache = (char*)httpc_buffer;

						for (int i = 0; i < 11; i++)
						{
							parse_start_pos = std::string::npos;
							parse_end_pos = std::string::npos;
							parse_start_pos = parse_cache.find(parse_start[i]);
							parse_end_pos = parse_cache.find(parse_end[i]);

							parse_start_pos += parse_start[i].length();
							parse_end_pos -= parse_start_pos;
							if (parse_start_pos != std::string::npos && parse_end_pos != std::string::npos)
								sem_newest_ver_data[i] = parse_cache.substr(parse_start_pos, parse_end_pos);
							else
							{
								sem_check_update_progress = 2;
								break;
							}

							if (i == 0)
								newest_ver = stoi(sem_newest_ver_data[i]);
							else if (i > 0 && i < 8)
								sem_available_ver[i - 1] = stoi(sem_newest_ver_data[i]);
							else if (i == 9)
							{
								if (sem_current_gas_ver == stoi(sem_newest_ver_data[i]))
									need_gas_update = false;
								else
									need_gas_update = true;
							}
						}

						if (sem_current_app_ver < newest_ver)
							new_version_available = true;
						else
							new_version_available = false;

						sem_check_update_progress = 1;
					}
					else if (sem_dl_file_request)
					{
						sem_update_progress = 1;
						if (sem_selected_edition_num == 0)
						{
							check_update_log_num_return = Log_log_save(sem_check_update_string, "File_save_to_file()...", 1234567890, false);
							check_update_result = File_save_to_file("Line_for_3DS.3dsx", (u8*)httpc_buffer, downloaded_size, "/Line/ver_" + sem_newest_ver_data[0] + "/", true, check_update_fs_handle, check_update_fs_archive);
							Log_log_add(check_update_log_num_return, check_update_result.string, check_update_result.code, false);
							if (check_update_result.code == 0)
								sem_update_progress = 2;
							else
								sem_update_progress = 3;
						}

						if (sem_selected_edition_num > 0 && sem_selected_edition_num < 8)
						{
							check_update_result.code = AM_StartCiaInstall(MEDIATYPE_SD, &check_update_am_handle);
							check_update_log_num_return = Log_log_save(sem_check_update_string, "AM_StartCiaInstall()...", check_update_result.code, false);

							check_update_result.code = FSFILE_Write(check_update_am_handle, &write_size, 0, (u8*)httpc_buffer, downloaded_size, FS_WRITE_FLUSH);
							check_update_log_num_return = Log_log_save(sem_check_update_string, "FSFILE_Write()...", check_update_result.code, false);

							check_update_result.code = AM_FinishCiaInstall(check_update_am_handle);
							check_update_log_num_return = Log_log_save(sem_check_update_string, "AM_FinishCiaInstall()...", check_update_result.code, false);
							if (check_update_result.code == 0)
								sem_update_progress = 2;
							else
								sem_update_progress = 3;
						}
					}
				}
			}

			free(httpc_buffer);

			if(sem_check_update_request)
				sem_check_update_request = false;
			else if(sem_dl_file_request)
				sem_dl_file_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (sem_thread_suspend)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}
	Log_log_save(sem_check_update_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

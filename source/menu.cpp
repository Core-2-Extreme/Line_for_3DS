#include <3ds.h>
#include <string>
#include <unistd.h>

#include "change_setting.hpp"
#include "draw.hpp"
#include "error.hpp"
#include "hid.hpp"
#include "httpc.hpp"
#include "log.hpp"
#include "types.hpp"

#include "menu.hpp"

#include "camera.hpp"
#include "google_translation.hpp"
#include "image_viewer.hpp"
#include "line.hpp"
#include "mic.hpp"
#include "music_player.hpp"
#include "setting_menu.hpp"
#include "speedtest.hpp"

/*For draw*/
bool menu_need_reflesh = true;
/*---------------------------------------------*/

bool menu_check_connectivity_thread_run = false;
bool menu_update_thread_run = false;
bool menu_change_brightness_request = false;
bool menu_jump_to_line_request = false;
bool menu_jump_to_gtr_request = false;
bool menu_jump_to_spt_request = false;
bool menu_jump_to_imv_request = false;
bool menu_jump_to_cam_request = false;
bool menu_jump_to_mic_request = false;
bool menu_jump_to_mup_request = false;
bool menu_jump_to_sem_request = false;
bool menu_destroy_line_request = false;
bool menu_destroy_gtr_request = false;
bool menu_destroy_spt_request = false;
bool menu_destroy_imv_request = false;
bool menu_destroy_cam_request = false;
bool menu_destroy_mic_request = false;
bool menu_destroy_mup_request = false;
bool menu_connect_test_succes = false;
bool menu_mcu_success = false;
bool menu_main_run = true;
bool menu_must_exit = false;
bool menu_check_exit_request = false;
u8* menu_wifi_state;
u8* menu_wifi_state_internet_sample;
u8 menu_wifi_signal = -1;
u8 menu_battery_charge = -1;
int menu_cam_fps = 0;
int menu_hours = -1;
int menu_minutes = -1;
int menu_seconds = -1;
int menu_days = -1;
int menu_months = -1;
int menu_years = -1;
int menu_fps = 0;
int menu_free_ram = 0;
int menu_free_linear_ram = 0;
int menu_afk_time;
int menu_battery_level_raw = 0;
char menu_status[128];
char menu_status_short[64];
std::string menu_clipboard = "";
std::string menu_battery_level = "?";
std::string menu_update_thread_string = "Menu/Update thread";
std::string menu_send_app_info_thread_string = "Menu/Send app info thread";
std::string menu_check_connection_thread_string = "Menu/Check connection thread";
std::string menu_init_string = "Menu/Init";
std::string menu_exit_string = "Menu/Exit";
std::string menu_app_ver = "v1.6.0";

Thread menu_update_thread, menu_send_app_info_thread, menu_check_connectivity_thread;

bool Menu_query_running_flag(void)
{
	return menu_main_run;
}

int Menu_query_afk_time(void)
{
	return menu_afk_time;
}

bool Menu_query_battery_charge(void)
{
	return (menu_battery_charge == 1);
}

std::string Menu_query_battery_level(void)
{
	return menu_battery_level;
}

int Menu_query_battery_level_raw(void)
{
	return menu_battery_level_raw;
}

std::string Menu_query_clipboard(void)
{
	return menu_clipboard;
}

int Menu_query_free_ram(void)
{
	return menu_free_ram;
}

int Menu_query_free_linear_ram(void)
{
	return menu_free_linear_ram;
}

bool Menu_query_must_exit_flag(void)
{
	return menu_must_exit;
}

std::string Menu_query_status(bool only_system_state)
{
	if(only_system_state)
		return menu_status_short;
	else
		return menu_status;
}

std::string Menu_query_time(int mode)
{
	char time[128];
	std::string return_time;

	if(mode == 0)
		sprintf(time, "%04d_%02d_%02d_%02d_%02d_%02d", menu_years, menu_months, menu_days, menu_hours, menu_minutes, menu_seconds);
	else if(mode == 1)
		sprintf(time, "%04d_%02d_%02d", menu_years, menu_months, menu_days);
	else if(mode == 2)
		sprintf(time, "%02d_%02d_%02d", menu_hours, menu_minutes, menu_seconds);

	return_time = time;
	return return_time;
}

std::string Menu_query_ver(void)
{
	return menu_app_ver;
}

int Menu_query_wifi_state(void)
{
	return (int)menu_wifi_signal;
}

void Menu_reset_afk_time(void)
{
		menu_afk_time = 0;
}

void Menu_set_clipboard(std::string data)
{
	menu_clipboard = data;
}

void Menu_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == MENU_JUMP_TO_LINE_REQUEST)
		menu_jump_to_line_request = flag;
	else if (operation_num == MENU_JUMP_TO_GTR_REQUEST)
		menu_jump_to_gtr_request = flag;
	else if (operation_num == MENU_JUMP_TO_SPT_REQUEST)
		menu_jump_to_spt_request = flag;
	else if (operation_num == MENU_JUMP_TO_IMV_REQUEST)
		menu_jump_to_imv_request = flag;
	else if (operation_num == MENU_JUMP_TO_CAM_REQUEST)
		menu_jump_to_cam_request = flag;
	else if (operation_num == MENU_JUMP_TO_MIC_REQUEST)
		menu_jump_to_mic_request = flag;
	else if (operation_num == MENU_JUMP_TO_MUP_REQUEST)
		menu_jump_to_mup_request = flag;
	else if (operation_num == MENU_JUMP_TO_SEM_REQUEST)
		menu_jump_to_sem_request = flag;
	else if (operation_num == MENU_DESTROY_LINE_REQUEST)
		menu_destroy_line_request = flag;
	else if (operation_num == MENU_DESTROY_GTR_REQUEST)
		menu_destroy_gtr_request = flag;
	else if (operation_num == MENU_DESTROY_SPT_REQUEST)
		menu_destroy_spt_request = flag;
	else if (operation_num == MENU_DESTROY_IMV_REQUEST)
		menu_destroy_imv_request = flag;
	else if (operation_num == MENU_DESTROY_CAM_REQUEST)
		menu_destroy_cam_request = flag;
	else if (operation_num == MENU_DESTROY_MIC_REQUEST)
		menu_destroy_mic_request = flag;
	else if (operation_num == MENU_DESTROY_MUP_REQUEST)
		menu_destroy_mup_request = flag;
	else if (operation_num == MENU_CHANGE_BRIGHTNESS_REQUEST)
		menu_change_brightness_request = flag;
	else if (operation_num == MENU_CHECK_EXIT_REQUEST)
		menu_check_exit_request = flag;
}

void Menu_resume(void)
{
	menu_main_run = true;
	menu_need_reflesh = true;
}

void Menu_suspend(void)
{
	menu_main_run = false;
}

void Menu_init(void)
{
	Log_log_save(menu_init_string, "Initializing...", 1234567890, DEBUG);

	Draw_progress("0/0 [Menu] Starting threads...");
	menu_update_thread_run = true;
	menu_check_connectivity_thread_run = true;
	menu_update_thread = threadCreate(Menu_update_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, -1, false);
	menu_check_connectivity_thread = threadCreate(Menu_check_connectivity_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);

	if (Sem_query_settings(SEM_ALLOW_SEND_APP_INFO))
	{
		for (int i = 1; i <= 1000; i++)
		{
			if (Sem_query_settings_i(SEM_NUM_OF_APP_START) == i * 10)
			{
				menu_send_app_info_thread = threadCreate(Menu_send_app_info_thread, (void*)(""), STACKSIZE, PRIORITY_LOW, -1, true);
				break;
			}
		}
	}

	menu_wifi_state = (u8*)malloc(0x1);
	menu_wifi_state_internet_sample = (u8*)malloc(0x1);
	memset(menu_wifi_state, 0xff, 0x1);
	memset(menu_wifi_state_internet_sample, 0x2, 0x1);

	Menu_resume();
	Log_log_save(menu_init_string, "Initialized", 1234567890, DEBUG);
}

void Menu_exit(void)
{
	Log_log_save(menu_exit_string, "Exiting...", 1234567890, DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	menu_update_thread_run = false;
	menu_check_connectivity_thread_run = false;

	if (Line_query_init_flag())
		Line_exit();
	if (Gtr_query_init_flag())
		Gtr_exit();
	if (Spt_query_init_flag())
		Spt_exit();
	if (Imv_query_init_flag())
		Imv_exit();
	if (Cam_query_init_flag())
		Cam_exit();
	if (Mic_query_init_flag())
		Mic_exit();
	if (Mup_query_init_flag())
		Mup_exit();
	if (Sem_query_init_flag())
		Sem_exit();

	Draw_progress("[Menu] Exiting...");

	for (int i = 0; i < 2; i++)
	{
		log_num = Log_log_save(menu_exit_string, "threadJoin()" + std::to_string(i) + "/1...", 1234567890, DEBUG);

		if(i == 0)
			result.code = threadJoin(menu_update_thread, time_out);
		else if(i == 1)
			result.code = threadJoin(menu_check_connectivity_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, DEBUG);
		else
		{
			failed = true;
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, DEBUG);
		}
	}

	threadFree(menu_update_thread);
	threadFree(menu_check_connectivity_thread);

	if (failed)
		Log_log_save(menu_exit_string, "[Warn] Some function returned error.", 1234567890, DEBUG);

	Log_log_save(menu_exit_string, "Exited.", 1234567890, DEBUG);
}

void Menu_main(void)
{
	Hid_set_disable_flag(false);
	bool draw_close[7];
	double draw_x;
	double draw_y;
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	std::string app_name[10] = { "     Line", "    Google \n  translation", " Speed test", "Image viewer", "   Camera", "      Mic", "Music player", "Settings" };
	Menu_get_system_info();

	if (Cam_query_running_flag())
		sprintf(menu_status_short, "%04d/%02d/%02d %02d:%02d:%02d cam %dfps ", menu_years, menu_months, menu_days, menu_hours, menu_minutes, menu_seconds, menu_cam_fps);
	else
		sprintf(menu_status_short, "%04d/%02d/%02d %02d:%02d:%02d ", menu_years, menu_months, menu_days, menu_hours, menu_minutes, menu_seconds);

	sprintf(menu_status, "%dfps %.1fms %s", menu_fps,  Draw_query_frametime(), menu_status_short);

	if (menu_main_run)
	{
		if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE))
			menu_need_reflesh = true;

		if(menu_need_reflesh)
		{
			if (Sem_query_settings(SEM_NIGHT_MODE))
			{
				text_red = 1.0;
				text_green = 1.0;
				text_blue = 1.0;
				text_alpha = 0.75;
			}
			else
			{
				text_red = 0.0;
				text_green = 0.0;
				text_blue = 0.0;
				text_alpha = 1.0;
			}

			Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));
			if (Sem_query_settings(SEM_NIGHT_MODE))
				Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
			else
				Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

			Draw_top_ui();

			if (Sem_query_settings(SEM_NIGHT_MODE))
				Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
			else
				Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

			draw_x = 0.0;
			draw_y = 0.0;
			for (int i = 0; i < 7; i++)
			{
				Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 60.0, 60.0);
				Draw(app_name[i], 0, draw_x, draw_y + 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

				draw_x += 80.0;
				if (i == 3)
				{
					draw_x = 0.0;
					draw_y += 80.0;
				}
			}

			Draw_texture(Square_image, weak_aqua_tint, 0, 260.0, 180.0, 60.0, 60.0);
			Draw(app_name[7], 0, 270.0, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

			draw_close[0] = Line_query_init_flag();
			draw_close[1] = Gtr_query_init_flag();
			draw_close[2] = Spt_query_init_flag();
			draw_close[3] = Imv_query_init_flag();
			draw_close[4] = Cam_query_init_flag();
			draw_close[5] = Mic_query_init_flag();
			draw_close[6] = Mup_query_init_flag();

			draw_x = 45.0;
			draw_y = 0.0;
			for (int i = 0; i < 7; i++)
			{
				if (draw_close[i])
				{
					Draw_texture(Square_image, weak_red_tint, 0, draw_x, draw_y, 15.0, 15.0);
					Draw("X", 0, draw_x + 2.5, draw_y, 0.5, 0.5, 1.0, 0.0, 0.0, 0.5);
				}

				draw_x += 80.0;
				if (i == 3)
				{
					draw_x = 45.0;
					draw_y += 80.0;
				}
			}
			Draw_bot_ui();
			Draw_touch_pos();

			Draw_apply_draw();
			menu_need_reflesh = false;
		}
		else
			gspWaitForVBlank();

		if (menu_check_exit_request)
		{
			if (Menu_check_exit())
				menu_must_exit = true;
			else
				menu_check_exit_request = false;
		}
		else if (menu_destroy_line_request)
		{
			Hid_set_disable_flag(true);
			Line_exit();
			Hid_set_disable_flag(false);
			menu_destroy_line_request = false;
		}
		else if (menu_jump_to_line_request)
		{
			Hid_set_disable_flag(true);
			if (!Line_query_init_flag())
				Line_init();
			else
				Line_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_line_request = false;
		}
		else if (menu_destroy_gtr_request)
		{
			Hid_set_disable_flag(true);
			Gtr_exit();
			Hid_set_disable_flag(false);
			menu_destroy_gtr_request = false;
		}
		else if (menu_jump_to_gtr_request)
		{
			Hid_set_disable_flag(true);
			if (!Gtr_query_init_flag())
				Gtr_init();
			else
				Gtr_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_gtr_request = false;
		}
		else if (menu_destroy_spt_request)
		{
			Hid_set_disable_flag(true);
			Spt_exit();
			Hid_set_disable_flag(false);
			menu_destroy_spt_request = false;
		}
		else if (menu_jump_to_spt_request)
		{
			Hid_set_disable_flag(true);
			if (!Spt_query_init_flag())
				Spt_init();
			else
				Spt_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_spt_request = false;
		}
		else if (menu_destroy_imv_request)
		{
			Hid_set_disable_flag(true);
			Imv_exit();

			Hid_set_disable_flag(false);
			menu_destroy_imv_request = false;
		}
		else if (menu_jump_to_imv_request)
		{
			Hid_set_disable_flag(true);
			if (!Imv_query_init_flag())
				Imv_init();
			else
				Imv_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_imv_request = false;
		}
		else if (menu_destroy_cam_request)
		{
			Hid_set_disable_flag(true);
			Cam_exit();

			Hid_set_disable_flag(false);
			menu_destroy_cam_request = false;
		}
		else if (menu_jump_to_cam_request)
		{
			Hid_set_disable_flag(true);
			if (!Cam_query_init_flag())
				Cam_init();
			else
				Cam_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_cam_request = false;
		}
		else if (menu_destroy_mic_request)
		{
			Hid_set_disable_flag(true);
			Mic_exit();

			Hid_set_disable_flag(false);
			menu_destroy_mic_request = false;
		}
		else if (menu_jump_to_mic_request)
		{
			Hid_set_disable_flag(true);
			if (!Mic_query_init_flag())
				Mic_init();
			else
				Mic_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_mic_request = false;
		}
		else if (menu_destroy_mup_request)
		{
			Hid_set_disable_flag(true);
			Mup_exit();

			Hid_set_disable_flag(false);
			menu_destroy_mup_request = false;
		}
		else if (menu_jump_to_mup_request)
		{
			Hid_set_disable_flag(true);
			if (!Mup_query_init_flag())
				Mup_init();
			else
				Mup_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_mup_request = false;
		}
		else if (menu_jump_to_sem_request)
		{
			Hid_set_disable_flag(true);
			if (!Sem_query_init_flag())
				Sem_init();
			else
				Sem_resume();

			Hid_set_disable_flag(false);
			menu_jump_to_sem_request = false;
		}
	}
	else if (Line_query_running_flag())
		Line_main();
	else if (Gtr_query_running_flag())
		Gtr_main();
	else if (Spt_query_running_flag())
		Spt_main();
	else if (Imv_query_running_flag())
		Imv_main();
	else if (Cam_query_running_flag())
		Cam_main();
	else if (Mic_query_running_flag())
		Mic_main();
	else if (Mup_query_running_flag())
		Mup_main();
	else if (Sem_query_running_flag())
		Sem_main();

	Hid_set_disable_flag(true);
}

bool Menu_check_exit(void)
{
	while (true)
	{
		Draw_set_draw_mode(1);
		if (Sem_query_settings(SEM_NIGHT_MODE))
		{
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
			Draw("Do you want to exit this software?", 0, 90.0, 105.0, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
		}
		else
		{
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);
			Draw("Do you want to exit this software?", 0, 90.0, 105.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
		}

		Draw("A to close", 0, 130.0, 140.0, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0);
		Draw("B to back", 0, 210.0, 140.0, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);

		Draw_apply_draw();
		if (Hid_query_key_press_state(KEY_P_A))
			return true;
		else if (Hid_query_key_press_state(KEY_P_B))
			return false;
	}
}

void Menu_get_system_info(void)
{
	u8 battery_level = -1;
	Result_with_string result;

	PTMU_GetBatteryChargeState(&menu_battery_charge);//battery charge
	result.code = MCUHWC_GetBatteryLevel(&battery_level);//battery level(%)
	if(result.code == 0)
	{
		menu_battery_level_raw = battery_level;
		menu_battery_level = std::to_string(battery_level);
	}
	else
	{
		PTMU_GetBatteryLevel(&battery_level);
		if ((int)battery_level == 0)
			menu_battery_level_raw = 0;
		else if ((int)battery_level == 1)
			menu_battery_level_raw = 5;
		else if ((int)battery_level == 2)
			menu_battery_level_raw = 10;
		else if ((int)battery_level == 3)
			menu_battery_level_raw = 30;
		else if ((int)battery_level == 4)
			menu_battery_level_raw = 60;
		else if ((int)battery_level == 5)
			menu_battery_level_raw = 100;

		menu_battery_level = "?";
	}

	menu_wifi_signal = osGetWifiStrength();
	//Get wifi state from shared memory #0x1FF81067
	memcpy((void*)menu_wifi_state, (void*)0x1FF81067, 0x1);
	if (memcmp(menu_wifi_state, menu_wifi_state_internet_sample, 0x1) == 0)
	{
		if (!menu_connect_test_succes)
			menu_wifi_signal = menu_wifi_signal + 4;
	}
	else
	{
		menu_wifi_signal = 8;
		menu_connect_test_succes = false;
	}

	//Get time
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t*)&unixTime);
	menu_years = timeStruct->tm_year + 1900;
	menu_months = timeStruct->tm_mon + 1;
	menu_days = timeStruct->tm_mday;
	menu_hours = timeStruct->tm_hour;
	menu_minutes = timeStruct->tm_min;
	menu_seconds = timeStruct->tm_sec;

	if (Sem_query_settings(SEM_DEBUG_MODE))
	{
		//check free RAM
		menu_free_ram = Menu_check_free_ram();
		menu_free_linear_ram = linearSpaceFree();
	}
}

int Menu_check_free_ram(void)
{
	u8* malloc_check[2000];
	int count;

	for (int i = 0; i < 2000; i++)
		malloc_check[i] = NULL;

	for (count = 0; count < 2000; count++)
	{
		malloc_check[count] = (u8*)malloc(0x186A0);// 100KB
		if (malloc_check[count] == NULL)
			break;
	}

	for (int i = 0; i <= count; i++)
		free(malloc_check[i]);

	return count;
}

void Menu_send_app_info_thread(void* arg)
{
	Log_log_save(menu_send_app_info_thread_string, "Thread started.", 1234567890, false);
	OS_VersionBin os_ver;
	bool is_new3ds = false;
	u8* dl_data;
	u32 status_code;
	u32 downloaded_size;
	char system_ver_char[0x50] = " ";
	std::string new3ds;
	dl_data = (u8*)malloc(0x1000);

	osGetSystemVersionDataString(&os_ver, &os_ver, system_ver_char, 0x50);
	std::string system_ver = system_ver_char;
	system_ver = system_ver.substr(0, (system_ver.length() - 1));

	APT_CheckNew3DS(&is_new3ds);

	if (is_new3ds)
		new3ds = "yes";
	else
		new3ds = "no";

	std::string send_data = "{ \"app_ver\": \"" + menu_app_ver + "\",\"system_ver\" : \"" + system_ver + "\",\"start_num_of_app\" : \"" + std::to_string(Sem_query_settings_i(SEM_NUM_OF_APP_START)) + "\",\"language\" : \"" + Sem_query_lang() + "\",\"new3ds\" : \"" + new3ds + "\",\"time_to_enter_sleep\" : \"" + std::to_string(Sem_query_settings_i(SEM_TIME_TO_TURN_OFF_LCD)) + "\",\"scroll_speed\" : \"" + std::to_string(Sem_query_settings_d(SEM_SCROLL_SPEED)) + "\" }";

	Httpc_post_and_dl_data("https://script.google.com/macros/s/AKfycbyn_blFyKWXCgJr6NIF8x6ETs7CHRN5FXKYEAAIrzV6jPYcCkI/exec", (char*)send_data.c_str(), send_data.length(), dl_data, 0x1000, &downloaded_size, &status_code, true, MENU_HTTP_PORT0);
	free(dl_data);
	dl_data = NULL;

	Log_log_save(menu_send_app_info_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Menu_check_connectivity_thread(void* arg)
{
	Log_log_save(menu_check_connection_thread_string, "Thread started.", 1234567890, false);
	u8* http_buffer;
	u32 response_code = 0;
	u32 dl_size = 0;
	int count = 100;
	std::string url = "https://connectivitycheck.gstatic.com/generate_204";
	std::string last_url;
	http_buffer = (u8*)malloc(0x1000);

	while (menu_check_connectivity_thread_run)
	{
		if (count >= 100 && !(Hid_query_disable_flag()))
		{
			count = 0;
			Httpc_dl_data(url, http_buffer, 0x1000, &dl_size, &response_code, true, &last_url, true, 100, MENU_HTTP_POST_PORT0);

			if (response_code == 204)
				menu_connect_test_succes = true;
			else
				menu_connect_test_succes = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		count++;
	}
	Log_log_save(menu_check_connection_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Menu_update_thread(void* arg)
{
	Log_log_save(menu_update_thread_string, "Thread started.", 1234567890, false);
	int update_thread_count = 0;
	Result_with_string result;

	while (menu_update_thread_run)
	{
		usleep(49000);
		update_thread_count++;

		if (update_thread_count >= 20)
		{
			menu_change_brightness_request = true;
			//fps

			menu_fps = Draw_query_fps();
			menu_cam_fps = Cam_query_framerate();
			Draw_reset_fps();
			Cam_reset_framerate();
			update_thread_count = 0;
		}

		if (menu_change_brightness_request)
		{
			if (menu_afk_time > (Sem_query_settings_i(SEM_TIME_TO_TURN_OFF_LCD) + 100) * 2)
				Change_screen_state(true, true, false);
			else if (menu_afk_time >= Sem_query_settings_i(SEM_TIME_TO_TURN_OFF_LCD) * 2)
			{
				Change_screen_state(true, true, true);
				Change_brightness(true, true, Sem_query_settings_i(SEM_LCD_BRIGHTNESS_BEFORE_TURN_OFF));
			}
			else
			{
				Change_screen_state(true, true, true);
				Change_brightness(true, true, Sem_query_settings_i(SEM_LCD_BRIGHTNESS));
			}
			menu_change_brightness_request = false;
		}

		if (Sem_query_settings(SEM_FLASH_MODE))
		{
			if (Sem_query_settings(SEM_NIGHT_MODE))
				C2D_PlainImageTint(&texture_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);
			else
				C2D_PlainImageTint(&texture_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.75), true);

			Sem_set_settings(SEM_NIGHT_MODE, !Sem_query_settings(SEM_NIGHT_MODE));
		}
		menu_afk_time++;
	}
	Log_log_save(menu_update_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

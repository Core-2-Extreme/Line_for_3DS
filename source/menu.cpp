#include <3ds.h>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "draw.hpp"
#include "share_function.hpp"
#include "speedtest.hpp"
#include "image_viewer.hpp"
#include "line.hpp"
#include "setting_menu.hpp"
#include "google_translation.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "httpc.hpp"
#include "change_setting.hpp"
#include "log.hpp"
#include "types.hpp"
#include "external_font.hpp"
#include "file.hpp"

bool menu_check_connectivity_thread_run = false;
bool menu_update_thread_run = false;
bool menu_change_brightness_request = false;
bool menu_jump_to_line_request = false;
bool menu_jump_to_gtr_request = false;
bool menu_jump_to_spt_request = false;
bool menu_jump_to_imv_request = false;
bool menu_jump_to_sem_request = false;
bool menu_destroy_line_request = false;
bool menu_destroy_gtr_request = false;
bool menu_destroy_spt_request = false;
bool menu_destroy_imv_request = false;
bool menu_destroy_sem_request = false;
bool menu_main_run = true;
bool menu_must_exit = false;
bool menu_check_exit_request = false;
bool menu_enable_wifi_request = false;
bool menu_disable_wifi_request = false;

Thread menu_update_thread, menu_send_app_info_thread, menu_check_connectivity_thread;

bool Menu_query_running_flag(void)
{
	return menu_main_run;
}

bool Menu_query_must_exit_flag(void)
{
	return menu_must_exit;
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
	else if (operation_num == MENU_DESTROY_SEM_REQUEST)
		menu_destroy_sem_request = flag;
	else if (operation_num == MENU_CHANGE_BRIGHTNESS_REQUEST)
		menu_change_brightness_request = flag;
	else if (operation_num == MENU_CHECK_EXIT_REQUEST)
		menu_check_exit_request = flag;
	else if (operation_num == MENU_ENABLE_WIFI_REQUEST)
		menu_enable_wifi_request = flag;
	else if (operation_num == MENU_DISABLE_WIFI_REQUEST)
		menu_disable_wifi_request = flag;
}

void Menu_resume(void)
{
	menu_main_run = true;
}

void Menu_suspend(void)
{
	menu_main_run = false;
}


void Menu_init(void)
{
	Log_log_save("Menu/Init", "Initializing...", 1234567890, s_debug_slow);

	Draw_progress("0/0 [Menu] Starting threads...");
	menu_update_thread_run = true;
	menu_check_connectivity_thread_run = true;
	menu_update_thread = threadCreate(Menu_update_thread, (void*)(""), STACKSIZE, 0x18, -1, true);
	//menu_check_connectivity_thread = threadCreate(Menu_check_connectivity_thread, (void*)(""), STACKSIZE, 0x30, -1, true);

	if (Sem_query_settings(SEM_ALLOW_SEND_APP_INFO))
	{
		for (int i = 1; i <= 1000; i++)
		{
			if (s_num_of_app_start == i * 10)
			{
				menu_send_app_info_thread = threadCreate(Menu_send_app_info_thread, (void*)(""), STACKSIZE, 0x24, -1, true);
				break;
			}
		}
	}

	Exfont_init();

	Menu_resume();
	Log_log_save("Menu/Init", "Initialized", 1234567890, s_debug_slow);
}

void Menu_exit(void)
{
	Log_log_save("Menu/Exit", "Exiting...", 1234567890, s_debug_slow);
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
	if (Sem_query_init_flag())
		Sem_exit();

	Draw_progress("0/0 [Menu] Exiting threads...");
	log_num = Log_log_save("Menu/Exit", "Exiting thread(0/1)...", 1234567890, s_debug_slow);
	result.code = threadJoin(menu_update_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	log_num = Log_log_save("Menu/Exit", "Exiting thread(1/1)...", 1234567890, s_debug_slow);
	result.code = threadJoin(menu_check_connectivity_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	if (failed)
		Log_log_save("Menu/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Log_log_save("Menu/Exit", "Exited.", 1234567890, s_debug_slow);
}

void Menu_main(void)
{
	Hid_set_disable_flag(false);
	int log_y = Log_query_y();
	double log_x = Log_query_x();
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	std::string app_name[10] = { "Line", "  Google \ntranslation", "    Speed test", "      Image viewer", "Settings" };
	Menu_get_system_info();
	sprintf(s_status, "%dfps %.1fms %02d/%02d %02d:%02d:%02d "
		, s_fps_show, s_frame_time, s_months, s_days, s_hours, s_minutes, s_seconds);

	if (menu_main_run)
	{
		if (Sem_query_settings(SEM_NIGHT_MODE))
		{
			text_red = 1.0f;
			text_green = 1.0f;
			text_blue = 1.0f;
			text_alpha = 0.75f;
		}
		else
		{
			text_red = 0.0f;
			text_green = 0.0f;
			text_blue = 0.0f;
			text_alpha = 1.0f;
		}

		Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		Draw_texture(Square_image, black_tint, 0, 0.0, 0.0, 400.0, 15.0);
		Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
		Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
		if (s_battery_charge)
			Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
		Draw(s_status, 0, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
		Draw(s_battery_level_string, 0, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

		if (Sem_query_settings(SEM_DEBUG_MODE))
			Draw_debug_info();
		if (Log_query_log_show_flag())
		{
			for (int i = 0; i < 23; i++)
				Draw(Log_query_log(log_y + i), 0, log_x, 10.0f + (i * 10), 0.4, 0.4, 0.0, 0.5, 1.0, 1.0);
		}

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		for (int i = 0; i <= 3; i++)
			Draw_texture(Square_image, weak_aqua_tint, 0, (80.0 * i), 0.0, 60.0, 60.0);

		Draw(app_name[0], 0, 20.0, 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(app_name[1], 0, 85.0, 15.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(app_name[2], 0, 150.0, 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);
		Draw(app_name[3], 0, 215.0, 20.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

		Draw_texture(Square_image, weak_aqua_tint, 0, 260.0, 180.0, 60.0, 60.0);
		Draw(app_name[4], 0, 270.0, 205.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

		if (Line_query_init_flag())
		{
			Draw_texture(Square_image, weak_red_tint, 0, 45.0, 0.0, 15.0, 15.0);
			Draw("X", 0, 47.5, 0.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.5);
		}

		if (Gtr_query_init_flag())
		{
			Draw_texture(Square_image, weak_red_tint, 0, 125.0, 0.0, 15.0, 15.0);
			Draw("X", 0, 127.5, 0.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.5);
		}

		if (Spt_query_init_flag())
		{
			Draw_texture(Square_image, weak_red_tint, 0, 205.0, 0.0, 15.0, 15.0);
			Draw("X", 0, 207.5, 0.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.5);
		}

		if (Imv_query_init_flag())
		{
			Draw_texture(Square_image, weak_red_tint, 0, 285.0, 0.0, 15.0, 15.0);
			Draw("X", 0, 287.5, 0.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.5);
		}

		if (Sem_query_init_flag())
		{
			Draw_texture(Square_image, weak_red_tint, 0, 305.0, 180.0, 15.0, 15.0);
			Draw("X", 0, 307.5, 180.0, 0.5, 0.5, 1.0, 0.0, 0.0, 0.5);
		}

		if (Err_query_error_show_flag())
			Draw_error();

		Draw_texture(Square_image, black_tint, 0, 0.0, 225.0, 320.0, 15.0);
		Draw(s_bot_button_string[1], 0, 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);

		if (Hid_query_key_held_state(KEY_H_TOUCH))
			Draw(s_circle_string, 0, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
		s_fps += 1;

		Draw_apply_draw();
		osTickCounterUpdate(&s_tcount_frame_time);
		s_frame_time = osTickCounterRead(&s_tcount_frame_time);

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
		else if (menu_destroy_sem_request)
		{
			Hid_set_disable_flag(true);
			Sem_exit();
			Hid_set_disable_flag(false);
			menu_destroy_sem_request = false;
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
			Draw("Do you want to exit this software?", 0, 90.0, 105.0f, 0.5, 0.5, 1.0, 1.0, 1.0, 0.75);
		}
		else
		{
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);
			Draw("Do you want to exit this software?", 0, 90.0, 105.0f, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
		}

		Draw("A to close", 0, 130.0, 140.0f, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0);
		Draw("B to back", 0, 210.0, 140.0f, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);

		Draw_apply_draw();
		if (Hid_query_key_press_state(KEY_P_A))
			return true;
		else if (Hid_query_key_press_state(KEY_P_B))
			return false;
	}
}

void Menu_get_system_info(void)
{
	PTMU_GetBatteryChargeState(&s_battery_charge);//battery charge
	if (s_mcu_success)
	{
		MCUHWC_GetBatteryLevel(&s_battery_level);//battery level(%)
		s_battery_level_string = std::to_string(s_battery_level);
	}
	else
	{
		PTMU_GetBatteryLevel(&s_battery_level);

		if ((int)s_battery_level == 0)
			s_battery_level = 0;
		else if ((int)s_battery_level == 1)
			s_battery_level = 5;
		else if ((int)s_battery_level == 2)
			s_battery_level = 10;
		else if ((int)s_battery_level == 3)
			s_battery_level = 30;
		else if ((int)s_battery_level == 4)
			s_battery_level = 60;
		else if ((int)s_battery_level == 5)
			s_battery_level = 100;

		s_battery_level_string = "?";
	}

	s_wifi_signal = osGetWifiStrength();

	//Get wifi state from shared memory #0x1FF81067
	memcpy((void*)wifi_state, (void*)0x1FF81067, 0x1);
	if (memcmp(wifi_state, wifi_state_internet_sample, 0x1) == 0)
	{
		if (!s_connect_test_succes)
			s_wifi_signal = s_wifi_signal + 4;
	}
	else
	{
		s_wifi_signal = 8;
		s_connect_test_succes = false;
	}

	//Get time
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t*)&unixTime);
	s_months = timeStruct->tm_mon;
	s_months = s_months + 1;
	s_days = timeStruct->tm_mday;
	s_hours = timeStruct->tm_hour;
	s_minutes = timeStruct->tm_min;
	s_seconds = timeStruct->tm_sec;

	if (Sem_query_settings(SEM_DEBUG_MODE)) 
	{
		//check free RAM
		s_free_ram = Menu_check_free_ram();
		s_free_linear_ram = linearSpaceFree();
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
	Log_log_save("Menu/Send app info thread", "Thread started.", 1234567890, false);
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

	if (s_apt_success)
		APT_CheckNew3DS(&is_new3ds);

	if (is_new3ds)
		new3ds = "yes";
	else
		new3ds = "no";

	std::string send_data = "{ \"app_ver\": \"" + s_app_ver + "\",\"system_ver\" : \"" + system_ver + "\",\"start_num_of_app\" : \"" + std::to_string(s_num_of_app_start) + "\",\"language\" : \"" + s_setting[1] + "\",\"new3ds\" : \"" + new3ds + "\",\"time_to_enter_sleep\" : \"" + std::to_string(s_time_to_enter_afk) + "\",\"scroll_speed\" : \"" + std::to_string(s_scroll_speed) + "\" }";

	Httpc_post_and_dl_data("https://script.google.com/macros/s/AKfycbyn_blFyKWXCgJr6NIF8x6ETs7CHRN5FXKYEAAIrzV6jPYcCkI/exec", (char*)send_data.c_str(), send_data.length(), dl_data, 0x1000, &downloaded_size, &status_code, true);
	free(dl_data);

	Log_log_save("Menu/Send app info thread", "Thread exit.", 1234567890, false);
}

void Menu_check_connectivity_thread(void* arg)
{
	Log_log_save("Menu/Check connectivity thread", "Thread started.", 1234567890, false);
	u8* http_buffer;
	u32 response_code = 0;
	u32 dl_size = 0;
	int count = 100;
	std::string url = "https://connectivitycheck.gstatic.com/generate_204";
	http_buffer = (u8*)malloc(0x1000);

	while (menu_check_connectivity_thread_run)
	{
		if (count >= 100 && !(Hid_query_disable_flag()))
		{
			count = 0;
			Httpc_dl_data(url, http_buffer, 0x1000, &dl_size, &response_code, true);

			if (response_code == 204)
				s_connect_test_succes = true;
			else
				s_connect_test_succes = false;
		}
		usleep(100000);
		count++;
	}
	Log_log_save("Menu/Check connectivity thread", "Thread exit.", 1234567890, false);
}

void Menu_update_thread(void* arg)
{
	Log_log_save("Menu/Update thread", "Thread started.", 1234567890, false);
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
			s_fps_show = s_fps;
			s_fps = 0;
			update_thread_count = 0;
		}

		if (menu_change_brightness_request)
		{
			if (s_disabled_enter_afk_mode)
				Change_brightness(true, true, s_lcd_brightness);
			else
			{
				if (s_afk_time > (s_time_to_enter_afk + 100) * 2)
					Change_screen_state(true, true, false);
				else if (s_afk_time >= s_time_to_enter_afk * 2)
				{
					Change_screen_state(true, true, true);
					Change_brightness(true, true, s_afk_lcd_brightness);
				}
				else
				{
					Change_screen_state(true, true, true);
					Change_brightness(true, true, s_lcd_brightness);
				}
			}
			menu_change_brightness_request = false;
		}

		if (menu_enable_wifi_request)
		{
			result.code = Wifi_enable();
			if (result.code == 0)
				s_wifi_enabled = true;

			menu_enable_wifi_request = false;
		}
		if (menu_disable_wifi_request)
		{
			result.code = Wifi_disable();
			if (result.code == 0)
				s_wifi_enabled = false;

			menu_disable_wifi_request = false;
		}

		if (Sem_query_settings(SEM_FLASH_MODE))
		{
			if (Sem_query_settings(SEM_NIGHT_MODE))
			{
				C2D_PlainImageTint(&texture_tint, C2D_Color32f(0.0, 0.0, 0.0, 1.0), true);
				Sem_set_settings(SEM_NIGHT_MODE, false);
			}
			else
			{
				C2D_PlainImageTint(&texture_tint, C2D_Color32f(1.0, 1.0, 1.0, 0.75), true);
				Sem_set_settings(SEM_NIGHT_MODE, true);
			}
		}
		s_afk_time++;
	}
	Log_log_save("Menu/Update thread", "Thread exit.", 1234567890, false);
}

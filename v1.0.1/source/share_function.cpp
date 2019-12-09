#include <3ds.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include "change_setting.hpp"
#include "share_function.hpp"
#include "draw.hpp"

bool share_ac_success = false;
bool share_apt_success = false;
bool share_mcu_success = false;
bool share_ptmu_success = false;
bool share_fs_success = false;
bool share_httpc_success = false;
bool share_ns_success = false;
bool share_ndsp_success = false;

bool share_key_A_press = false;
bool share_key_B_press = false;
bool share_key_X_press = false;
bool share_key_Y_press = false;
bool share_key_DRIGHT_press = false;
bool share_key_DLEFT_press = false;
bool share_key_ZL_press = false;
bool share_key_ZR_press = false;
bool share_key_START_press = false;
bool share_key_SELECT_press = false;
bool share_key_touch_press = false;
bool share_key_A_held = false;
bool share_key_B_held = false;
bool share_key_X_held = false;
bool share_key_Y_held = false;
bool share_key_CPAD_UP_held = false;
bool share_key_CPAD_DOWN_held = false;
bool share_key_CPAD_RIGHT_held = false;
bool share_key_CPAD_LEFT_held = false;
bool share_key_DUP_held = false;
bool share_key_DDOWN_held = false;
bool share_key_DRIGHT_held = false;
bool share_key_DLEFT_held = false;
bool share_key_touch_held = false;

bool share_menu_main_run = true;
bool share_setting_main_run = false;
bool share_speed_test_main_run = false;
bool share_speed_test_thread_suspend = false;
bool share_speed_test_already_init = false;
bool share_speed_test_thread_run = false;
bool share_connect_test_succes = false;
bool share_connect_test_thread_run = false;
bool share_google_tr_tr_thread_run = false;
bool share_google_tr_thread_suspend = false;
bool share_google_tr_already_init = false;
bool share_google_tr_main_run = false;
bool share_update_thread_run = false;
bool share_hid_thread_run = false;
bool share_app_logs_show = false;
bool share_system_setting_menu_show = false;
bool share_line_setting_menu_show = false;
bool share_wifi_enabled = false;
bool share_disabled_enter_afk_mode = false;
bool hid_disabled = true;

bool share_line_already_init = false;
bool share_line_main_run = false;
bool share_line_thread_suspend = false;
bool line_update_detail_show = false;
bool line_frame_time_show = false;
bool line_auto_update_mode = false;
bool night_mode = false;
bool draw_vsync_mode = true;
bool line_message_select_mode = false;
int language_select_num = 0;
int message_select_num = 0;
float text_x_cache = 0.0;
float text_y_cache = 0.0;
float text_size_cache = 0.45;
float text_interval_cache = 35;
float app_log_x_cache = 0.0;


u64 share_thread_exit_time_out = 3000000000; //3s
u32 share_connect_test_response_code = 0;
u8 share_wifi_signal = -1;
u8 share_battery_level = -1;
u8 share_battery_charge = -1;
u8* wifi_state;
u8* wifi_state_internet_sample;

int share_afk_time = 0;
int share_afk_lcd_bridgeness = 0;
int share_app_log_num = 0;
int share_app_log_view_num_cache = 0;
int share_fps_show = 0;
int share_hours = -1;
int share_minutes = -1;
int share_seconds = -1;
int share_day = -1;
int share_month = -1;
int share_held_time = 0;
int share_touch_pos_x = 0;
int share_touch_pos_y = 0;
int share_touch_pos_x_before = 0;
int share_touch_pos_x_moved = 0;
int share_touch_pos_x_move_left = 0;
int share_touch_pos_y_before = 0;
int share_touch_pos_y_moved = 0;
int share_touch_pos_y_move_left = 0;

int share_app_log_view_num = 0;
int share_fps;
int share_lcd_bridgeness = 50;
int share_time_to_enter_afk = 30;
int share_sleeped_time = 0;
float share_frame_time_point[270];
float share_drawing_time_history[120];
float share_app_log_x = 0.0;
float share_drawing_time;
double share_scroll_speed = 0.5;
double share_app_up_time_ms = 0;

char share_status[100];
char share_swkb_input_text[8192];
char* share_connected_ssid;
std::string share_clipboard[10];
std::string share_app_logs[4096];
std::string share_bot_button_string = "<                \u25BD                >";
std::string share_square_string = "\u25A0";
std::string share_circle_string = "\u25CF";
std::string share_test_string = "\u25EF \u01F0 \u01F1 \u0590";
std::string share_connect_check_url = "https://connectivitycheck.gstatic.com/generate_204";
std::string share_battery_level_string = "?";
std::string share_setting[10]; //0 sorce data, 1 language, 2 normal bridgeness, 3 time to enter afk, 4 afk bridgeness
std::string share_speed_test_ver = "v1.0.0";
std::string share_google_translation_ver = "v1.0.0";
std::string share_line_ver = "v1.0.1";
std::string share_app_ver = "v1.0.1";


TickCounter share_tick_counter_up_time, share_total_frame_time;
touchPosition touch_pos;
Result share_function_result;
httpcContext share_connect_test_httpc;
Thread share_connect_test_thread;
SwkbdState share_swkb;
SwkbdLearningData share_swkb_learn_data;
SwkbdDictWord share_swkb_words[8];
SwkbdButton press_button, share_swkb_press_button;

void Share_connectivity_check_thread(void* arg)
{
	while (share_connect_test_thread_run)
	{
		httpcOpenContext(&share_connect_test_httpc, HTTPC_METHOD_GET, share_connect_check_url.c_str(), 0);
		httpcSetSSLOpt(&share_connect_test_httpc, SSLCOPT_DisableVerify);
		httpcSetKeepAlive(&share_connect_test_httpc, HTTPC_KEEPALIVE_ENABLED);
		httpcAddRequestHeaderField(&share_connect_test_httpc, "Connection", "Keep-Alive");
		httpcAddRequestHeaderField(&share_connect_test_httpc, "User-Agent", " Mozilla/5.0 (iPhone; CPU iPhone OS 7_1_2 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Version/7.0 Mobile/11D257 Safari/9537.53");
		httpcAddRequestHeaderField(&share_connect_test_httpc, "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3");
		httpcAddRequestHeaderField(&share_connect_test_httpc, "Accept-Encoding", "en,en-US;q=1,ja;q=0.9");
		httpcAddRequestHeaderField(&share_connect_test_httpc, "Accept-Language", "gzip, deflate, br");
		httpcAddRequestHeaderField(&share_connect_test_httpc, "Upgrade-Insecure-Requests", "1");
		httpcBeginRequest(&share_connect_test_httpc);
		httpcGetResponseStatusCode(&share_connect_test_httpc, &share_connect_test_response_code);
		httpcCloseContext(&share_connect_test_httpc);

		if (share_connect_test_response_code == 204)
			share_connect_test_succes = true;
		else
			share_connect_test_succes = false;

		usleep(1000000);
	}
}

void Share_get_system_info(void)
{
	PTMU_GetBatteryChargeState(&share_battery_charge);//battery charge
	if (share_mcu_success)
	{
		MCUHWC_GetBatteryLevel(&share_battery_level);//battery level(%)
		share_battery_level_string = std::to_string(share_battery_level);
	}
	else
		share_battery_level_string = "?";

	share_wifi_signal = osGetWifiStrength();

	//Get wifi state from shared memory #0x1FF81067
	memcpy((void*)wifi_state, (void*)0x1FF81067, 0x1);
	if (memcmp(wifi_state, wifi_state_internet_sample, 0x1) == 0)
	{
		if (!share_connect_test_succes)
			share_wifi_signal = share_wifi_signal + 4;
	}
	else
	{
		share_wifi_signal = 8;
		share_connect_test_succes = false;
	}
	share_function_result = ACU_GetSSID(share_connected_ssid);
	if (share_function_result != 0)
		memset(share_connected_ssid, 0x0, 0x200);

	//Get time
	time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t*)& unixTime);
	share_month = timeStruct->tm_mon;
	share_month = share_month + 1;
	share_day = timeStruct->tm_mday;
	share_hours = timeStruct->tm_hour;
	share_minutes = timeStruct->tm_min;
	share_seconds = timeStruct->tm_sec;
}

bool Share_exit_check(void)
{
	while (true)
	{
		Draw_set_draw_mode(1);
		Draw_screen_ready_to_draw(0, true, 1);

		Draw("Do you want to exit this software?", 90.0, 105.0f, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
		Draw("A to close", 130.0, 140.0f, 0.5, 0.5, 0.0, 1.0, 0.0, 1.0);
		Draw("B to back", 210.0, 140.0f, 0.5, 0.5, 1.0, 0.0, 0.0, 1.0);

		Draw_apply_draw();
		if (share_key_A_press)
		{
			return true;
		}
		else if (share_key_B_press)
		{
			return false;
		}
	}
}

void Share_update_thread(void* arg)
{
	int update_thread_count = 0;
	Share_app_log_save("Share/Thread", "Update thread start", 1234567890, true);
	while (share_update_thread_run)
	{
		usleep(100000);
		update_thread_count++;

		if (update_thread_count >= 10)
		{
			//fps
			share_fps_show = share_fps;
			share_fps = 0;
			update_thread_count = 0;
		}

		//screen control
		share_afk_time++;
		if (share_disabled_enter_afk_mode)
			Change_bridgeness(true, true, share_lcd_bridgeness);
		else
		{
			if (share_afk_time > (share_time_to_enter_afk + 100))
				Change_screen_state(true, true, false);
			else if (share_afk_time >= share_time_to_enter_afk)
			{
				Change_screen_state(true, true, true);
				Change_bridgeness(true, true, share_afk_lcd_bridgeness);
			}
			else if (share_afk_time <= (share_time_to_enter_afk - 1))
			{
				Change_screen_state(true, true, true);
				Change_bridgeness(true, true, share_lcd_bridgeness);
			}
		}
	}
	Share_app_log_save("Share/Thread", "Update thread exit", 1234567890, true);
}

void Share_app_log_draw(void)
{
	Draw_set_draw_mode(1);
	Draw_screen_ready_to_draw(0, true, 1);

	int y_i = 0;
	for (int i = share_app_log_view_num_cache; i < share_app_log_view_num_cache + 23; i++)
	{
		Draw(share_app_logs[i], 0.0f, 10.0f + (y_i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
		y_i++;
	}
	Draw_apply_draw();
}

int Share_app_log_save(std::string type, std::string text, Result result, bool draw)
{
	double time_cache;
	char app_log_cache[1024];
	osTickCounterUpdate(&share_tick_counter_up_time);
	time_cache = osTickCounterRead(&share_tick_counter_up_time);
	share_app_up_time_ms = share_app_up_time_ms + time_cache;
	time_cache = share_app_up_time_ms / 1000;

	if (result == 1234567890)
	{
		sprintf(app_log_cache, "[%.5f][%s] %s", time_cache, type.c_str(), text.c_str());
	}
	else
	{
		sprintf(app_log_cache, "[%.5f][%s] %s0x%lx", time_cache, type.c_str(), text.c_str(), result);
	}

	share_app_logs[share_app_log_num] = app_log_cache;
	share_app_log_num++;
	if (share_app_log_num >= 4096)
	{
		share_app_log_num = 0;
	}

	if (share_app_log_num < 23)
	{
		share_app_log_view_num = 0;
	}
	else
	{
		share_app_log_view_num_cache = share_app_log_num - 23;
	}

	if (draw)
	{
		Share_app_log_draw();
	}
	/*
	for (int i = 0; i <= 2; i++)
	{
		function_result = Share_save_to_file("Log.txt", app_log_cache, "/Line/", false, Share_log_fs_handle, Share_log_fs_archive);
		if (function_result == 0)
		{
			break;
		}
	}*/

	return share_app_log_num - 1;
}

void Share_app_log_add_result(int add_log_num, std::string add_text, Result result, bool draw)
{
	char app_log_add_cache[4096];
	memset(app_log_add_cache, 0x0, 4096);

	if (result != 1234567890)
	{
		sprintf(app_log_add_cache, "%s0x%lx", add_text.c_str(), result);
	}
	else
	{
		sprintf(app_log_add_cache, "%s", add_text.c_str());
	}

	if (draw)
	{
		Share_app_log_draw();
	}

	share_app_logs[add_log_num] += app_log_add_cache;
}

void Share_key_flag_reset(void)
{
	share_key_A_press = false;
	share_key_B_press = false;
	share_key_X_press = false;
	share_key_Y_press = false;
	share_key_DRIGHT_press = false;
	share_key_DLEFT_press = false;
	share_key_ZL_press = false;
	share_key_ZR_press = false;
	share_key_START_press = false;
	share_key_SELECT_press = false;
	share_key_touch_press = false;
	share_key_A_held = false;
	share_key_B_held = false;
	share_key_X_held = false;
	share_key_Y_held = false;
	share_key_CPAD_UP_held = false;
	share_key_CPAD_DOWN_held = false;
	share_key_CPAD_RIGHT_held = false;
	share_key_CPAD_LEFT_held = false;
	share_key_DUP_held = false;
	share_key_DDOWN_held = false;
	share_key_DRIGHT_held = false;
	share_key_DLEFT_held = false;
	share_key_touch_held = false;
	share_touch_pos_x = 0;
	share_touch_pos_y = 0;
}

void Share_scan_hid(void* arg)
{
	Share_app_log_save("Share/scan hid thread", "Thread started.", 1234567890, true);

	u32 kDown;
	u32 kHeld;

	while (share_hid_thread_run)
	{
		if (hid_disabled)
		{
			Share_key_flag_reset();
		}

		hidScanInput();
		hidTouchRead(&touch_pos);
		kHeld = hidKeysHeld();
		kDown = hidKeysDown();

		if (kDown & KEY_A)
		{
			share_key_A_press = true;
		}
		if (kDown & KEY_B)
		{
			share_key_B_press = true;
		}
		if (kDown & KEY_X)
		{
			//Share_function_test();
			share_key_X_press = true;
			//	bgm_thread = threadCreate(Share_bgm, (void*)(""), STACKSIZE, 0x32, -1, true);

		}
		if (kDown & KEY_Y)
		{
			share_key_Y_press = true;
		}
		if (kDown & KEY_DRIGHT)
		{
			share_key_DRIGHT_press = true;
		}
		if (kDown & KEY_DLEFT)
		{
			share_key_DLEFT_press = true;
		}
		if (kDown & KEY_SELECT)
		{
			share_key_SELECT_press = true;
		}
		if (kDown & KEY_START)
		{
			share_key_START_press = true;
		}
		if (kDown & KEY_ZL)
		{
			share_key_ZL_press = true;
		}
		if (kDown & KEY_ZR)
		{
			share_key_ZR_press = true;
		}
		if (kDown & KEY_TOUCH || kHeld & KEY_TOUCH)
		{
			if (kDown & KEY_TOUCH)
			{
				share_key_touch_press = true;
				share_touch_pos_x_before = touch_pos.px;
				share_touch_pos_y_before = touch_pos.py;
				share_touch_pos_x = touch_pos.px;
				share_touch_pos_y = touch_pos.py;
			}
			if (kHeld & KEY_TOUCH)
			{
				share_key_touch_held = true;
				share_touch_pos_x = touch_pos.px;
				share_touch_pos_y = touch_pos.py;
				share_touch_pos_x_moved = share_touch_pos_x_before - share_touch_pos_x;
				share_touch_pos_y_moved = share_touch_pos_y_before - share_touch_pos_y;
				share_touch_pos_x_before = touch_pos.px;
				share_touch_pos_y_before = touch_pos.py;
			}
		}
		else
		{
			share_touch_pos_x_moved = 0;
			share_touch_pos_y_moved = 0;
			share_touch_pos_x_before = 0;
			share_touch_pos_y_before = 0;
		}
		if (kHeld & KEY_DRIGHT)
		{
			share_key_DRIGHT_held = true;
		}
		if (kHeld & KEY_DLEFT)
		{
			share_key_DLEFT_held = true;
		}
		if (kHeld & KEY_DDOWN)
		{
			share_key_DDOWN_held = true;
		}
		if (kHeld & KEY_DUP)
		{
			share_key_DUP_held = true;
		}
		if (kHeld & KEY_A)
		{
			share_key_A_held = true;
		}
		if (kHeld & KEY_B)
		{
			share_key_B_held = true;
		}
		if (kHeld & KEY_Y)
		{
			share_key_Y_held = true;
		}
		if (kHeld & KEY_X)
		{
			share_key_X_held = true;
		}
		if (kHeld & KEY_CPAD_UP)
		{
			share_key_CPAD_UP_held = true;
		}
		if (kHeld & KEY_CPAD_DOWN)
		{
			share_key_CPAD_DOWN_held = true;
		}
		if (kHeld & KEY_CPAD_RIGHT)
		{
			share_key_CPAD_RIGHT_held = true;
		}
		if (kHeld & KEY_CPAD_LEFT)
		{
			share_key_CPAD_LEFT_held = true;
		}

		if (kHeld & KEY_X && kDown & KEY_Y)
		{
			if (share_system_setting_menu_show)
				share_system_setting_menu_show = false;
			else
				share_system_setting_menu_show = true;
		}

		if (share_setting_main_run)
		{
			if (share_key_touch_held)
			{
				if (share_touch_pos_x >= 230 && share_touch_pos_x <= 245 && share_touch_pos_y >= 30 && share_touch_pos_y <= 35)
				{
					share_lcd_bridgeness -= 1;
				}
				if (share_touch_pos_x >= 250 && share_touch_pos_x <= 265 && share_touch_pos_y >= 30 && share_touch_pos_y <= 35)
				{
					share_lcd_bridgeness += 1;
				}
				if (share_touch_pos_x >= 230 && share_touch_pos_x <= 245 && share_touch_pos_y >= 40 && share_touch_pos_y <= 50)
				{
					share_time_to_enter_afk -= 5;
				}
				if (share_touch_pos_x >= 250 && share_touch_pos_x <= 265 && share_touch_pos_y >= 40 && share_touch_pos_y <= 50)
				{
					share_time_to_enter_afk += 5;
				}
				if (share_touch_pos_x >= 230 && share_touch_pos_x <= 245 && share_touch_pos_y >= 55 && share_touch_pos_y <= 65)
				{
					share_scroll_speed -= 0.005;
				}
				if (share_touch_pos_x >= 250 && share_touch_pos_x <= 265 && share_touch_pos_y >= 55 && share_touch_pos_y <= 65)
				{
					share_scroll_speed += 0.005;
				}

				if (share_time_to_enter_afk <= 10)
				{
					share_time_to_enter_afk = 10;
				}
				if (share_scroll_speed <= 0.001)
				{
					share_scroll_speed = 0.001;
				}
				if (share_scroll_speed >= 1.0)
				{
					share_scroll_speed = 1.0;
				}

			}
		}

		if (share_system_setting_menu_show)
		{
			if (kHeld & KEY_TOUCH)
			{
				if (share_touch_pos_x > 0 && share_touch_pos_x < 300 && share_touch_pos_y > 0 && share_touch_pos_y < 30)
				{
					share_lcd_bridgeness = (share_touch_pos_x + 20) / 2;
					Change_bridgeness(true, true, share_lcd_bridgeness);
				}
			}
			if (kDown & KEY_TOUCH)
			{
				if (share_touch_pos_x > 300 && share_touch_pos_x < 320 && share_touch_pos_y > 0 && share_touch_pos_y < 30)
				{
					if (share_wifi_enabled)
					{
						share_function_result = Wifi_disable();
						Share_app_log_save("HW/LAN", "Wifi_disable();", share_function_result, false);
						if (share_function_result == 0)
							share_wifi_enabled = false;
					}
					else
					{
						share_function_result = Wifi_enable();
						Share_app_log_save("HW/LAN", "Wifi_enable();", share_function_result, false);
						if (share_function_result == 0)
						{
							share_wifi_enabled = true;
						}
					}
				}
				else if (share_touch_pos_x > 300 && share_touch_pos_x < 320 && share_touch_pos_y > 30 && share_touch_pos_y < 60)
				{
					if (share_disabled_enter_afk_mode)
					{
						share_disabled_enter_afk_mode = false;
					}
					else
					{
						share_disabled_enter_afk_mode = true;
					}
				}
			}
		}

		if (share_key_touch_press && share_line_setting_menu_show && !hid_disabled)
		{
			if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 60 && touch_pos.py < 80)
			{
				if (line_auto_update_mode)
				{
					line_auto_update_mode = false;
				}
				else
				{
					line_auto_update_mode = true;
				}
			}
			else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 100 && touch_pos.py < 120)
			{
				if (night_mode)
				{
					night_mode = false;
				}
				else
				{
					night_mode = true;
				}
			}
			else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 120 && touch_pos.py < 140)
			{
				if (line_frame_time_show)
				{
					line_frame_time_show = false;
				}
				else
				{
					line_frame_time_show = true;
				}
			}
			else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 140 && touch_pos.py < 160)
			{
				if (line_update_detail_show)
				{
					line_update_detail_show = false;
				}
				else
				{
					line_update_detail_show = true;
				}
			}
			else if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 160 && touch_pos.py < 180)
			{
				if (draw_vsync_mode)
				{
					draw_vsync_mode = false;
				}
				else
				{
					draw_vsync_mode = true;
				}
			}
			else if (touch_pos.px >= 160 && touch_pos.px <= 240 && touch_pos.py >= 80 && touch_pos.py <= 90)
				language_select_num = 0;
			else if (touch_pos.px >= 160 && touch_pos.px <= 200 && touch_pos.py >= 90 && touch_pos.py <= 100)
				language_select_num = 1;
			else if (touch_pos.px >= 160 && touch_pos.px <= 200 && touch_pos.py >= 100 && touch_pos.py <= 110)
				language_select_num = 2;
			else if (touch_pos.px >= 160 && touch_pos.px <= 200 && touch_pos.py >= 110 && touch_pos.py <= 120)
				language_select_num = 3;
		}

		if (share_line_main_run && !hid_disabled)
		{
			if (share_key_DUP_held)
			{
				if (line_message_select_mode)
				{
					message_select_num--;
					if (message_select_num < 0)
					{
						message_select_num = 0;
					}
				}
				else
				{
					text_interval_cache += 0.5;
					if (text_interval_cache > 150)
					{
						text_interval_cache = 150;
					}
				}
			}
			if (share_key_DDOWN_held)
			{
				if (line_message_select_mode)
				{
					message_select_num++;
					if (message_select_num > 299)
					{
						message_select_num = 299;
					}
				}
				else
				{
					text_interval_cache -= 0.5;
					if (text_interval_cache < 10)
					{
						text_interval_cache = 10;
					}
				}
			}
			if (share_key_touch_press)
			{
				if (touch_pos.px > 0 && touch_pos.px < 140 && touch_pos.py > 200 && touch_pos.py < 220)
				{
					if (share_setting[1] == "jp")
					{
						share_setting[1] = "en";
					}
					else
					{
						share_setting[1] = "jp";
					}
				}
				if (touch_pos.px > 280 && touch_pos.px < 300 && touch_pos.py > 40 && touch_pos.py < 60)
				{
					if (share_line_setting_menu_show)
						share_line_setting_menu_show = false;
					else
						share_line_setting_menu_show = true;
				}
			}
			if (kHeld & KEY_CPAD_UP)
			{
				if (share_app_logs_show)
				{
					share_app_log_view_num_cache--;
					if (share_app_log_view_num_cache < 0)
					{
						share_app_log_view_num_cache = 0;
					}
				}
				else
				{
					if (share_held_time > 600)
					{
						text_y_cache += 50.0f;
					}
					else if (share_held_time > 240)
					{
						text_y_cache += 10.0f;
					}
					else
					{
						text_y_cache += 5.0f;
					}
				}
			}
			if (kHeld & KEY_CPAD_DOWN)
			{
				if (share_app_logs_show)
				{
					share_app_log_view_num_cache++;
					if (share_app_log_view_num_cache > 512)
					{
						share_app_log_view_num_cache = 512;
					}
				}
				else
				{
					if (share_held_time > 600)
					{
						text_y_cache -= 50.0f;
					}
					else if (share_held_time > 240)
					{
						text_y_cache -= 10.0f;
					}
					else
					{
						text_y_cache -= 5.0f;
					}
				}
			}
			if (kHeld & KEY_CPAD_LEFT)
			{
				if (share_app_logs_show)
				{
					app_log_x_cache += 5.0f;
					if (app_log_x_cache > 0.0)
					{
						app_log_x_cache = 0.0f;
					}
				}
				else
				{
					if (share_held_time > 240)
					{
						text_x_cache += 20.0f;
					}
					else
					{
						text_x_cache += 5.0f;
					}

					if (text_x_cache > 0.0)
					{
						text_x_cache = 0.0f;
					}
				}
			}
			if (kHeld & KEY_CPAD_RIGHT)
			{
				if (share_app_logs_show)
				{
					app_log_x_cache -= 5.0f;
				}
				else
				{
					if (share_held_time > 240)
					{
						text_x_cache -= 20.0f;
					}
					else
					{
						text_x_cache -= 5.0f;
					}
				}
			}
			if (share_key_touch_held)
			{
				if (share_touch_pos_y <= 220)
				{
					if (share_touch_pos_x_moved >= 2 || share_touch_pos_x_moved <= -2)
					{
						share_touch_pos_x_move_left += share_touch_pos_x_moved;
					}
					if (share_touch_pos_y_moved >= 3 || share_touch_pos_y_moved <= -3)
					{
						share_touch_pos_y_move_left += share_touch_pos_y_moved;
					}
				}
			}
			text_x_cache -= (share_touch_pos_x_move_left * share_scroll_speed);
			text_y_cache -= (share_touch_pos_y_move_left * share_scroll_speed);
			share_touch_pos_x_move_left -= (share_touch_pos_x_move_left * 0.15);
			share_touch_pos_y_move_left -= (share_touch_pos_y_move_left * 0.15);

		}

		if (!hid_disabled)
		{


			if (kHeld & KEY_R)
			{
				text_size_cache += 0.005f;
				if (text_size_cache > 2.5)
				{
					text_size_cache = 2.5f;
				}
			}
			else if (kHeld & KEY_L)
			{
				text_size_cache -= 0.005f;
				if (text_size_cache < 0.25)
				{
					text_size_cache = 0.25f;
				}
			}
		}

		if (share_key_A_press || share_key_B_press || share_key_X_press || share_key_Y_press || share_key_DRIGHT_press
			|| share_key_DLEFT_press || share_key_ZL_press || share_key_ZR_press || share_key_START_press
			|| share_key_SELECT_press || share_key_touch_press || share_key_A_held || share_key_B_held
			|| share_key_X_held || share_key_Y_held || share_key_DDOWN_held || share_key_DRIGHT_held
			|| share_key_DLEFT_held || share_key_CPAD_UP_held || share_key_CPAD_DOWN_held || share_key_CPAD_RIGHT_held
			|| share_key_CPAD_LEFT_held || share_key_DUP_held || share_key_touch_held)
			share_afk_time = 0;
		if (share_key_CPAD_UP_held || share_key_CPAD_DOWN_held || share_key_CPAD_RIGHT_held
			|| share_key_CPAD_LEFT_held || share_key_touch_held)
			share_held_time++;
		else
			share_held_time = 0;

		usleep(16700);
	}
	Share_app_log_save("Share/Thread", "Scan hid thread exit", 1234567890, true);
}

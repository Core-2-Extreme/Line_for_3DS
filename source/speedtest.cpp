#include <string>
#include <cstring>
#include <unistd.h>
#include <3ds.h>
#include "hid.hpp"
#include "draw.hpp"
#include "speedtest.hpp"
#include "httpc.hpp"
#include "share_function.hpp"
#include "error.hpp"
#include "menu.hpp"

bool spt_already_init = false;
bool spt_start_request = false;
bool spt_thread_run = false;
bool spt_thread_suspend = false;
bool spt_main_run = false;
int spt_httpc_buffer_size = 0x700000;
int spt_data_size = 0;
int spt_total_dl_size = 0;
double spt_total_dl_time = 0.0;
double spt_test_result = 0.0;
std::string spt_message_en[12] = {
	"Downloaded size : ",
	"Download time : ",
	"Speed : ",
	"Select test data size",
	"About 1MB",
	"About 2MB",
	"About 4MB",
	"About 8MB",
	"About 15MB",
	"About 30MB",
	"About 60MB",
	"Start",
};
std::string spt_message_jp[12] = {
	"ダウンロード済み容量 : ",
	"ダウンロード時間 : ",
	"速度 : ",
	"テストデータ容量選択",
	"約1MB",
	"約2MB",
	"約4MB",
	"約8MB",
	"約15MB",
	"約30MB",
	"約60MB",
	"開始",
};

Thread spt_spt_thread;

bool Spt_query_init_flag(void)
{
	return spt_already_init;
}

bool Spt_query_running_flag(void)
{
	return spt_main_run;
}

int Spt_query_buffer_size(int buffer_num)
{
	if (buffer_num == SPT_HTTPC_BUFFER)
		return spt_httpc_buffer_size;
	else
		return -1;
}

void Spt_set_buffer_size(int buffer_num, int size)
{
	if(buffer_num == SPT_HTTPC_BUFFER)
		spt_httpc_buffer_size = size;
}

void Spt_set_spt_data_size(int size)
{
	spt_data_size = size;
}

void Spt_start_request(void)
{
	spt_start_request = true;
}

void Spt_resume(void)
{
	Menu_suspend();
	spt_thread_suspend = false;
	spt_main_run = true;
}

void Spt_suspend(void)
{
	spt_thread_suspend = true;
	spt_main_run = false;
	Menu_resume();
}

void Spt_init(void)
{
	S_log_save("Spt/Init", "Initializing...", 1234567890, s_debug_slow);

	Draw_progress("0/0 [Spt] Starting threads...");
	spt_thread_run = true;
	spt_spt_thread = threadCreate(Spt_spt_thread, (void*)(""), STACKSIZE, 0x26, -1, true);

	Spt_resume();
	spt_already_init = true;
	S_log_save("Spt/Init", "Initialized.", 1234567890, s_debug_slow);
}

void Spt_exit(void)
{
	S_log_save("Spt/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;


	Draw_progress("0/0 [Spt] Exiting threads...");
	spt_already_init = false;
	spt_thread_run = false;
	spt_thread_suspend = false;

	log_num = S_log_save("Spt/Exit", "Exiting thread(0/0)...", 1234567890, s_debug_slow);
	result.code = threadJoin(spt_spt_thread, time_out);
	if (result.code == 0)
		S_log_add(log_num, "[Success] ", result.code, s_debug_slow);
	else
	{
		failed = true;
		S_log_add(log_num, "[Error] ", result.code, s_debug_slow);
	}

	if (failed)
		S_log_save("Spt/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	S_log_save("Spt/Exit", "Exited.", 1234567890, s_debug_slow);
}

void Spt_main(void)
{
	osTickCounterUpdate(&s_tcount_frame_time);

	Draw_set_draw_mode(s_draw_vsync_mode);
	if (s_night_mode)
		Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

	Draw_texture(Square_image, black_tint, 0, 0.0, 0.0, 400.0, 15.0);
	Draw_texture(Wifi_icon_image, dammy_tint, s_wifi_signal, 360.0, 0.0, 15.0, 15.0);
	Draw_texture(Battery_level_icon_image, dammy_tint, s_battery_level / 5, 330.0, 0.0, 30.0, 15.0);
	if (s_battery_charge)
		Draw_texture(Battery_charge_icon_image, dammy_tint, 0, 310.0, 0.0, 20.0, 15.0);
	Draw(s_status, 0.0f, 0.0f, 0.45f, 0.45f, 0.0f, 1.0f, 0.0f, 1.0f);
	Draw(s_battery_level_string, 337.5f, 1.25f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.5f);

	if (s_setting[1] == "en")
	{
		Draw(spt_message_en[0] + std::to_string(spt_total_dl_size / (1024 * 1024)) + "MB(" + std::to_string(spt_total_dl_size / 1024) + "KB)", 0.0f, 20.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(spt_message_en[1] + std::to_string(spt_total_dl_time) + " ms", 0.0f, 40.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(spt_message_en[2] + std::to_string((spt_test_result / (1024 * 1024)) * 8) + "Mbps", 0.0f, 60.0f, 1.0f, 1.0f, 0.25f, 0.0f, 1.0f, 1.0f);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(spt_message_jp[0] + std::to_string(spt_total_dl_size / (1024 * 1024)) + "MB(" + std::to_string(spt_total_dl_size / 1024) + "KB)", 0.0f, 20.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(spt_message_jp[1] + std::to_string(spt_total_dl_time) + " ms", 0.0f, 40.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
		Draw(spt_message_jp[2] + std::to_string((spt_test_result / (1024 * 1024)) * 8) + "Mbps", 0.0f, 60.0f, 1.0f, 1.0f, 0.25f, 0.0f, 1.0f, 1.0f);
	}

	if (s_debug_mode)
		Draw_debug_info();
	if (s_app_logs_show)
	{
		for (int i = 0; i < 23; i++)
			Draw(s_app_logs[s_app_log_view_num + i], s_app_log_x, 10.0f + (i * 10), 0.4f, 0.4f, 0.0f, 0.5f, 1.0f, 1.0f);
	}

	if (s_night_mode)
		Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

	Draw(s_spt_ver, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);

	if (s_setting[1] == "en")
	{
		Draw(spt_message_en[3], 70.0, 10.0, 0.75, 0.75, 0.0, 0.0, 0.0, 1.0);
		for (int i = 0; i < 7; i++)
		{
			if (spt_data_size == i)
				Draw(spt_message_en[4 + i], 125.0, 40.0 + (i * 20), 0.5, 0.5, 1.0, 0.0, 0.5, 1.0);
			else
				Draw(spt_message_en[4 + i], 125.0, 40.0 + (i * 20), 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
		}
		Draw(spt_message_en[11], 150.0, 190.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	}
	else if (s_setting[1] == "jp")
	{
		Draw(spt_message_jp[3], 75.0, 10.0, 0.75, 0.75, 0.0, 0.0, 0.0, 1.0);
		for (int i = 0; i < 7; i++)
		{
			if (spt_data_size == i)
				Draw(spt_message_jp[4 + i], 135.0, 40.0 + (i * 20), 0.5, 0.5, 1.0, 0.0, 0.5, 1.0);
			else
				Draw(spt_message_jp[4 + i], 135.0, 40.0 + (i * 20), 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
		}
		Draw(spt_message_jp[11], 150.0, 190.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
	}

	if (Err_query_error_show_flag())
		Draw_error();

	Draw_texture(Square_image, black_tint, 0, 0.0, 225.0, 320.0, 15.0);
	Draw(s_bot_button_string[1], 30.0f, 220.0f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 1.0f);
	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw(s_circle_string, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);

	Draw_apply_draw();
	s_fps += 1;
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);
}

void Spt_spt_thread(void* arg)
{
	S_log_save("Spt/Spt thread", "Thread started.", 1234567890, false);

	u8* httpc_buffer;
	u32 dl_size;
	u32 status_code;
	int log_num;
	float dl_time;
	std::string url[8] = { "http://v2.musen-lan.com/flash/test_001.swf", "http://v2.musen-lan.com/flash/test_002.swf", "http://v2.musen-lan.com/flash/test_004.swf", "http://v2.musen-lan.com/flash/test_008.swf", "http://v2.musen-lan.com/flash/test_016.swf", "http://v2.musen-lan.com/flash/test_032.swf", "http://v2.musen-lan.com/flash/test_064.swf", "http://v2.musen-lan.com/flash/test_128.swf" };
	Result_with_string result;
	TickCounter timer;

	while (spt_thread_run)
	{
		if (spt_start_request)
		{
			dl_size = 0;
			status_code = 0;
			spt_total_dl_time = 0;
			spt_total_dl_size = 0;
			httpc_buffer = (u8*)malloc(spt_httpc_buffer_size);

			if (httpc_buffer == NULL)
			{
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate 'httpc buffer'(" + std::to_string(spt_httpc_buffer_size / 1024) + "KB). ", "Spt/Spt thread", OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				S_log_save("Spt/Spt thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
			}
			else
			{
				for (int i = 0; i <= 9; i++)
				{
					dl_time = 0.0000001;
					log_num = S_log_save("Spt/Spt thread/httpc", "Downloading test data(" + std::to_string(i) + "/9)...", 1234567890, false);
					osTickCounterStart(&timer);
					result = Httpc_dl_data(url[spt_data_size], httpc_buffer, spt_httpc_buffer_size, &dl_size, &status_code, true);
					osTickCounterUpdate(&timer);
					dl_time = osTickCounterRead(&timer);
					S_log_add(log_num, result.string, result.code, false);

					spt_total_dl_size += dl_size;
					spt_total_dl_time += dl_time;
					spt_test_result = spt_total_dl_size / (spt_total_dl_time / 1000);
					if (result.code != 0)
					{
						Err_set_error_message(result.string, result.error_description, "Spt/Spt thread/httpc", result.code);
						Err_set_error_show_flag(true);
						break;
					}
				}
			}

			free(httpc_buffer);
			httpc_buffer = NULL;
			spt_start_request = false;
		}
		usleep(100000);

		while (spt_thread_suspend)
			usleep(250000);
	}
	S_log_save("Spt/Spt thread", "Thread exit.", 1234567890, false);
}

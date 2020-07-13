#include <3ds.h>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "draw.hpp"
#include "speedtest.hpp"
#include "httpc.hpp"
#include "share_function.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "types.hpp"
#include "setting_menu.hpp"

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
std::string spt_msg[SPT_NUM_OF_MSG];
std::string spt_ver = "v1.0.4";

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

void Spt_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < SPT_NUM_OF_MSG)
		spt_msg[msg_num] = msg;
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
	Log_log_save("Spt/Init", "Initializing...", 1234567890, s_debug_slow);

	Draw_progress("0/0 [Spt] Starting threads...");
	spt_thread_run = true;
	spt_spt_thread = threadCreate(Spt_spt_thread, (void*)(""), STACKSIZE, 0x26, -1, false);

	Spt_resume();
	spt_already_init = true;
	Log_log_save("Spt/Init", "Initialized.", 1234567890, s_debug_slow);
}

void Spt_exit(void)
{
	Log_log_save("Spt/Exit", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;


	Draw_progress("[Spt] Exiting...");
	spt_already_init = false;
	spt_thread_run = false;
	spt_thread_suspend = false;

	log_num = Log_log_save("Spt/Exit", "Exiting thread(0/0)...", 1234567890, s_debug_slow);
	result.code = threadJoin(spt_spt_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_general_success_string(), result.code, s_debug_slow);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_general_error_string(), result.code, s_debug_slow);
	}

	threadFree(spt_spt_thread);

	if (failed)
		Log_log_save("Spt/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);

	Log_log_save("Spt/Exit", "Exited.", 1234567890, s_debug_slow);
}

void Spt_main(void)
{
	int log_y = Log_query_y();
	double log_x = Log_query_x();
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;

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
	Draw(spt_msg[0] + std::to_string(spt_total_dl_size / (1024 * 1024)) + "MB(" + std::to_string(spt_total_dl_size / 1024) + "KB)", 0, 0.0f, 20.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
	Draw(spt_msg[1] + std::to_string(spt_total_dl_time) + " ms", 0, 0.0f, 40.0f, 0.75f, 0.75f, 0.25f, 0.0f, 1.0f, 1.0f);
	Draw(spt_msg[2] + std::to_string((spt_test_result / (1024 * 1024)) * 8) + "Mbps", 0, 0.0f, 60.0f, 1.0f, 1.0f, 0.25f, 0.0f, 1.0f, 1.0f);

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

	Draw(spt_ver, 0, 0.0, 0.0, 0.45, 0.45, 0.0, 1.0, 0.0, 1.0);

	Draw(spt_msg[3], 0, 70.0, 10.0, 0.75, 0.75, 0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < 7; i++)
	{
		Draw_texture(Square_image, yellow_tint, 0, 100.0, 40.0 + (i * 20.0), 130.0, 20.0);

		if (spt_data_size == i)
			Draw(spt_msg[4 + i], 0, 125.0, 40.0 + (i * 20.0), 0.5, 0.5, 1.0, 0.0, 0.5, 1.0);
		else
			Draw(spt_msg[4 + i], 0, 125.0, 40.0 + (i * 20.0), 0.5, 0.5, 0.0, 1.0, 1.0, 1.0);
	}

	Draw_texture(Square_image, weak_red_tint, 0, 150.0, 190.0 , 40.0, 20.0);
	Draw(spt_msg[11], 0, 150.0, 190.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);

	if (Err_query_error_show_flag())
		Draw_error();

	Draw_bot_ui();
	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw_touch_pos();

	Draw_apply_draw();
}

void Spt_spt_thread(void* arg)
{
	Log_log_save("Spt/Spt thread", "Thread started.", 1234567890, false);

	u8* httpc_buffer;
	u32 dl_size;
	u32 status_code;
	int log_num;
	float dl_time;
	std::string last_url;
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
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate memory.", "Spt/Spt thread", OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save("Spt/Spt thread", "[Error] Out of memory. ", OUT_OF_MEMORY, false);
			}
			else
			{
				for (int i = 0; i <= 9; i++)
				{
					dl_time = 0.0000001;
					log_num = Log_log_save("Spt/Spt thread/httpc", "Downloading test data(" + std::to_string(i) + "/9)...", 1234567890, false);
					osTickCounterStart(&timer);
					result = Httpc_dl_data(url[spt_data_size], httpc_buffer, spt_httpc_buffer_size, &dl_size, &status_code, true, &last_url, false, 100);
					osTickCounterUpdate(&timer);
					dl_time = osTickCounterRead(&timer);
					Log_log_add(log_num, result.string, result.code, false);

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
	Log_log_save("Spt/Spt thread", "Thread exit.", 1234567890, false);
	threadExit(0);
}

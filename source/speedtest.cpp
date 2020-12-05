#include <3ds.h>
#include <string>
#include <unistd.h>

#include "hid.hpp"
#include "draw.hpp"
#include "speedtest.hpp"
#include "httpc.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "log.hpp"
#include "types.hpp"
#include "setting_menu.hpp"

/*For draw*/
bool spt_need_reflesh = false;
int spt_pre_data_size = 0;
std::string spt_pre_text[16];
/*---------------------------------------------*/

bool spt_already_init = false;
bool spt_start_request = false;
bool spt_count_request = false;
bool spt_count_reset_request = false;
bool spt_thread_run = false;
bool spt_thread_suspend = false;
bool spt_main_run = false;
int spt_httpc_buffer_size = 0x700000;
int spt_data_size = 0;
int spt_total_dl_size = 0;
double spt_total_dl_time = 0.0;
std::string spt_msg[SPT_NUM_OF_MSG];
std::string spt_text[16];
std::string spt_spt_thread_string = "Spt/Spt thread";
std::string spt_timer_thread_string = "Spt/Timer thread";
std::string spt_init_string = "Spt/Init";
std::string spt_exit_string = "Spt/Exit";
std::string spt_ver = "v1.0.6";

Thread spt_spt_thread, spt_timer_thread;

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

void Spt_reset_data(void)
{
	Spt_reset_draw_data();
	spt_data_size = 0;
	spt_total_dl_size = 0;
	spt_total_dl_time = 0.0;
}

void Spt_reset_draw_data(void)
{
	for(int i = 0; i < 16; i++)
	{
		spt_text[i] = "";
		spt_pre_text[i] = "";
	}
	spt_pre_data_size = -1;
}

void Spt_resume(void)
{
	Spt_reset_draw_data();
	spt_thread_suspend = false;
	spt_main_run = true;
	spt_need_reflesh = true;
	Menu_suspend();
}

void Spt_suspend(void)
{
	spt_thread_suspend = true;
	spt_main_run = false;
	Menu_resume();
}

void Spt_init(void)
{
	Log_log_save(spt_init_string, "Initializing...", 1234567890, FORCE_DEBUG);

	Draw_progress("0/0 [Spt] Starting threads...");
	spt_thread_run = true;
	spt_spt_thread = threadCreate(Spt_spt_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
	spt_timer_thread = threadCreate(Spt_timer_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, 1, false);

 	Spt_reset_data();
	Spt_resume();
	spt_already_init = true;
	Log_log_save(spt_init_string, "Initialized.", 1234567890, FORCE_DEBUG);
}

void Spt_exit(void)
{
	Log_log_save(spt_exit_string, "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;


	Draw_progress("[Spt] Exiting...");
	spt_already_init = false;
	spt_thread_run = false;
	spt_thread_suspend = false;

	log_num = Log_log_save(spt_exit_string, "threadJoin()0/1...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(spt_spt_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	log_num = Log_log_save(spt_exit_string, "threadJoin()1/1...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(spt_timer_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
	{
		failed = true;
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	threadFree(spt_spt_thread);
	threadFree(spt_timer_thread);

	if (failed)
		Log_log_save(spt_exit_string, "[Warn] Some function returned error.", 1234567890, FORCE_DEBUG);

	Log_log_save(spt_exit_string, "Exited.", 1234567890, FORCE_DEBUG);
}

void Spt_main(void)
{
	double size[3] = { 0.5, 0.5, 0.75, };
	float r[2], g[2], b[2], a[2];
	Hid_info key;

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		r[0] = 1.0;
		g[0] = 1.0;
		b[0] = 1.0;
		a[0] = 0.75;
	}
	else
	{
		r[0] = 0.0;
		g[0] = 0.0;
		b[0] = 0.0;
		a[0] = 1.0;
	}

	spt_text[0] = spt_msg[0] + std::to_string(spt_total_dl_size / (1024 * 1024)) + "MB(" + std::to_string(spt_total_dl_size / 1024) + "KB)";
	spt_text[1] = spt_msg[1] + std::to_string(spt_total_dl_time).substr(0, std::to_string(spt_total_dl_time).length() - 3) + " ms";
	if(spt_total_dl_time != 0.0)
		spt_text[2] =  spt_msg[2] + std::to_string(((spt_total_dl_size / (spt_total_dl_time / 1000.0)) / (1024 * 1024)) * 8) + "Mbps";

	for(int i = 0; i < 3; i++)
	{
		if(spt_pre_text[i] != spt_text[i])
		{
			spt_need_reflesh = true;
			break;
		}
	}

	if(spt_need_reflesh || spt_pre_data_size != spt_data_size)
	{
		for(int i = 0; i < 3; i++)
			spt_pre_text[i] = spt_text[i];

		spt_pre_data_size = spt_data_size;
		spt_need_reflesh = true;
	}

	Hid_query_key_state(&key);
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE))
		spt_need_reflesh = true;

	Hid_key_flag_reset();

	if(spt_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		for(int i = 0; i < 3; i++)
			Draw(spt_text[i], 0, 0.0, 20.0 + (i * 20.0), size[i], size[i], 0.25, 0.0, 1.0, 1.0);

		Draw_top_ui();

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
			{
				r[1] = 1.0;
				g[1] = 0.0;
				b[1] = 0.5;
			}
			else
			{
				r[1] = 0.0;
				g[1] = 1.0;
				b[1] = 1.0;
			}

			if(spt_start_request && spt_data_size != i)
				a[1] = 0.25;
			else
				a[1] = 1.0;

			Draw(spt_msg[4 + i], 0, 125.0, 40.0 + (i * 20.0), 0.5, 0.5, r[1], g[1], b[1], a[1]);
		}
		Draw_texture(Square_image, weak_red_tint, 0, 150.0, 190.0 , 40.0, 20.0);
		Draw(spt_msg[11], 0, 150.0, 190.0, 0.5, 0.5, r[0], g[0], b[0], a[0]);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		spt_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	if (Err_query_error_show_flag())
	{
		if (key.p_touch && key.touch_x >= 150 && key.touch_x <= 170 && key.touch_y >= 150 && key.touch_y < 170)
			Err_set_error_show_flag(false);
	}
	else
	{
		if (key.p_start || (key.p_touch && key.touch_x >= 110 && key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Spt_suspend();
		else if (key.p_a || (key.p_touch && key.touch_x >= 150 && key.touch_x <= 189 && key.touch_y >= 190 && key.touch_y <= 209))
			spt_start_request = true;
		else if (key.p_touch)
		{
			for (int i = 0; i < 7; i++)
			{
				if (!spt_start_request && key.touch_x >= 100 && key.touch_x <= 230 && key.touch_y >= 40 + (i * 20) && key.touch_y <= 59 + (i * 20))
				{
					spt_data_size = i;
					break;
				}
			}
		}
	}
}

void Spt_timer_thread(void* arg)
{
	Log_log_save(spt_spt_thread_string, "Thread started.", 1234567890, false);

	TickCounter timer;
	osTickCounterStart(&timer);

	while (spt_thread_run)
	{
		while (spt_count_request)
		{
			if(spt_count_reset_request)
				break;

			osTickCounterUpdate(&timer);
			spt_total_dl_time += osTickCounterRead(&timer);
			usleep(41500);
		}

		if (spt_count_reset_request)
		{
			osTickCounterUpdate(&timer);
			spt_total_dl_time = 0.000001;
			spt_count_reset_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (spt_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	Log_log_save(spt_spt_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Spt_spt_thread(void* arg)
{
	Log_log_save(spt_spt_thread_string, "Thread started.", 1234567890, false);

	u8* httpc_buffer = NULL;
	u32 dl_size = 0;
	u32 status_code = 0;
	int log_num = 0;
	std::string last_url;
	std::string url[8] = { "http://v2.musen-lan.com/flash/test_001.swf", "http://v2.musen-lan.com/flash/test_002.swf", "http://v2.musen-lan.com/flash/test_004.swf", "http://v2.musen-lan.com/flash/test_008.swf", "http://v2.musen-lan.com/flash/test_016.swf", "http://v2.musen-lan.com/flash/test_032.swf", "http://v2.musen-lan.com/flash/test_064.swf", "http://v2.musen-lan.com/flash/test_128.swf" };
	Result_with_string result;

	while (spt_thread_run)
	{
		if (spt_start_request)
		{
			dl_size = 0;
			status_code = 0;
			spt_count_reset_request = true;
			spt_total_dl_size = 0;
			httpc_buffer = (u8*)malloc(spt_httpc_buffer_size);

			if (httpc_buffer == NULL)
			{
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate memory.", spt_spt_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(spt_spt_thread_string, "[Error] Out of memory. ", OUT_OF_MEMORY, false);
			}
			else
			{
				for (int i = 0; i <= 9; i++)
				{
					log_num = Log_log_save(spt_spt_thread_string, "Httpc_dl_data()" + std::to_string(i) + "/9...", 1234567890, false);
					spt_count_request = true;
					result = Httpc_dl_data(url[spt_data_size], httpc_buffer, spt_httpc_buffer_size, &dl_size, &status_code, true, &last_url, false, 100, SPT_HTTP_PORT0);
					spt_count_request = false;

					Log_log_add(log_num, result.string, result.code, false);

					spt_total_dl_size += dl_size;
					if (result.code != 0)
					{
						Err_set_error_message(result.string, result.error_description, spt_spt_thread_string, result.code);
						Err_set_error_show_flag(true);
						break;
					}
				}
			}

			free(httpc_buffer);
			httpc_buffer = NULL;
			spt_start_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (spt_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	Log_log_save(spt_spt_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

#include "definitions.hpp"
#include "system/types.hpp"

#include "system/menu.hpp"
#include "system/variables.hpp"

#include "system/draw/draw.hpp"

#include "system/util/curl.hpp"
#include "system/util/error.hpp"
#include "system/util/hid.hpp"
#include "system/util/log.hpp"
#include "system/util/util.hpp"

//Include myself.
#include "speedtest.hpp"


#define DEF_SPT_NUM_OF_DL_THREADS	4


bool spt_already_init = false;
bool spt_start_request[DEF_SPT_NUM_OF_DL_THREADS] = { false, false, false, false, };
bool spt_thread_run = false;
bool spt_thread_suspend = false;
bool spt_main_run = false;
uint8_t spt_array_index[DEF_SPT_NUM_OF_DL_THREADS] = { 0, 1, 2, 3, };
int spt_data_size = 0;
int spt_total_dl_size[DEF_SPT_NUM_OF_DL_THREADS] = { 0, 0, 0, 0, };
double spt_total_dl_time = 0.0;
std::string spt_msg[DEF_SPT_NUM_OF_MSG];
std::string spt_status = "";
Thread spt_init_thread, spt_exit_thread, spt_spt_thread[DEF_SPT_NUM_OF_DL_THREADS], spt_timer_thread;
Image_data spt_data_size_button[7], spt_start_dl_button;


bool Spt_query_init_flag(void)
{
	return spt_already_init;
}

bool Spt_query_running_flag(void)
{
	return spt_main_run;
}

void Spt_resume(void)
{
	spt_thread_suspend = false;
	spt_main_run = true;
	var_need_reflesh = true;
	Menu_suspend();
}

void Spt_suspend(void)
{
	spt_thread_suspend = true;
	spt_main_run = false;
	Menu_resume();
}

Result_with_string Spt_load_msg(std::string lang)
{
	return Util_load_msg("spt_" + lang + ".txt", spt_msg, DEF_SPT_NUM_OF_MSG);
}

void Spt_timer_thread(void* arg)
{
	Util_log_save(DEF_SPT_TIMER_THREAD_STR, "Thread started.");

	TickCounter stop_watch;
	osTickCounterStart(&stop_watch);

	while (spt_thread_run)
	{
		bool is_testing = false;
		osTickCounterUpdate(&stop_watch);

		for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
		{
			if(spt_start_request[i] && spt_total_dl_size[i] != 0)
			{
				is_testing = true;
				break;
			}
		}

		while (is_testing)
		{
			osTickCounterUpdate(&stop_watch);
			spt_total_dl_time += osTickCounterRead(&stop_watch);
			Util_sleep(41500);

			is_testing = false;
			for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
			{
				if(spt_start_request[i])
				{
					is_testing = true;
					break;
				}
			}
		}

		Util_sleep(DEF_ACTIVE_THREAD_SLEEP_TIME);
	}
	Util_log_save(DEF_SPT_TIMER_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Spt_spt_thread(void* arg)
{
	Util_log_save(DEF_SPT_SPEEDTEST_THREAD_STR, "Thread started.");
	uint8_t thread_id = 0;
	u8* buffer = NULL;
	uint32_t data_size_list[] = { (64 * 1024), (128 * 1024), (256 * 1024), (512 * 1024), (1024 * 1024), (2048 * 1024), (4096 * 1024), };
	int log_num = 0;
	std::string base_url = "https://script.google.com/macros/s/AKfycbw0ElStcpyg1vxx52TDT7sJITysofb7G0Np9CxO47AxzcO2gKXj4fRrI0ESHFpxWKKn/exec?size=";
	Result_with_string result;

	if(!arg)
		thread_id = 0;
	else
		thread_id = *((uint8_t*)arg);

	while (spt_thread_run)
	{
		if (spt_start_request[thread_id])
		{
			std::string url = base_url + std::to_string(data_size_list[spt_data_size]);

			log_num = Util_log_save(DEF_SPT_SPEEDTEST_THREAD_STR, "Util_curl_dl_data()...");
			result = Util_curl_dl_data(url, &buffer, 0x400000, &spt_total_dl_size[thread_id], true, 5);
			Util_log_add(log_num, result.string + result.error_description, result.code);

			free(buffer);
			buffer = NULL;
			if (result.code != 0)
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_SPT_SPEEDTEST_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}

			spt_start_request[thread_id] = false;
		}
		else
			Util_sleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (spt_thread_suspend)
			Util_sleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}
	Util_log_save(DEF_SPT_SPEEDTEST_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Spt_init_thread(void* arg)
{
	Util_log_save(DEF_SPT_INIT_STR, "Thread started.");
	Result_with_string result;

	spt_status = "Starting threads...";
	spt_thread_run = true;
	spt_spt_thread[0] = threadCreate(Spt_spt_thread, (void*)&spt_array_index[0], DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 0, false);
	spt_spt_thread[1] = threadCreate(Spt_spt_thread, (void*)&spt_array_index[1], DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	spt_spt_thread[2] = threadCreate(Spt_spt_thread, (void*)&spt_array_index[2], DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, (var_core_2_available ? 2 : 0), false);
	spt_spt_thread[3] = threadCreate(Spt_spt_thread, (void*)&spt_array_index[3], DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, (var_core_3_available ? 3 : 1), false);

	spt_timer_thread = threadCreate(Spt_timer_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_REALTIME, 1, false);

	spt_status += "\nInitializing variables...";
	spt_data_size = 0;
	spt_total_dl_time = 0.0;
	for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
		spt_total_dl_size[i] = 0;

	for(int i = 0; i < 7; i++)
		spt_data_size_button[i].c2d = var_square_image[0];

	spt_start_dl_button.c2d = var_square_image[0];
	spt_already_init = true;

	for(int i = 0; i < 7; i++)
		Util_add_watch(&spt_data_size_button[i].selected);

	Util_add_watch(&spt_start_dl_button.selected);
	Util_add_watch(&spt_data_size);
	Util_add_watch(&spt_total_dl_time);
	for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
		Util_add_watch(&spt_total_dl_size[i]);

	Util_log_save(DEF_SPT_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Spt_exit_thread(void* arg)
{
	Util_log_save(DEF_SPT_EXIT_STR, "Thread started.");

	spt_thread_run = false;
	spt_thread_suspend = false;

	spt_status = "Exiting threads...";

	for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
	{
		Util_log_save(DEF_SPT_EXIT_STR, "threadJoin()...", threadJoin(spt_spt_thread[i], DEF_THREAD_WAIT_TIME));
		spt_status += ".";
	}
	Util_log_save(DEF_SPT_EXIT_STR, "threadJoin()...", threadJoin(spt_timer_thread, DEF_THREAD_WAIT_TIME));

	spt_status += "\nCleaning up...";
	for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
		threadFree(spt_spt_thread[i]);

	threadFree(spt_timer_thread);

	for(int i = 0; i < 7; i++)
		Util_remove_watch(&spt_data_size_button[i].selected);

	Util_remove_watch(&spt_start_dl_button.selected);
	Util_remove_watch(&spt_data_size);
	Util_remove_watch(&spt_total_dl_time);
	for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
		Util_remove_watch(&spt_total_dl_size[i]);

	spt_already_init = false;
	Util_log_save(DEF_SPT_EXIT_STR, "Thread exit.");
	threadExit(0);
}

void Spt_init(bool draw)
{
	Util_log_save(DEF_SPT_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	Util_add_watch(&spt_status);
	spt_status = "";

	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) && var_core_2_available)
		spt_init_thread = threadCreate(Spt_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
	{
		APT_SetAppCpuTimeLimit(80);
		spt_init_thread = threadCreate(Spt_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	}

	while(!spt_already_init)
	{
		if(draw)
		{
			if (var_night_mode)
			{
				color = DEF_DRAW_WHITE;
				back_color = DEF_DRAW_BLACK;
			}

			if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
			{
				var_need_reflesh = false;
				Draw_frame_ready();
				Draw_screen_ready(SCREEN_TOP_LEFT, back_color);
				Draw_top_ui();
				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();

				Draw(spt_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Util_log_save(DEF_SPT_EXIT_STR, "threadJoin()...", threadJoin(spt_init_thread, DEF_THREAD_WAIT_TIME));
	threadFree(spt_init_thread);
	Spt_resume();

	Util_log_save(DEF_SPT_INIT_STR, "Initialized.");
}

void Spt_exit(bool draw)
{
	Util_log_save(DEF_SPT_EXIT_STR, "Exiting...");

	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	spt_status = "";
	spt_exit_thread = threadCreate(Spt_exit_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);

	while(spt_already_init)
	{
		if(draw)
		{
			if (var_night_mode)
			{
				color = DEF_DRAW_WHITE;
				back_color = DEF_DRAW_BLACK;
			}

			if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
			{
				var_need_reflesh = false;
				Draw_frame_ready();
				Draw_screen_ready(SCREEN_TOP_LEFT, back_color);
				Draw_top_ui();
				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();

				Draw(spt_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			Util_sleep(20000);
	}

	Util_log_save(DEF_SPT_EXIT_STR, "threadJoin()...", threadJoin(spt_exit_thread, DEF_THREAD_WAIT_TIME));
	threadFree(spt_exit_thread);
	Util_remove_watch(&spt_status);
	var_need_reflesh = true;

	Util_log_save(DEF_SPT_EXIT_STR, "Exited.");
}

void Spt_hid(Hid_info key)
{
	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else
	{
		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
		{
			Draw_get_bot_ui_button()->selected = true;
			var_need_reflesh = true;
		}
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
			Spt_suspend();
		else if(Util_hid_is_pressed(key, spt_start_dl_button) && !spt_start_request[0] && !spt_start_request[1])
			spt_start_dl_button.selected = true;
		else if (key.p_a || (Util_hid_is_released(key, spt_start_dl_button) && spt_start_dl_button.selected && !spt_start_request[0] && !spt_start_request[1]))
		{
			for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
				spt_total_dl_size[i] = 0;

			spt_total_dl_time = 0;
			for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
				spt_start_request[i] = true;
		}
		else
		{
			for (int i = 0; i < 7; i++)
			{
				if(Util_hid_is_pressed(key, spt_data_size_button[i]))
					spt_data_size_button[i].selected = true;
				else if (!spt_start_request[0] && !spt_start_request[1] && Util_hid_is_released(key, spt_data_size_button[i]) && spt_data_size_button[i].selected)
					spt_data_size = i;
			}
		}
	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;
		for(int i = 0; i < 7; i++)
			spt_data_size_button[i].selected = false;

		spt_start_dl_button.selected = false;
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

void Spt_main(void)
{
	int color = DEF_DRAW_BLACK;
	int weak_color = DEF_DRAW_WEAK_BLACK;
	int back_color = DEF_DRAW_WHITE;

	if (var_night_mode)
	{
		color = DEF_DRAW_WHITE;
		weak_color = DEF_DRAW_WEAK_WHITE;
		back_color = DEF_DRAW_BLACK;
	}

	if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
	{
		var_need_reflesh = false;
		Draw_frame_ready();

		if(var_turn_on_top_lcd)
		{
			uint32_t total_size = 0;

			for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
				total_size += spt_total_dl_size[i];

			Draw_screen_ready(SCREEN_TOP_LEFT, back_color);

			Draw(spt_msg[0] + std::to_string(total_size / (1024 * 1024)) + "MB(" + std::to_string(total_size / 1024) + "KB)", 0.0, 20.0, 0.5, 0.5, DEF_DRAW_BLUE);
			Draw(spt_msg[1] + std::to_string(spt_total_dl_time).substr(0, std::to_string(spt_total_dl_time).length() - 3) + " ms", 0.0, 40.0, 0.5, 0.5, DEF_DRAW_BLUE);
			Draw(spt_msg[2] + std::to_string(((total_size / (spt_total_dl_time / 1000.0)) / (1024 * 1024)) * 8) + "Mbps", 0.0, 60.0, 0.75, 0.75, DEF_DRAW_BLUE);

			if(Util_log_query_log_show_flag())
				Util_log_draw();

			Draw_top_ui();

			if(var_monitor_cpu_usage)
				Draw_cpu_usage_info();

			if(Draw_is_3d_mode())
			{
				Draw_screen_ready(SCREEN_TOP_RIGHT, back_color);

				if(Util_log_query_log_show_flag())
					Util_log_draw();

				Draw_top_ui();

				if(var_monitor_cpu_usage)
					Draw_cpu_usage_info();
			}
		}

		if(var_turn_on_bottom_lcd)
		{
			bool is_testing = false;

			Draw_screen_ready(SCREEN_BOTTOM, back_color);

			Draw(DEF_SPT_VER, 0, 0, 0.4, 0.4, DEF_DRAW_GREEN);
			Draw(spt_msg[3], 70.0, 10.0, 0.75, 0.75, DEF_DRAW_RED);
			for (int i = 0; i < 7; i++)
			{
				Draw(spt_msg[4 + i], 100, 40 + (i * 20), 0.5, 0.5, spt_data_size == i ? DEF_DRAW_BLUE : color, X_ALIGN_CENTER, Y_ALIGN_CENTER,
				130, 20, BACKGROUND_ENTIRE_BOX, &spt_data_size_button[i], spt_data_size_button[i].selected ? DEF_DRAW_YELLOW : DEF_DRAW_WEAK_YELLOW);
			}

			for(uint8_t i = 0; i < DEF_SPT_NUM_OF_DL_THREADS; i++)
			{
				if(spt_start_request[i])
					is_testing = true;
			}

			Draw(spt_msg[11], 130, 190, 0.5, 0.5, (is_testing ? weak_color : color), X_ALIGN_CENTER, Y_ALIGN_CENTER, 60, 20,
			BACKGROUND_ENTIRE_BOX, &spt_start_dl_button, spt_start_dl_button.selected ? DEF_DRAW_RED : DEF_DRAW_WEAK_RED);

			Draw_bot_ui();

			if(Util_err_query_error_show_flag())
				Util_err_draw();
		}

		Draw_apply_draw();
	}
	else
		gspWaitForVBlank();
}

#include "system/headers.hpp"

bool mic_main_run = false;
bool mic_thread_run = false;
bool mic_already_init = false;
bool mic_thread_suspend = true;
bool mic_start_record_request = false;
bool mic_stop_record_request = false;
u64 mic_record_start_time = 0.0;
std::string mic_msg[DEF_MIC_NUM_OF_MSG];
std::string mic_format = ".mp4";
std::string mic_dir_path = "";
std::string mic_file_name = "";
std::string mic_status = "";
Thread mic_init_thread, mic_exit_thread, mic_record_thread;
Image_data mic_start_record_button, mic_stop_record_button;

bool Mic_query_init_flag(void)
{
	return mic_already_init;
}

bool Mic_query_running_flag(void)
{
	return mic_main_run;
}

void Mic_resume(void)
{
	mic_thread_suspend = false;
	mic_main_run = true;
	var_need_reflesh = true;
	Menu_suspend();
}

void Mic_suspend(void)
{
	mic_thread_suspend = true;
	mic_main_run = false;
	Menu_resume();
}

void Mic_record_thread(void* arg)
{
	Util_log_save(DEF_MIC_RECORD_THREAD_STR, "Thread started.");
	int log = 0;
	int audio_size = 0;
	u8 dummy = 0;
	u8* raw_audio = NULL;
	char time[128];
	Result_with_string result;

	Util_file_save_to_file(".", DEF_MAIN_DIR + "sound/", &dummy, 1, true);//create directory

	while (mic_thread_run)
	{
		if (mic_start_record_request)
		{
			aptSetSleepAllowed(false);

			memset(time, 0, 128);
			sprintf(time, "%04d_%02d_%02d", var_years, var_months, var_days);
			mic_dir_path = DEF_MAIN_DIR + "sound/" + time + "/";

			memset(time, 0, 128);
			sprintf(time, "%02d_%02d_%02d", var_hours, var_minutes, var_seconds);
			mic_file_name = time + mic_format;

			Util_file_save_to_file(".", mic_dir_path, &dummy, 1, true);//create directory

			log = Util_log_save(DEF_MIC_RECORD_THREAD_STR, "Util_encoder_create_output_file()...");
			result = Util_encoder_create_output_file(mic_dir_path + mic_file_name, 0);
			Util_log_add(log, result.string, result.code);
			if(result.code != 0)
				mic_stop_record_request = true;

			if(!mic_stop_record_request)
			{
				log = Util_log_save(DEF_MIC_RECORD_THREAD_STR, "Util_init_audio_encoder()...");
				if(mic_format == ".mp4")
					result = Util_audio_encoder_init(DEF_ENCODER_AUDIO_CODEC_AAC, 32728, 32000, 96000, 0);
				else if(mic_format == ".mp3")
					result = Util_audio_encoder_init(DEF_ENCODER_AUDIO_CODEC_MP3, 32728, 32000, 96000, 0);
				else if(mic_format == ".mp2")
					result = Util_audio_encoder_init(DEF_ENCODER_AUDIO_CODEC_MP2, 32728, 32000, 96000, 0);
				else if(mic_format == ".ac3")
					result = Util_audio_encoder_init(DEF_ENCODER_AUDIO_CODEC_AC3, 32728, 32000, 96000, 0);
				Util_log_add(log, result.string, result.code);

				if(result.code != 0)
					mic_stop_record_request = true;
			}

			if(!mic_stop_record_request)
			{
				log = Util_log_save(DEF_MIC_RECORD_THREAD_STR, "Util_encoder_write_header()...");
				result = Util_encoder_write_header(0);
				Util_log_add(log, result.string, result.code);
				if(result.code != 0)
					mic_stop_record_request = true;
			}

			if(!mic_stop_record_request)
			{
				log = Util_log_save(DEF_MIC_RECORD_THREAD_STR, "Util_mic_init()...");
				result = Util_mic_init(1024 * 128);
				Util_log_add(log, result.string, result.code);
				if(result.code != 0)
					mic_stop_record_request = true;
			}

			if(!mic_stop_record_request)
			{
				log = Util_log_save(DEF_MIC_RECORD_THREAD_STR, "Util_mic_start_recording()...");
				result = Util_mic_start_recording(32728);
				Util_log_add(log, result.string, result.code);
				mic_record_start_time = osGetTime();
				if(result.code != 0)
					mic_stop_record_request = true;
			}

			if(!mic_stop_record_request)
			{
				while (true)
				{
					usleep(100000);
					var_need_reflesh = true;

					if(Util_mic_query_remaining_buffer_time() < 500 || mic_stop_record_request)
					{
						result = Util_mic_get_audio_data(&raw_audio, &audio_size);
						if(result.code != 0)
							mic_stop_record_request = true;
						else
						{
							log = Util_log_save("", "");
							result = Util_audio_encoder_encode(audio_size, raw_audio, 0);
							Util_log_add(log, "");
							if(result.code != 0)
								mic_stop_record_request = true;
						}

						free(raw_audio);
						raw_audio = NULL;

						if(mic_stop_record_request)
							break;
					}
				}
			}

			if(result.code != 0)
			{
				Util_err_set_error_show_flag(true);
				Util_err_set_error_message(result.string, result.error_description, DEF_MIC_RECORD_THREAD_STR, result.code);
			}

			Util_mic_exit();
			Util_encoder_close_output_file(0);
			mic_start_record_request = false;
			mic_stop_record_request = false;
			aptSetSleepAllowed(true);
		}
		else
			usleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (mic_thread_suspend)
			usleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}
	Util_log_save(DEF_MIC_RECORD_THREAD_STR, "Thread exit.");
	threadExit(0);
}

Result_with_string Mic_load_msg(std::string lang)
{
	return  Util_load_msg("mic_" + lang + ".txt", mic_msg, DEF_MIC_NUM_OF_MSG);
}

void Mic_hid(Hid_info key)
{
	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else
	{
		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
			Draw_get_bot_ui_button()->selected = true;
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
			Mic_suspend();
		else if (Util_hid_is_pressed(key, mic_start_record_button))
			mic_start_record_button.selected = true;
		else if (key.p_a || (Util_hid_is_released(key, mic_start_record_button) && mic_start_record_button.selected))
			mic_start_record_request = true;
		else if (Util_hid_is_pressed(key, mic_stop_record_button))
			mic_stop_record_button.selected = true;
		else if (key.p_b || (Util_hid_is_released(key, mic_stop_record_button) && mic_stop_record_button.selected))
			mic_stop_record_request = true;
		else if (key.p_y && !mic_start_record_request)
		{
			if(mic_format == ".mp4")
				mic_format = ".mp3";
			else if(mic_format == ".mp3")
				mic_format = ".mp2";
			else if(mic_format == ".mp2")
				mic_format = ".ac3";
			else
				mic_format = ".mp4";
		}
	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;
		mic_start_record_button.selected = false;
		mic_stop_record_button.selected = false;
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

void Mic_init_thread(void* arg)
{
	Util_log_save(DEF_MIC_INIT_STR, "Thread started.");
	Result_with_string result;

	mic_status = "Starting threads...";
	mic_thread_run = true;
	mic_record_thread = threadCreate(Mic_record_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_HIGH, 0, false);

	mic_status += "\nInitializing variables...";
	mic_format = ".mp4";
	mic_record_start_time = 0;

	mic_start_record_button.c2d = var_square_image[0];
	mic_stop_record_button.c2d = var_square_image[0];

	Util_add_watch(&mic_format);
	Util_add_watch((double*)&mic_record_start_time);
	Util_add_watch(&mic_start_record_button.selected);
	Util_add_watch(&mic_stop_record_button.selected);

	mic_already_init = true;

	Util_log_save(DEF_MIC_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Mic_exit_thread(void* arg)
{
	Util_log_save(DEF_MIC_EXIT_STR, "Thread started.");

	mic_thread_suspend = false;
	mic_thread_run = false;
	mic_stop_record_request = true;
	mic_start_record_request = false;

	mic_status = "Exiting threads...";
	Util_log_save(DEF_MIC_EXIT_STR, "threadJoin()...", threadJoin(mic_record_thread, DEF_THREAD_WAIT_TIME));

	mic_status += "\nCleaning up...";
	threadFree(mic_record_thread);

	Util_remove_watch(&mic_format);
	Util_remove_watch((double*)&mic_record_start_time);
	Util_remove_watch(&mic_start_record_button.selected);
	Util_remove_watch(&mic_stop_record_button.selected);

	mic_already_init = false;
	
	Util_log_save(DEF_MIC_EXIT_STR, "Thread exit.");
	threadExit(0);
}

void Mic_init(bool draw)
{
	Util_log_save(DEF_MIC_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	Util_add_watch(&mic_status);
	mic_status = "";

	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) && var_core_2_available)
		mic_init_thread = threadCreate(Mic_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
	{
		APT_SetAppCpuTimeLimit(80);
		mic_init_thread = threadCreate(Mic_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	}

	while(!mic_already_init)
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
				Draw_screen_ready(0, back_color);
				Draw_top_ui();
				Draw(mic_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			usleep(20000);
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_N3DS) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Util_log_save(DEF_MIC_EXIT_STR, "threadJoin()...", threadJoin(mic_init_thread, DEF_THREAD_WAIT_TIME));
	threadFree(mic_init_thread);
	Mic_resume();

	Util_log_save(DEF_MIC_INIT_STR, "Initialized.");
}

void Mic_exit(bool draw)
{
	Util_log_save(DEF_MIC_EXIT_STR, "Exiting...");

	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	mic_status = "";
	mic_exit_thread = threadCreate(Mic_exit_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);

	while(mic_already_init)
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
				Draw_screen_ready(0, back_color);
				Draw_top_ui();
				Draw(mic_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			usleep(20000);
	}

	Util_log_save(DEF_MIC_EXIT_STR, "threadJoin()...", threadJoin(mic_exit_thread, DEF_THREAD_WAIT_TIME));	
	threadFree(mic_exit_thread);
	Util_remove_watch(&mic_status);
	var_need_reflesh = true;

	Util_log_save(DEF_MIC_EXIT_STR, "Exited.");
}

void Mic_main(void)
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
			Draw_screen_ready(0, back_color);

			if(Util_log_query_log_show_flag())
				Util_log_draw();

			Draw_top_ui();

			if(var_3d_mode)
			{
				Draw_screen_ready(2, back_color);

				if(Util_log_query_log_show_flag())
					Util_log_draw();

				Draw_top_ui();
			}
		}
		
		if(var_turn_on_bottom_lcd)
		{
			Draw_screen_ready(1, back_color);

			Draw(DEF_MIC_VER, 0, 0, 0.4, 0.4, DEF_DRAW_GREEN);
			Draw(mic_msg[3], 75.0, 35.0, 0.4, 0.4, color);
			if(mic_start_record_request)
				Draw(mic_msg[2], 15.0, 75.0, 0.6, 0.6, DEF_DRAW_RED);

			//Start recording button
			Draw(mic_msg[0], 105, 60, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 50, 50,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mic_start_record_button, mic_start_record_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			//Stop recording button
			Draw(mic_msg[1], 165, 60, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 50, 50,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mic_stop_record_button, mic_stop_record_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			if(mic_start_record_request)
				Draw(Util_convert_seconds_to_time((osGetTime() - mic_record_start_time) / 1000.0), 102.5, 105.0, 0.5, 0.5, color);

			Draw("format(Y) " + mic_format, 102.5, 125.0, 0.5, 0.5, mic_start_record_request ? weak_color : color);

			if(Util_err_query_error_show_flag())
				Util_err_draw();

			Draw_bot_ui();
		}

		Draw_apply_draw();
	}
	else
		gspWaitForVBlank();
}

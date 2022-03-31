#include "system/headers.hpp"


bool mup_main_run = false;
bool mup_thread_run = false;
bool mup_already_init = false;
bool mup_thread_suspend = true;
bool mup_play_request = false;
bool mup_stop_request = false;
bool mup_loop_request = false;
bool mup_shuffle_request = false;
bool mup_allow_sleep = false;
bool mup_seek_request = false;
bool mup_change_music_request = false;
bool mup_dl_and_play_request = false;
bool mup_pause_request = false;
double mup_bar_pos = 0.0;
u64 mup_offset = 0;
int mup_dled_size = 0;
std::string mup_url = "";
std::string mup_load_file_name = "";
std::string mup_load_dir_name = "/";
std::string mup_msg[DEF_MUP_NUM_OF_MSG];
std::string mup_status = "";
Thread mup_init_thread, mup_exit_thread, mup_play_thread, mup_worker_thread;
Audio_info mup_audio_info;
Image_data mup_shuffle_button[2], mup_allow_sleep_button[2], mup_loop_button[2], mup_select_file_button, mup_play_button, mup_stop_button;

bool Mup_query_init_flag(void)
{
	return mup_already_init;
}

bool Mup_query_running_flag(void)
{
	return mup_main_run;
}

void Mup_resume(void)
{
	mup_thread_suspend = false;
	mup_main_run = true;
	var_need_reflesh = true;
	Menu_suspend();
}

void Mup_suspend(void)
{
	mup_thread_suspend = true;
	mup_main_run = false;
	var_need_reflesh = true;
	Menu_resume();
}

void Mup_cancel_select_file(void)
{
}

void Mup_set_allow_sleep(bool flag)
{
	mup_allow_sleep = flag;
	aptSetSleepAllowed(mup_allow_sleep);
}

void Mup_set_url(std::string url)
{
	mup_url = url;
	mup_dl_and_play_request = true;
}

void Mup_set_load_file(std::string file_name, std::string dir_name)
{
	mup_load_file_name = file_name;
	mup_load_dir_name = dir_name;
	mup_play_request = true;
	mup_change_music_request = true;
}

void Mup_hid(Hid_info key)
{
	if(Util_err_query_error_show_flag())
		Util_err_main(key);
	else if(Util_expl_query_show_flag())
		Util_expl_main(key);
	else
	{
		if(Util_hid_is_pressed(key, *Draw_get_bot_ui_button()))
			Draw_get_bot_ui_button()->selected = true;
		else if (key.p_start || (Util_hid_is_released(key, *Draw_get_bot_ui_button()) && Draw_get_bot_ui_button()->selected))
			Mup_suspend();
		else if(key.p_touch && key.touch_x >= 10 && key.touch_x <= 310 && key.touch_y >= 120 && key.touch_y <= 127)
		{
			mup_offset = (key.touch_x - 10) * (mup_audio_info.duration * 1000 / 300);
			mup_seek_request = true;
		}
		else if(Util_hid_is_pressed(key, mup_play_button))
			mup_play_button.selected = true;
		if (key.p_a || (Util_hid_is_released(key, mup_play_button) && mup_play_button.selected))
		{
			if(!mup_play_request)
				mup_play_request = true;
			else if(!mup_pause_request)
				mup_pause_request = true;
			else
				mup_pause_request = false;
		}
		else if(Util_hid_is_pressed(key, mup_stop_button))
			mup_stop_button.selected = true;
		else if (key.p_b || (Util_hid_is_released(key, mup_stop_button) && mup_stop_button.selected))
			mup_stop_request = true;
		else if(Util_hid_is_pressed(key, mup_shuffle_button[0]))
			mup_shuffle_button[0].selected = true;
		else if (Util_hid_is_released(key, mup_shuffle_button[0]) && mup_shuffle_button[0].selected)
			mup_shuffle_request = true;
		else if(Util_hid_is_pressed(key, mup_shuffle_button[1]))
			mup_shuffle_button[1].selected = true;
		else if (Util_hid_is_released(key, mup_shuffle_button[1]) && mup_shuffle_button[1].selected)
			mup_shuffle_request = false;
		else if(Util_hid_is_pressed(key, mup_loop_button[0]))
			mup_loop_button[0].selected = true;
		else if (Util_hid_is_released(key, mup_loop_button[0]) && mup_loop_button[0].selected)
			mup_loop_request = true;
		else if(Util_hid_is_pressed(key, mup_loop_button[1]))
			mup_loop_button[1].selected = true;
		else if (Util_hid_is_released(key, mup_loop_button[1]) && mup_loop_button[1].selected)
			mup_loop_request = false;
		else if(Util_hid_is_pressed(key, mup_allow_sleep_button[0]))
			mup_allow_sleep_button[0].selected = true;
		else if (Util_hid_is_released(key, mup_allow_sleep_button[0]) && mup_allow_sleep_button[0].selected)
			Mup_set_allow_sleep(true);
		else if(Util_hid_is_pressed(key, mup_allow_sleep_button[1]))
			mup_allow_sleep_button[1].selected = true;
		else if (Util_hid_is_released(key, mup_allow_sleep_button[1]) && mup_allow_sleep_button[1].selected)
			Mup_set_allow_sleep(false);
		else if(Util_hid_is_pressed(key, mup_select_file_button))
			mup_select_file_button.selected = true;
		else if (key.p_x || (Util_hid_is_released(key, mup_select_file_button) && mup_select_file_button.selected))
		{
			Util_expl_set_callback(Mup_set_load_file);
			Util_expl_set_cancel_callback(Mup_cancel_select_file);
			Util_expl_set_show_flag(true);
		}
	}

	if(!key.p_touch && !key.h_touch)
	{
		Draw_get_bot_ui_button()->selected = false;

		for (int i = 0; i < 2; i++)
		{
			mup_shuffle_button[i].selected = false;
			mup_allow_sleep_button[i].selected = false;
			mup_loop_button[i].selected = false;
		}
		mup_select_file_button.selected = false;
		mup_play_button.selected = false;
		mup_stop_button.selected = false;
	}

	if(Util_log_query_log_show_flag())
		Util_log_main(key);
}

void Mup_worker_thread(void* arg)
{
	Util_log_save(DEF_MUP_WORKER_THREAD_STR, "Thread started.");
	size_t cut_pos = std::string::npos;
	int log_num = 0;
	char date[128];
	std::string cache_string = "";
	Result_with_string result;

	while (mup_thread_run)
	{
		if(mup_dl_and_play_request)
		{
			cut_pos = mup_url.find("&id=");
			if (!(cut_pos == std::string::npos))
			{
				cache_string = mup_url.substr(cut_pos + 4);

				if (cache_string.length() > 33)
					cache_string = cache_string.substr(0, 33);

				cache_string += ".m4a";
			}
			else
			{
				memset(date, 0, 128);
				sprintf(date, "%04d_%02d_%02d_%02d_%02d_%02d.m4a", var_years, var_months, var_days, var_hours, var_minutes, var_seconds);
				cache_string = date;
			}

			log_num = Util_log_save(DEF_MUP_WORKER_THREAD_STR, "Util_httpc_dl_data()...");
			result = Util_httpc_save_data(mup_url, 0x20000, (u32*)&mup_dled_size, true, 5, DEF_MAIN_DIR + "audio/", cache_string);
			Util_log_add(log_num, result.string, result.code);

			if(result.code == 0)
			{
				mup_load_file_name = cache_string;
				mup_load_dir_name = DEF_MAIN_DIR + "audio/";
				mup_play_request = true;
				mup_change_music_request = true;
			}
			mup_dl_and_play_request = false;
		}
		else
			usleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (mup_thread_suspend)
			usleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}

	Util_log_save(DEF_MUP_WORKER_THREAD_STR, "Thread exit.");
	threadExit(0);
}

void Mup_play_thread(void* arg)
{
	Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Thread started.");
	bool key_frame = false;
	int audio_tracks = 0;
	int video_tracks = 0;
	int subtitle_tracks = 0;
	int random_num = 0;
	int file_index = 0;
	int audio_size = 0;
	int log_num = 0;
	int type = 0;
	int packet_index = 0;
	double pos = 0;
	u8* sound_buffer = NULL;
	std::string file_name = "";
	std::string dir_name = "/";
	Result_with_string result;
	Audio_info audio_info;

	while (mup_thread_run)
	{
		if (mup_play_request)
		{
			if(mup_change_music_request)
				mup_change_music_request = false;

			mup_stop_request = false;
			mup_seek_request = false;
			mup_pause_request = false;
			file_name = mup_load_file_name;
			dir_name = mup_load_dir_name;
			mup_offset = 0;
			Util_expl_set_current_dir(dir_name);
			//Reload
			for(int i = 0; i < 10; i++)
			{
				usleep(100000);
				if(Util_expl_query_num_of_file() > 0)
					break;
			}

			if(mup_shuffle_request)
			{
				file_index++;
				srand((unsigned) time(NULL));
				while (true)
				{
					random_num = rand() % 2;
					if(!(file_index < Util_expl_query_num_of_file()))
						file_index = 0;
					else if(random_num == 1 && Util_expl_query_type(file_index) == DEF_EXPL_TYPE_FILE)
						break;
					else
						file_index++;
				}

				file_name = Util_expl_query_file_name(file_index);
			}

			log_num = Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Util_decoder_open_file()...");
			result = Util_decoder_open_file(dir_name + file_name, &audio_tracks, &video_tracks, &subtitle_tracks, 1);
			Util_log_add(log_num, result.string, result.code);

			if(result.code != 0)
			{
				Util_err_set_error_message(result.string, result.error_description, DEF_MUP_PLAY_THREAD_STR, result.code);
				Util_err_set_error_show_flag(true);
			}
			if(result.code == 0 && audio_tracks > 0)
			{
				log_num = Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Util_audio_decoder_init()...");
				result = Util_audio_decoder_init(audio_tracks, 1);
				Util_log_add(log_num, result.string, result.code);

				if(result.code == 0)
				{
					Util_audio_decoder_get_info(&audio_info, 0, 1);
					mup_audio_info.bitrate = audio_info.bitrate;
					mup_audio_info.ch = audio_info.ch;
					mup_audio_info.duration = audio_info.duration;
					mup_audio_info.sample_rate = audio_info.sample_rate;
					mup_audio_info.track_lang = audio_info.track_lang;
					mup_audio_info.format_name = audio_info.format_name;
					log_num = Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Util_speaker_init()...");
					result = Util_speaker_init();
					Util_log_add(log_num, result.string, result.code);
					audio_info.duration *= 1000;

					if(result.code == 0 || result.code == DEF_ERR_ALREADY_INITIALIZED)
					{
						Util_speaker_set_audio_info(1, audio_info.ch, audio_info.sample_rate);
						while(true)
						{
							if(mup_pause_request)
							{
								Util_speaker_pause(1);
								while(mup_pause_request && !mup_stop_request && !mup_change_music_request && !mup_seek_request)
									usleep(50000);

								Util_speaker_resume(1);
							}

							if(mup_stop_request || mup_change_music_request || mup_seek_request)
							{
								if(mup_seek_request)
								{
									log_num = Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Util_decoder_seek()... ");
									result = Util_decoder_seek(mup_offset, DEF_DECODER_SEEK_FLAG_ANY, 1);
									Util_log_add(log_num, result.string, result.code);
									Util_speaker_clear_buffer(1);
									if(result.code == 0)
										mup_bar_pos = mup_offset;

									mup_seek_request = false;
								}
								else
									break;
							}

							result = Util_decoder_read_packet(1);
							if(result.code == 0)
							{
								result = Util_decoder_parse_packet(&type, &packet_index, &key_frame, 1);
								if(type == DEF_DECODER_PACKET_TYPE_AUDIO && packet_index == 0)
								{
									result = Util_decoder_ready_audio_packet(packet_index, 1);
									if(result.code == 0)
									{
										result = Util_audio_decoder_decode(&audio_size, &sound_buffer, &pos, packet_index, 1);
										if(result.code == 0)
										{
											mup_bar_pos = pos - (Util_speaker_get_available_buffer_size(1) / 2.0 / mup_audio_info.ch / mup_audio_info.sample_rate * 1000);
											while(true)
											{
												result = Util_speaker_add_buffer(1, sound_buffer, audio_size);
												if(result.code == 0 || mup_stop_request || mup_change_music_request || mup_seek_request)
													break;
												else
													usleep(100000);
											}
										}
										else
											Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Util_decode_audio()..." + result.string + result.error_description, result.code);

										free(sound_buffer);
										sound_buffer = NULL;
									}
									else
										Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Util_decoder_ready_audio_packet()..." + result.string + result.error_description, result.code);
								}
								else if(type == DEF_DECODER_PACKET_TYPE_AUDIO)
									Util_decoder_skip_audio_packet(packet_index, 1);
								else if(type == DEF_DECODER_PACKET_TYPE_VIDEO)
									Util_decoder_skip_video_packet(packet_index, 1);
								else if(type == DEF_DECODER_PACKET_TYPE_SUBTITLE)
									Util_decoder_skip_subtitle_packet(packet_index, 1);
							}
							else
								break;
						}
					}
				}
			}

			//Wait audio playback to finish
			while(!mup_stop_request && !mup_change_music_request && Util_speaker_get_available_buffer_num(1) > 0)
			{
				mup_bar_pos = (mup_audio_info.duration * 1000) - (Util_speaker_get_available_buffer_size(1) / 2.0 / mup_audio_info.ch / mup_audio_info.sample_rate * 1000);
				usleep(100000);
			}

			Util_speaker_clear_buffer(1);
			Util_decoder_close_file(1);
			if (!mup_loop_request && !mup_change_music_request)
				mup_play_request = false;
		}
		else
			usleep(DEF_ACTIVE_THREAD_SLEEP_TIME);

		while (mup_thread_suspend && !mup_loop_request && !mup_change_music_request)
			usleep(DEF_INACTIVE_THREAD_SLEEP_TIME);
	}

	Util_log_save(DEF_MUP_PLAY_THREAD_STR, "Thread exit.");
	threadExit(0);
}

Result_with_string Mup_load_msg(std::string lang)
{
	return  Util_load_msg("mup_" + lang + ".txt", mup_msg, DEF_MUP_NUM_OF_MSG);
}

void Mup_init_thread(void* arg)
{
	Util_log_save(DEF_MUP_INIT_STR, "Thread started.");
	Result_with_string result;
	
	mup_status = "Starting threads...";
	mup_thread_run = true;
	mup_play_thread = threadCreate(Mup_play_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_HIGH, 0, false);
	mup_worker_thread = threadCreate(Mup_worker_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 0, false);

	mup_status = "Initializing variables...";
	mup_bar_pos = 0.0;
	mup_audio_info.bitrate = 0;
	mup_audio_info.ch = 0;
	mup_audio_info.duration = 0;
	mup_audio_info.sample_rate = 0;
	mup_audio_info.track_lang = "unknown";
	mup_audio_info.format_name = "unknown";

	for (int i = 0; i < 2; i++)
	{
		mup_shuffle_button[i].c2d = var_square_image[0];
		mup_allow_sleep_button[i].c2d = var_square_image[0];
		mup_loop_button[i].c2d = var_square_image[0];
	}
	mup_select_file_button.c2d = var_square_image[0];
	mup_play_button.c2d = var_square_image[0];
	mup_stop_button.c2d = var_square_image[0];

	for (int i = 0; i < 2; i++)
	{
		Util_add_watch(&mup_shuffle_button[i].selected);
		Util_add_watch(&mup_allow_sleep_button[i].selected);
		Util_add_watch(&mup_loop_button[i].selected);
	}
	Util_add_watch(&mup_select_file_button.selected);
	Util_add_watch(&mup_play_button.selected);
	Util_add_watch(&mup_stop_button.selected);
	Util_add_watch(&mup_loop_request);
	Util_add_watch(&mup_shuffle_request);
	Util_add_watch(&mup_allow_sleep);
	Util_add_watch(&mup_play_request);
	Util_add_watch(&mup_stop_request);
	Util_add_watch(&mup_change_music_request);
	Util_add_watch(&mup_dl_and_play_request);
	Util_add_watch(&mup_pause_request);
	Util_add_watch(&mup_dled_size);
	Util_add_watch(&mup_bar_pos);

	aptSetSleepAllowed(false);

	mup_already_init = true;

	Util_log_save(DEF_MUP_INIT_STR, "Thread exit.");
	threadExit(0);
}

void Mup_exit_thread(void* arg)
{
	Util_log_save(DEF_MUP_EXIT_STR, "Thread started.");

	if(mup_play_request)
		mup_stop_request = true;

	mup_already_init = false;
	mup_thread_suspend = false;
	mup_thread_run = false;

	mup_status = "Exiting threads...";
	Util_log_save(DEF_MUP_EXIT_STR, "threadJoin()...", threadJoin(mup_worker_thread, DEF_THREAD_WAIT_TIME));	

	mup_status += ".";
	Util_log_save(DEF_MUP_EXIT_STR, "threadJoin()...", threadJoin(mup_play_thread, DEF_THREAD_WAIT_TIME));	

	mup_status += "\nCleaning up...";	
	threadFree(mup_worker_thread);
	threadFree(mup_play_thread);

	for (int i = 0; i < 2; i++)
	{
		Util_remove_watch(&mup_shuffle_button[i].selected);
		Util_remove_watch(&mup_allow_sleep_button[i].selected);
		Util_remove_watch(&mup_loop_button[i].selected);
	}
	Util_remove_watch(&mup_select_file_button.selected);
	Util_remove_watch(&mup_play_button.selected);
	Util_remove_watch(&mup_stop_button.selected);
	Util_remove_watch(&mup_play_request);
	Util_remove_watch(&mup_stop_request);
	Util_remove_watch(&mup_change_music_request);
	Util_remove_watch(&mup_dl_and_play_request);
	Util_remove_watch(&mup_pause_request);
	Util_remove_watch(&mup_dled_size);
	Util_remove_watch(&mup_bar_pos);

	mup_already_init = false;

	Util_log_save(DEF_MUP_EXIT_STR, "Thread exit.");
	threadExit(0);
}

void Mup_init(bool draw)
{
	Util_log_save(DEF_MUP_INIT_STR, "Initializing...");
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	Util_add_watch(&mup_status);
	mup_status = "";

	if((var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_3DSXL) && var_core_2_available)
		mup_init_thread = threadCreate(Mup_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 2, false);
	else
	{
		APT_SetAppCpuTimeLimit(80);
		mup_init_thread = threadCreate(Mup_init_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);
	}

	while(!mup_already_init)
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
				Draw(mup_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			usleep(20000);
	}

	if(!(var_model == CFG_MODEL_N2DSXL || var_model == CFG_MODEL_N3DSXL || var_model == CFG_MODEL_3DSXL) || !var_core_2_available)
		APT_SetAppCpuTimeLimit(10);

	Util_log_save(DEF_MUP_EXIT_STR, "threadJoin()...", threadJoin(mup_init_thread, DEF_THREAD_WAIT_TIME));	
	threadFree(mup_init_thread);
	Mup_resume();

	Util_log_save(DEF_MUP_INIT_STR, "Initialized.");
}

void Mup_exit(bool draw)
{
	Util_log_save(DEF_MUP_EXIT_STR, "Exiting...");

	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;

	mup_status = "";
	mup_exit_thread = threadCreate(Mup_exit_thread, (void*)(""), DEF_STACKSIZE, DEF_THREAD_PRIORITY_NORMAL, 1, false);

	while(mup_already_init)
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
				Draw(mup_status, 0, 20, 0.65, 0.65, color);

				Draw_apply_draw();
			}
			else
				gspWaitForVBlank();
		}
		else
			usleep(20000);
	}

	Util_log_save(DEF_MUP_EXIT_STR, "threadJoin()...", threadJoin(mup_exit_thread, DEF_THREAD_WAIT_TIME));	
	threadFree(mup_exit_thread);
	Util_remove_watch(&mup_status);
	var_need_reflesh = true;

	Util_log_save(DEF_MUP_EXIT_STR, "Exited.");
}

void Mup_main(void)
{
	int color = DEF_DRAW_BLACK;
	int back_color = DEF_DRAW_WHITE;
	int msg_num = 0;

	if (var_night_mode)
	{
		color = DEF_DRAW_WHITE;
		back_color = DEF_DRAW_BLACK;
	}

	if(Util_is_watch_changed() || var_need_reflesh || !var_eco_mode)
	{
		var_need_reflesh = false;
		Draw_frame_ready();

		if(var_turn_on_top_lcd)
		{
			Draw_screen_ready(0, back_color);

			if(mup_dl_and_play_request)
				Draw(mup_msg[20] + std::to_string(mup_dled_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 0, 20, 0.5, 0.5, color);

			Draw(mup_msg[0] + std::to_string(mup_audio_info.bitrate / 1000) + mup_msg[1], 0, 35, 0.5, 0.5, color);
			Draw(mup_msg[2] + std::to_string((double)mup_audio_info.sample_rate / 1000.0).substr(0, std::to_string((double)mup_audio_info.sample_rate / 1000.0).length() - 4) + "Kbps", 0, 50, 0.5, 0.5, color);
			Draw(mup_msg[3] + std::to_string(mup_audio_info.ch) + mup_msg[4], 0, 65, 0.5, 0.5, color);
			Draw(mup_msg[5] + Util_convert_seconds_to_time(mup_audio_info.duration), 0, 80, 0.5, 0.5, color);
			Draw(mup_msg[7] + mup_audio_info.format_name, 0.0, 95.0, 0.5, 0.5, color);

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

			if(mup_play_request && !mup_pause_request)
				msg_num = 19;
			else
				msg_num = 16;

			Draw(DEF_MUP_VER, 0, 0, 0.4, 0.4, DEF_DRAW_GREEN);

			//Play button
			Draw(mup_msg[msg_num], 105, 60, 0.45, 0.45, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 50, 50,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mup_play_button, mup_play_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			//Stop button
			Draw(mup_msg[17], 165, 60, 0.45, 0.45, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER, 50, 50,
			DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mup_stop_button, mup_stop_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			//Time bar
			Draw(Util_convert_seconds_to_time(mup_bar_pos / 1000) + " / " + Util_convert_seconds_to_time(mup_audio_info.duration), 12.5, 105, 0.5, 0.5, color);
			Draw_texture(var_square_image[0], DEF_DRAW_AQUA, 10, 120, 300, 8);
			if(mup_audio_info.duration != 0.0)
				Draw_texture(var_square_image[0], DEF_DRAW_RED, 10, 120, 300 * ((mup_bar_pos / 1000) / mup_audio_info.duration), 8);

			//Loop button
			Draw(mup_msg[9], 12.5, 165, 0.5, 0.5, color);
			for (int i = 0; i < 2; i++)
			{
				Draw(mup_msg[10 + i], 10 + (i * 50), 180, 0.5, 0.5, mup_loop_request != i ? DEF_DRAW_RED : color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER,
				40, 20, DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mup_loop_button[i], mup_loop_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			}

			//Allow sleep button
			Draw(mup_msg[12], 122.5, 165, 0.5, 0.5, color);
			for (int i = 0; i < 2; i++)
			{
				Draw(mup_msg[13 + i], 120 + (i * 50), 180, 0.4, 0.4, mup_allow_sleep != i ? DEF_DRAW_RED : color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER,
				40, 20, DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mup_allow_sleep_button[i], mup_allow_sleep_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			}

			//Select file button
			Draw(mup_msg[15], 230, 180, 0.4, 0.4, color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER,
			80, 20, DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mup_select_file_button, mup_select_file_button.selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);

			//Shuffle button
			Draw(mup_msg[18], 12.5, 135, 0.5, 0.5, color);
			for (int i = 0; i < 2; i++)
			{
				Draw(mup_msg[10 + i], 10 + (i * 50), 150, 0.5, 0.5, mup_shuffle_request != i ? DEF_DRAW_RED : color, DEF_DRAW_X_ALIGN_CENTER, DEF_DRAW_Y_ALIGN_CENTER,
				40, 20, DEF_DRAW_BACKGROUND_ENTIRE_BOX, &mup_shuffle_button[i], mup_shuffle_button[i].selected ? DEF_DRAW_AQUA : DEF_DRAW_WEAK_AQUA);
			}

			if(Util_expl_query_show_flag())
				Util_expl_draw();

			if(Util_err_query_error_show_flag())
				Util_err_draw();

			Draw_bot_ui();
		}

		Draw_apply_draw();
	}
	else
		gspWaitForVBlank();
}

#include <3ds.h>
#include <unistd.h>
#include <string>

#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "log.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "types.hpp"
#include "setting_menu.hpp"
#include "music_player.hpp"
#include "explorer.hpp"
#include "httpc.hpp"
#include "util.hpp"

extern "C" {
#include "libavcodec/avcodec.h"
}

/*For draw*/
bool mup_need_reflesh = false;
bool mup_pre_loop_request = false;
bool mup_pre_select_file_request = false;
bool mup_pre_shuffle_request = false;
bool mup_pre_allow_sleep = false;
double mup_pre_music_length = 0.0;
float mup_pre_bar_pos = 0.0;
int mup_pre_music_sample_rate = 0;
int mup_pre_num_of_music_ch = 0;
int mup_pre_music_bit_rate = 0;
int mup_pre_dled_size = 0;
int mup_pre_dl_progress = 0;
std::string mup_pre_file_type = "unknown";
/*---------------------------------------------*/
int mup_debug = 0;
bool mup_main_run = false;
bool mup_play_thread_run = false;
bool mup_timer_thread_run = false;
bool mup_worker_thread_run = false;
bool mup_already_init = false;
bool mup_thread_suspend = true;
bool mup_play_request = false;
bool mup_stop_request = false;
bool mup_loop_request = false;
bool mup_count_request = false;
bool mup_count_reset_request = false;
bool mup_select_file_request = false;
bool mup_shuffle_request = false;
bool mup_allow_sleep = false;
bool mup_seek_request = false;
bool mup_change_music_request = false;
bool mup_dl_and_play_request = false;
bool mup_pause_request = false;
double mup_music_length = 0.0;
float mup_bar_pos = 0.0;
u64 mup_offset = 0;
int mup_music_sample_rate = 0;
int mup_num_of_music_ch = 0;
int mup_music_bit_rate = 0;
int mup_dled_size = 0;
int mup_dl_progress = 0;
std::string mup_url = "";
std::string mup_load_file_name = "";
std::string mup_load_dir_name = "/";
std::string mup_file_type = "unknown";
std::string mup_msg[MUP_NUM_OF_MSG];
std::string mup_worker_thread_string = "Mup/Worker thread";
std::string mup_play_thread_string = "Mup/Play thread";
std::string mup_timer_thread_string = "Mup/Timer thread";
std::string mup_init_string = "Mup/Init";
std::string mup_exit_string = "Mup/Exit";
std::string mup_ver = "v1.1.2";
Thread mup_play_thread, mup_timer_thread, mup_worker_thread;

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
	mup_need_reflesh = true;
	Menu_suspend();
}

void Mup_suspend(void)
{
	mup_thread_suspend = true;
	mup_main_run = false;
	Menu_resume();
}

void Mup_cancel_select_file(void)
{
	mup_select_file_request = false;
}

void Mup_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < MUP_NUM_OF_MSG)
		mup_msg[msg_num] = msg;
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
	mup_select_file_request = false;
}

Result_with_string Mup_play_sound(u8* sound_buffer, int buffer_size, int sample_rate, int num_of_ch, int play_ch)
{
	Result_with_string result;

	result.code = csndPlaySound(play_ch, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, (sample_rate * num_of_ch), 1.0, 0.0, (u32*)sound_buffer, NULL, buffer_size);
	if (result.code != 0)
		result.string = "csndPlaySound failed. ";

	return result;
}

void Mup_worker_thread(void* arg)
{
	Log_log_save(mup_worker_thread_string, "Thread started.", 1234567890, false);
	size_t cut_pos = std::string::npos;
	int log_num = 0;
	u8* httpc_buffer = NULL;
	u32 dl_size = 0;
	u32 status_code = 0;
	std::string cache_string = "";
	std::string last_url = "";
	Result_with_string result;

	while (mup_worker_thread_run)
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

			httpc_buffer = (u8*)malloc(0x20000);
			Log_log_save(mup_worker_thread_string, mup_url, 1234567890, false);
			log_num = Log_log_save(mup_worker_thread_string, "Httpc_dl_data()...", 1234567890, false);
			result = Httpc_dl_data(mup_url, httpc_buffer, 0x20000, &dl_size, &status_code, true, &last_url, false, 10, MUP_HTTP_PORT0, "/Line/audio/", cache_string);
			Log_log_add(log_num, result.string, result.code, false);
			free(httpc_buffer);
			httpc_buffer = NULL;

			if(result.code == 0)
			{
				mup_load_file_name = cache_string;
				mup_load_dir_name = "/Line/audio/";
				mup_play_request = true;
				mup_change_music_request = true;
			}
			mup_dl_and_play_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (mup_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	Log_log_save(mup_worker_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_timer_thread(void* arg)
{
	Log_log_save(mup_timer_thread_string, "Thread started.", 1234567890, false);
	TickCounter elapsed_time;
	osTickCounterStart(&elapsed_time);

	while (mup_timer_thread_run)
	{
		if (mup_count_reset_request)
		{
			mup_count_reset_request = false;
			osTickCounterUpdate(&elapsed_time);
			mup_bar_pos = 0.0;
		}

		if (mup_count_request)
		{
			osTickCounterUpdate(&elapsed_time);
			while (mup_count_request)
			{
				if (mup_count_reset_request)
					break;

				usleep(100000);
				osTickCounterUpdate(&elapsed_time);
				mup_bar_pos += osTickCounterRead(&elapsed_time);
				if(mup_bar_pos / 1000 > mup_music_length)
					mup_bar_pos = mup_music_length * 1000;
			}
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (mup_thread_suspend && !mup_count_request && !mup_count_reset_request)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	Log_log_save(mup_timer_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_play_thread(void* arg)
{
	Log_log_save(mup_play_thread_string, "Thread started.", 1234567890, false);
	bool init = true;
	bool has_audio = false;
	bool has_video = false;
	int buffer_num = 0;
	int random_num = 0;
	int file_index = 0;
	int audio_size = 0;
	int log_num = 0;
	u8* sound_buffer[5] = { NULL, NULL, NULL, NULL, NULL, };
	u8* sound_cache = NULL;
	std::string file_name = "";
	std::string dir_name = "/";
	ndspWaveBuf ndsp_buffer[5];
	Result_with_string result;
	AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
	for(int i = 0; i < 5; i++)
		sound_buffer[i] = (u8*)linearAlloc(0x5000);

	if(sound_buffer[0] == NULL || sound_buffer[1] == NULL || sound_buffer[2] == NULL || sound_buffer[3] == NULL || sound_buffer[4] == NULL)
	{
		Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), mup_play_thread_string, OUT_OF_MEMORY);
		Err_set_error_show_flag(true);
		Log_log_save(mup_play_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
		mup_play_thread_run = false;
	}

	while (mup_play_thread_run)
	{
		if (mup_play_request)
		{
			if(mup_change_music_request)
				mup_change_music_request = false;

			mup_stop_request = false;
			mup_seek_request = false;
			mup_pause_request = false;
			init = true;
			mup_count_reset_request = true;
			file_name = mup_load_file_name;
			dir_name = mup_load_dir_name;
			mup_offset = 0;
			Expl_set_current_patch(dir_name);
			Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
			while (Expl_query_operation_flag(EXPL_READ_DIR_REQUEST))
				usleep(50000);

			if(mup_shuffle_request)
			{
				file_index++;
				srand((unsigned) time(NULL));
				while (true)
				{
					random_num = rand() % 2;
					if(!(file_index < Expl_query_num_of_file()))
						file_index = 0;
					else if(random_num == 1 && Expl_query_type(file_index) == "file")
						break;
					else
						file_index++;
				}

				file_name = Expl_query_file_name(file_index);
			}

			result = Util_open_file(dir_name + file_name, &has_audio, &has_video, UTIL_DECODER_0);
			if(result.code != 0)
			{
				Err_set_error_message(result.string, result.error_description, mup_play_thread_string, result.code);
				Err_set_error_show_flag(true);
				Log_log_save(mup_play_thread_string, result.string, result.code, false);
			}
			else if(has_audio)
			{
				result = Util_init_audio_decoder(UTIL_DECODER_0);
				if(result.code != 0)
				{
					Err_set_error_message(result.string, result.error_description, mup_play_thread_string, result.code);
					Err_set_error_show_flag(true);
					Log_log_save(mup_play_thread_string, result.string, result.code, false);
				}
				else
				{
					while(true)
					{
						mup_count_request = false;
						while(mup_pause_request && !mup_stop_request && !mup_change_music_request && !mup_seek_request)
							usleep(50000);

						mup_count_request = true;
						if(mup_stop_request || mup_change_music_request || mup_seek_request)
						{
							ndspChnWaveBufClear(8);
							if(mup_seek_request)
							{
								//Use us(microsecond) to specify time
								log_num = Log_log_save(mup_play_thread_string, "Util_seek()... ", 1234567890, false);
								result = Util_seek(mup_offset, 4, UTIL_DECODER_0);//AVSEEK_FLAG_ANY
								Log_log_add(log_num, result.string, result.code, false);
								if(result.code >= 0)
									mup_bar_pos = mup_offset / 1000;

								mup_seek_request = false;
							}
							else
								break;
						}

						result = Util_read_packet(&type, UTIL_DECODER_0);
						if(result.code == 0)
						{
							if(type == AVMEDIA_TYPE_AUDIO)
							{
								result = Util_ready_audio_packet(UTIL_DECODER_0);
								if(result.code == 0)
								{
									free(sound_cache);
									sound_cache = NULL;
									result = Util_decode_audio(&audio_size, &sound_cache, UTIL_DECODER_0);
									if(result.code == 0)
									{
										if(init)
										{
											Util_get_audio_info(&mup_music_bit_rate, &mup_music_sample_rate, &mup_num_of_music_ch, &mup_file_type, &mup_music_length, UTIL_DECODER_0);

											ndspChnReset(8);
											ndspChnWaveBufClear(8);
											float mix[12];
											memset(mix, 0, sizeof(mix));
											mix[0] = 1.0;
											mix[1] = 1.0;
											ndspChnSetMix(8, mix);
											memset(ndsp_buffer, 0, sizeof(ndsp_buffer));
											if(mup_num_of_music_ch == 2)
											{
												ndspChnSetFormat(8, NDSP_FORMAT_STEREO_PCM16);
												ndspSetOutputMode(NDSP_OUTPUT_STEREO);
											}
											else
											{
												ndspChnSetFormat(8, NDSP_FORMAT_MONO_PCM16);
												ndspSetOutputMode(NDSP_OUTPUT_MONO);
											}
											
											ndspChnSetInterp(8, NDSP_INTERP_LINEAR);
											ndspChnSetRate(8, mup_music_sample_rate);
											init = false;
										}

										memcpy(sound_buffer[buffer_num], sound_cache, audio_size * mup_num_of_music_ch);
										
										ndsp_buffer[buffer_num].data_vaddr = sound_buffer[buffer_num];
										ndsp_buffer[buffer_num].nsamples = audio_size / 2;
										ndspChnWaveBufAdd(8, &ndsp_buffer[buffer_num]);

										if(buffer_num >= 0 && buffer_num <= 3)
											buffer_num++;
										else
											buffer_num = 0;

										while((ndsp_buffer[buffer_num].status == NDSP_WBUF_PLAYING || ndsp_buffer[buffer_num].status == NDSP_WBUF_QUEUED)
										&& !mup_stop_request && !mup_change_music_request && !mup_seek_request)
											usleep(7500);

										memset(sound_buffer[buffer_num], 0x0, audio_size);
									}
									else
										Log_log_save(mup_play_thread_string, "Util_decode_audio()..." + result.string + result.error_description, result.code, false);
								}
								else
									Log_log_save(mup_play_thread_string, "Util_ready_audio_packet()..." + result.string + result.error_description, result.code, false);
							}
							else if(type == AVMEDIA_TYPE_VIDEO)
								Util_skip_video_packet(UTIL_DECODER_0);
						}
						else
							break;
					}
				}
			}

			usleep(100000);

			Util_exit_audio_decoder(UTIL_DECODER_0);
			Util_close_file(UTIL_DECODER_0);
			mup_count_request = false;
			if (!mup_loop_request && !mup_change_music_request)
				mup_play_request = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (mup_thread_suspend && !mup_loop_request && !mup_change_music_request)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}

	free(sound_cache);
	sound_cache = NULL;
	for (int i = 0; i < 5; i++)
	{
		linearFree(sound_buffer[i]);
		sound_buffer[i] = NULL;
	}

	Log_log_save(mup_play_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_exit(void)
{
	Log_log_save(mup_exit_string, "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	Result_with_string result;

	if(mup_play_request)
		mup_stop_request = true;

	mup_already_init = false;
	mup_thread_suspend = false;
	mup_play_thread_run = false;
	mup_timer_thread_run = false;
	mup_worker_thread_run = false;

	aptSetSleepAllowed(true);
	Draw_progress("[Mup] Exiting...");

	for (int i = 0; i < 3; i++)
	{
		log_num = Log_log_save(mup_exit_string, "threadJoin()" + std::to_string(i) + "/1...", 1234567890, FORCE_DEBUG);

		if(i == 0)
			result.code = threadJoin(mup_play_thread, time_out);
		else if(i == 1)
			result.code = threadJoin(mup_timer_thread, time_out);
		else if(i == 2)
			result.code = threadJoin(mup_worker_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
		else
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
	}

	threadFree(mup_play_thread);
	threadFree(mup_timer_thread);
	threadFree(mup_worker_thread);
}

Result_with_string Mup_load_msg(std::string lang)
{
	u8* fs_buffer = NULL;
	u32 read_size;
	std::string setting_data[128];
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x2000);

	result = File_load_from_rom("mup_" + lang + ".txt", fs_buffer, 0x2000, &read_size, "romfs:/gfx/msg/");
	if (result.code != 0)
	{
		free(fs_buffer);
		return result;
	}

	result = Sem_parse_file((char*)fs_buffer, MUP_NUM_OF_MSG, setting_data);
	if (result.code != 0)
	{
		free(fs_buffer);
		return result;
	}

	for (int k = 0; k < MUP_NUM_OF_MSG; k++)
		Mup_set_msg(k, setting_data[k]);

	free(fs_buffer);
	return result;
}


void Mup_init(void)
{
	Log_log_save(mup_init_string, "Initializing...", 1234567890, FORCE_DEBUG);
	bool failed = false;
	Result_with_string result;

	Draw_progress("[Mup] Starting threads...");
	if (!failed)
	{
		mup_play_thread_run = true;
		mup_timer_thread_run = true;
		mup_worker_thread_run = true;
		mup_play_thread = threadCreate(Mup_play_thread, (void*)(""), STACKSIZE, PRIORITY_HIGH, 0, false);
		mup_timer_thread = threadCreate(Mup_timer_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, 1, false);
		mup_worker_thread = threadCreate(Mup_worker_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
	}

	aptSetSleepAllowed(false);
	mup_music_length = 0.0;
	mup_bar_pos = 0.0;
	mup_music_sample_rate = 0;
	mup_num_of_music_ch = 0;
	mup_music_bit_rate = 0;
	mup_file_type = "unknown";

	Mup_resume();
	mup_already_init = true;
	Log_log_save(mup_init_string, "Initialized", 1234567890, FORCE_DEBUG);
}

void Mup_main(void)
{
	int msg_num = 0;
	float text_red, text_green, text_blue, text_alpha;
	float red[2], green[2], blue[2], alpha[2];
	Hid_info key;

	if(mup_dl_and_play_request)
	{
		mup_dled_size = Httpc_query_dled_size(MUP_HTTP_PORT0);
		mup_dl_progress = Httpc_query_dl_progress(MUP_HTTP_PORT0);
	}

	if (Sem_query_settings(SEM_NIGHT_MODE))
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

	for (int i = 0; i < 2; i++)
	{
		red[i] = text_red;
		green[i] = text_green;
		blue[i] = text_blue;
		alpha[i] = text_alpha;
	}

	if(mup_pre_music_bit_rate != mup_music_bit_rate || mup_pre_music_sample_rate != mup_music_sample_rate || mup_pre_num_of_music_ch != mup_num_of_music_ch
		|| mup_pre_music_length != mup_music_length || mup_pre_file_type != mup_file_type || mup_pre_bar_pos != mup_bar_pos || mup_pre_loop_request != mup_loop_request
		|| mup_pre_select_file_request != mup_select_file_request || mup_pre_shuffle_request != mup_shuffle_request || mup_pre_allow_sleep != mup_allow_sleep
		|| mup_dled_size != mup_pre_dled_size || mup_dl_progress != mup_pre_dl_progress)
	{
		mup_pre_music_bit_rate = mup_music_bit_rate;
		mup_pre_music_sample_rate = mup_music_sample_rate;
		mup_pre_num_of_music_ch = mup_num_of_music_ch;
		mup_pre_music_length = mup_music_length;
		mup_pre_file_type = mup_file_type;
		mup_pre_bar_pos = mup_bar_pos;
		mup_pre_loop_request = mup_loop_request;
		mup_pre_select_file_request = mup_select_file_request;
		mup_pre_shuffle_request = mup_shuffle_request;
		mup_pre_allow_sleep = mup_allow_sleep;
		mup_pre_dled_size = mup_dled_size;
		mup_pre_dl_progress = mup_dl_progress;
		mup_need_reflesh = true;
	}

	Hid_query_key_state(&key);
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE) || Expl_query_need_reflesh())
		mup_need_reflesh = true;

	if(!mup_select_file_request)
		Hid_key_flag_reset();

	if(mup_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		if(mup_dl_and_play_request)
		{
			Draw_texture(Square_image, aqua_tint, 0, 0.0, 15.0, 50.0 * mup_dl_progress, 3.0);
			Draw(mup_msg[20] + std::to_string(mup_dled_size / 1024.0 / 1024.0).substr(0, 4) + "MB", 0, 0.0, 20.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		}
		Draw(mup_msg[0] + std::to_string(mup_music_bit_rate / 1000) + mup_msg[1], 0, 0.0, 35.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[2] + std::to_string((double)mup_music_sample_rate / 1000.0).substr(0, std::to_string((double)mup_music_sample_rate / 1000.0).length() - 4) + "Kbps", 0, 0.0, 50.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[3] + std::to_string(mup_num_of_music_ch) + mup_msg[4], 0, 0.0, 65.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[5] + Sem_convert_seconds_to_time(mup_music_length), 0, 0.0, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[7] + mup_file_type, 0, 0.0, 95.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		for (int i = 0; i < 2; i++)
			Draw_texture(Square_image, weak_aqua_tint, 0, 105.0 + (i * 60.0), 60.0, 50.0, 50.0);

		if(mup_play_request && !mup_pause_request)
			msg_num = 19;
		else
			msg_num = 16;
		
		Draw(mup_msg[msg_num], 0, 107.5, 80.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[17], 0, 167.5, 80.0, 0.45, 0.45, text_red, text_green, text_blue, text_alpha);

		Draw(mup_ver, 0, 0.0, 0.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);
		Draw(Sem_convert_seconds_to_time(mup_bar_pos / 1000) + " / " + Sem_convert_seconds_to_time(mup_music_length), 0, 12.5, 105.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_texture(Square_image, aqua_tint, 0, 10.0, 120.0, 300.0, 8.0);
		if(mup_music_length != 0.0)
			Draw_texture(Square_image, red_tint, 0, 10.0, 120.0, 300.0 * ((mup_bar_pos / 1000) / mup_music_length), 8.0);

		red[!mup_loop_request] = 1.0;
		green[!mup_loop_request] = 0.0;
		blue[!mup_loop_request] = 0.0;
		alpha[!mup_loop_request] = 1.0;

		Draw(mup_msg[9], 0, 12.5, 165.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 10.0 + (i * 50.0), 180.0, 40.0, 20.0);
			Draw(mup_msg[10 + i], 0, 12.5 + (i * 50.0), 180.0, 0.5, 0.5, red[i], green[i], blue[i], alpha[i]);
		}

		for (int i = 0; i < 2; i++)
		{
			red[i] = text_red;
			green[i] = text_green;
			blue[i] = text_blue;
			alpha[i] = text_alpha;
		}
		red[!mup_allow_sleep] = 1.0;
		green[!mup_allow_sleep] = 0.0;
		blue[!mup_allow_sleep] = 0.0;
		alpha[!mup_allow_sleep] = 1.0;

		Draw(mup_msg[12], 0, 122.5, 165.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 120.0 + (i * 50.0), 180.0, 40.0, 20.0);
			Draw(mup_msg[13 + i], 0, 122.5 + (i * 50.0), 180.0, 0.4, 0.4, red[i], green[i], blue[i], alpha[i]);
		}

		Draw_texture(Square_image, weak_aqua_tint, 0, 230.0, 180.0, 80.0, 20.0);
		Draw(mup_msg[15], 0, 232.5, 180.0, 0.4, 0.4, text_red, text_green, text_blue, text_alpha);

		for (int i = 0; i < 2; i++)
		{
			red[i] = text_red;
			green[i] = text_green;
			blue[i] = text_blue;
			alpha[i] = text_alpha;
		}
		red[!mup_shuffle_request] = 1.0;
		green[!mup_shuffle_request] = 0.0;
		blue[!mup_shuffle_request] = 0.0;
		alpha[!mup_shuffle_request] = 1.0;

		Draw(mup_msg[18], 0, 12.5, 135.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 10.0 + (i * 50.0), 150.0, 40.0, 20.0);
			Draw(mup_msg[10 + i], 0, 12.5 + (i * 50.0), 150.0, 0.5, 0.5, red[i], green[i], blue[i], alpha[i]);
		}

		if (mup_select_file_request)
			Expl_draw(mup_msg[8]);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		mup_need_reflesh = false;
	}
	else
		gspWaitForVBlank();

	if (Err_query_error_show_flag())
	{
		if (key.p_a || (key.p_touch && key.touch_x >= 150 && key.touch_x <= 169 && key.touch_y >= 150 && key.touch_y <= 169))
			Err_set_error_show_flag(false);
		else if(key.p_x || (key.p_touch && key.touch_x >= 200 && key.touch_x <= 239 && key.touch_y >= 150 && key.touch_y <= 169))
			Err_save_error();
	}
	else if (mup_select_file_request)
		Expl_main();
	else
	{
		if (key.p_start || (key.p_touch && key.touch_x >= 110 && key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Mup_suspend();
		else if(key.p_touch && key.touch_x >= 10 && key.touch_x <= 310 && key.touch_y >= 120 && key.touch_y <= 127)
		{
			mup_offset = (key.touch_x - 10) * (mup_music_length / 300);
			mup_offset *= 1000000;
			mup_seek_request = true;
		}
		if (key.p_a || (key.p_touch && key.touch_x >= 105 && key.touch_x <= 154 && key.touch_y >= 60 && key.touch_y <= 109))
		{
			if(!mup_play_request)
				mup_play_request = true;
			else if(!mup_pause_request)
				mup_pause_request = true;
			else
				mup_pause_request = false;
		}
		else if (key.p_b || (key.p_touch && key.touch_x >= 165 && key.touch_x <= 214 && key.touch_y >= 60 && key.touch_y <= 109))
			mup_stop_request = true;
		else if (key.p_touch && key.touch_x >= 10 && key.touch_x <= 49 && key.touch_y >= 150 && key.touch_y <= 169)
			mup_shuffle_request = true;
		else if (key.p_touch && key.touch_x >= 60 && key.touch_x <= 99 && key.touch_y >= 150 && key.touch_y <= 169)
			mup_shuffle_request = false;
		else if (key.p_touch && key.touch_x >= 10 && key.touch_x <= 49 && key.touch_y >= 180 && key.touch_y <= 199)
			mup_loop_request = true;
		else if (key.p_touch && key.touch_x >= 60 && key.touch_x <= 99 && key.touch_y >= 180 && key.touch_y <= 199)
			mup_loop_request = false;
		else if (key.p_touch && key.touch_x >= 110 && key.touch_x <= 149 && key.touch_y >= 180 && key.touch_y <= 199)
			Mup_set_allow_sleep(true);
		else if (key.p_touch && key.touch_x >= 160 && key.touch_x <= 199 && key.touch_y >= 180 && key.touch_y <= 199)
			Mup_set_allow_sleep(false);
		else if (key.p_x || (key.p_touch && key.touch_x >= 230 && key.touch_x <= 309 && key.touch_y >= 180 && key.touch_y <= 199))
		{
			void (*callback)(std::string, std::string);
			void (*cancel_callback)(void);
			callback = Mup_set_load_file;
			cancel_callback = Mup_cancel_select_file;
			Expl_set_callback(callback);
			Expl_set_cancel_callback(cancel_callback);
			mup_select_file_request = true;
			Expl_set_operation_flag(EXPL_READ_DIR_REQUEST, true);
		}
	}
}

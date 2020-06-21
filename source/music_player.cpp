#include <3ds.h>
#include <unistd.h>
#include <string>

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "minimp3/minimp3_ex.h"

#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "log.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "types.hpp"
#include "share_function.hpp"
#include "setting_menu.hpp"
#include "explorer.hpp"
#include "music_player.hpp"

bool mup_main_run = false;
bool mup_play_thread_run = false;
bool mup_timer_thread_run = false;
bool mup_already_init = false;
bool mup_thread_suspend = true;
bool mup_play_request = false;
bool mup_stop_request = false;
bool mup_loop_request = false;
bool mup_count_request = false;
bool mup_count_reset_request = false;
bool mup_select_file_request = false;
bool mup_allow_sleep = false;
double mup_music_length = 0.0;
float mup_bar_pos = 0.0;
int mup_music_sample_rate = 0;
int mup_num_of_music_ch = 0;
int mup_music_bit_rate = 0;
int mup_sound_fs_out_buffer_size = 0x100000;
int mup_sound_fs_in_buffer_size = 0x100000;
std::string mup_load_file_name = "";
std::string mup_load_dir_name = "";
std::string mup_file_type = "unknown";
std::string mup_msg[MUP_NUM_OF_MSG];
Thread mup_play_thread, mup_timer_thread;

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
	Menu_suspend();
}

void Mup_suspend(void)
{
	mup_thread_suspend = true;
	mup_main_run = false;
	Menu_resume();
}

int Mup_query_buffer_size(int buffer_num)
{
	if (buffer_num == MUP_FS_OUT_BUFFER)
		return mup_sound_fs_out_buffer_size;
	else if (buffer_num == MUP_FS_IN_BUFFER)
		return mup_sound_fs_in_buffer_size;
	else
		return -1;
}

bool Mup_query_operation_flag(int operation_num)
{
	if (operation_num == MUP_PLAY_MUSIC_REQUEST)
		return mup_play_request;
	else if (operation_num == MUP_STOP_MUSIC_REQUEST)
		return mup_stop_request;
	else if (operation_num == MUP_SELECT_FILE_REQUEST)
		return mup_select_file_request;
	else if (operation_num == MUP_LOOP_REQUEST)
		return mup_loop_request;
	else
		return false;
}

void Mup_set_buffer_size(int buffer_num, int size)
{
	if (buffer_num == MUP_FS_OUT_BUFFER)
		mup_sound_fs_out_buffer_size = size;
	else if (buffer_num == MUP_FS_IN_BUFFER)
		mup_sound_fs_in_buffer_size = size;
}

void Mup_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num < MUP_NUM_OF_MSG)
		mup_msg[msg_num] = msg;
}

void Mup_set_operation_flag(int operation_num, bool flag)
{
	if (operation_num == MUP_PLAY_MUSIC_REQUEST)
		mup_play_request = flag;
	else if (operation_num == MUP_STOP_MUSIC_REQUEST)
		mup_stop_request = flag;
	else if (operation_num == MUP_SELECT_FILE_REQUEST)
		mup_select_file_request = flag;
	else if (operation_num == MUP_LOOP_REQUEST)
		mup_loop_request = flag;
}

void Mup_set_allow_sleep(bool flag)
{
	mup_allow_sleep = flag;
	aptSetSleepAllowed(mup_allow_sleep);
}

void Mup_set_load_file_name(std::string file_name)
{
	mup_load_file_name = file_name;
}

void Mup_set_load_dir_name(std::string dir_name)
{
	mup_load_dir_name = dir_name;
}

Result_with_string Mup_play_sound(u8* sound_buffer, int buffer_size, int sample_rate, int num_of_ch)
{
	Result_with_string result;
	result.code = 0;
	result.string = s_success;

	result.code = GSPGPU_FlushDataCache(sound_buffer, buffer_size);

	result.code = csndPlaySound(31, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, (sample_rate * num_of_ch), 1.0, 0.0, (u32*)sound_buffer, NULL, buffer_size);
	if (result.code != 0)
		result.string = "csndPlaySound failed. ";

	return result;
}

void Mup_timer_thread(void* arg)
{
	Log_log_save("Mup/Timer thread", "Thread started.", 1234567890, false);
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
		
		while (mup_count_request)
		{
			if (mup_count_reset_request)
				break;

			usleep(10000);
			osTickCounterUpdate(&elapsed_time);
			mup_bar_pos += osTickCounterRead(&elapsed_time);
		}
		usleep(10000);
	}

	Log_log_save("Mup/Timer thread", "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_play_thread(void* arg)
{
	Log_log_save("Mup/Play thread", "Thread started.", 1234567890, false);
	int buffer_num = 0;
	int file_size = 0;
	int raw_buffer_size = 0;
	int mp3_result = 0;
	int mp3_estimated_output = 0;
	int mp3_input_size = 128 * 1024;
	double multiple[2] = { 0.0, 0.0 };
	u8* sound_buffer[2];
	u8* sound_header[2];
	u8* sound_in_buffer;
	u8* wav_sample;
	u8* mp3_sample;
	u8 status;
	u32 read_size;
	u64 mp3_file_size = 0;
	std::string file_name = "";
	std::string dir_name = "/";
	Handle fs_handle = 0;
	FS_Archive fs_archive = 0;
	Result_with_string result;

	sound_buffer[0] = (u8*)linearAlloc(mup_sound_fs_out_buffer_size / 2);
	sound_buffer[1] = (u8*)linearAlloc(mup_sound_fs_out_buffer_size / 2);
	sound_in_buffer = (u8*)malloc(mup_sound_fs_in_buffer_size);
	sound_header[0] = (u8*)malloc(0x4000);
	sound_header[1] = (u8*)malloc(0x4);
	wav_sample = (u8*)malloc(0x4);
	mp3_sample = (u8*)malloc(0x3);
	memset(wav_sample, 0x52, 0x1);
	memset((wav_sample + 1), 0x49, 0x1);
	memset((wav_sample + 2), 0x46, 0x1);
	memset((wav_sample + 3), 0x46, 0x1);
	memset(mp3_sample, 0x49, 0x1);
	memset((mp3_sample + 1), 0x44, 0x1);
	memset((mp3_sample + 2), 0x33, 0x1);

	while (mup_play_thread_run)
	{
		if (mup_play_request)
		{
			mup_count_reset_request = true;
			file_name = mup_load_file_name;
			dir_name = mup_load_dir_name;

			mup_bar_pos = 0.0;
			mp3_input_size = mup_sound_fs_in_buffer_size;
			file_size = 0;
			memset(sound_buffer[0], 0x0, mup_sound_fs_out_buffer_size / 2);
			memset(sound_buffer[1], 0x0, mup_sound_fs_out_buffer_size / 2);
			memset(sound_in_buffer, 0x0, mup_sound_fs_in_buffer_size);
			memset(sound_header[0], 0x0, 0x4000);
			memset(sound_header[1], 0x0, 0x4);

			mp3dec_t mp3_decode;
			mp3dec_init(&mp3_decode);
			mp3dec_file_info_t mp3_info;
			MP3D_PROGRESS_CB mp3_progress;

			result = File_load_from_file_with_range(mup_load_file_name, sound_header[0], 0x4000, 0, &read_size, mup_load_dir_name, fs_handle, fs_archive);

			if (result.code != 0)
			{
				Log_log_save("", "File_load_from_file..." + result.string, result.code, false);
				break;
			}

			memcpy((void*)sound_header[1], (void*)(sound_header[0]), 0x4);

			mup_file_type = (char*)sound_header[1];
			if (memcmp(mp3_sample, sound_header[1], 3) == 0)
				mup_file_type = "mp3";
			else if (memcmp(wav_sample, sound_header[1], 4) == 0)
				mup_file_type = "wav";
			else
				mup_file_type = "unsupported";

			if (mup_file_type == "wav")
			{
				memset(sound_header[1], 0x0, 0x4);
				memcpy((void*)sound_header[1], (void*)(sound_header[0] + 4), 0x4);//file size
				file_size = *(int*)sound_header[1];

				memset(sound_header[1], 0x0, 0x4);
				memcpy((void*)sound_header[1], (void*)(sound_header[0] + 22), 0x2);//ch
				mup_num_of_music_ch = *(int*)sound_header[1];

				memset(sound_header[1], 0x0, 0x4);
				memcpy((void*)sound_header[1], (void*)(sound_header[0] + 24), 0x4);//sample rate
				mup_music_sample_rate = *(int*)sound_header[1];

				memset(sound_header[1], 0x0, 0x4);
				memcpy((void*)sound_header[1], (void*)(sound_header[0] + 28), 0x4);//byte rate
				mup_music_bit_rate = *(int*)sound_header[1];
				mup_music_bit_rate = mup_music_bit_rate * 8;
				mup_music_length = (double)file_size / ((double)mup_music_bit_rate / 8);
			}
			else if (mup_file_type == "mp3")
			{
				result = File_check_file_size(file_name, dir_name, &mp3_file_size, fs_handle, fs_archive);
				mp3_result = mp3dec_load_buf(&mp3_decode, (const uint8_t*)sound_header[0], read_size, &mp3_info, mp3_progress, NULL, 0x50000);
				if (result.code == 0)
				{
					mup_num_of_music_ch = mp3_info.channels;//ch
					mup_music_sample_rate = mp3_info.hz;//sample rate
					mup_music_bit_rate = mp3_info.avg_bitrate_kbps;//kb rate
					mup_music_bit_rate = mup_music_bit_rate * 1000;//bit rate
					mup_music_length = (double)((mp3_file_size - mp3_info.samples) * 8) / (double)mup_music_bit_rate;
					multiple[0] = (double)mup_music_sample_rate / 48000.0;
					multiple[1] = 320.0 / (double)(mup_music_bit_rate / 1000);

					while (true)
					{
						mp3_estimated_output = mp3_input_size * 2.4 * multiple[0] * multiple[1] * mup_num_of_music_ch;
						if ((mp3_estimated_output <= (int)mup_sound_fs_out_buffer_size / 2 && mp3_input_size <= 0x100000) || mp3_input_size <= 1)
							break;
						else
							mp3_input_size = mp3_input_size / 2;
					}
				}
			}

			for (int i = 0; i < 10000; i++)
			{
				if (mup_file_type == "wav")
				{
					result = File_load_from_file_with_range(file_name, sound_buffer[buffer_num], mup_sound_fs_out_buffer_size / 2, (i * (mup_sound_fs_out_buffer_size / 2)), &read_size, dir_name, fs_handle, fs_archive);

					if(result.code != 0)
						Log_log_save("", "File_load_from_file..." + result.string, result.code, false);

					raw_buffer_size = read_size;
				}
				else if (mup_file_type == "mp3")
				{
					result = File_load_from_file_with_range(file_name, sound_in_buffer, mp3_input_size, (i * mp3_input_size), &read_size, dir_name, fs_handle, fs_archive);

					if (result.code == 0)
					{
						free(mp3_info.buffer);
						mp3_info.buffer = NULL;
						mp3_result = mp3dec_load_buf(&mp3_decode, (const uint8_t*)sound_in_buffer, read_size, &mp3_info, mp3_progress, NULL, mup_sound_fs_out_buffer_size / 2);
						if(mp3_result != 0)
							Log_log_save("", "mp3dec_load_buf()...input " + std::to_string(read_size / 1024) + "KB. ""decoded " + std::to_string(mp3_info.samples * 2 / 1024) + "KB [Error] ", mp3_result, false);

						memset(sound_buffer[buffer_num], 0x0, mup_sound_fs_out_buffer_size / 2);
						memcpy((void*)(sound_buffer[buffer_num]), mp3_info.buffer, mp3_info.samples * 2);
						
						raw_buffer_size = mp3_info.samples * 2;
					}
					else
						Log_log_save("", "File_load_from_file..." + result.string, result.code, false);
				}
				else
					break;

				while (true)
				{
					csndIsPlaying(31, &status);
					if (status == 0 || !mup_play_thread_run || mup_stop_request)
						break;

					usleep(7500);
				}

				if (!mup_play_thread_run || mup_stop_request || result.code != 0)
				{
					CSND_SetPlayState(31, 0);
					CSND_UpdateInfo(0);
					break;
				}

				mup_count_request = false;
				if (raw_buffer_size > 0)
				{
					result = Mup_play_sound(sound_buffer[buffer_num], raw_buffer_size, mup_music_sample_rate, mup_num_of_music_ch);

					if (buffer_num == 0)
						buffer_num = 1;
					else if (buffer_num == 1)
						buffer_num = 0;
				}
				mup_count_request = true;
			}

			mup_count_request = false;
			if (!mup_loop_request)
				mup_play_request = false;
		}
		mup_stop_request = false;
		usleep(250000);
	}

	free(sound_in_buffer);
	free(wav_sample);
	free(mp3_sample);
	sound_in_buffer = NULL;
	mp3_sample = NULL;
	wav_sample = NULL;
	for (int i = 0; i < 2; i++)
	{
		linearFree(sound_buffer[i]);
		free(sound_header[i]);
		sound_buffer[i] = NULL;
		sound_header[i] = NULL;
	}

	Log_log_save("Mup/Play thread", "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_exit(void)
{
	Log_log_save("Mup/Init", "Exiting...", 1234567890, s_debug_slow);
	u64 time_out = 10000000000;
	int log_num;
	bool failed = false;
	Result_with_string result;

	mup_already_init = false;
	mup_thread_suspend = false;
	mup_play_thread_run = false;
	mup_timer_thread_run = false;

	aptSetSleepAllowed(true);
	Draw_progress("[Mup] Exiting...");

	for (int i = 0; i < 2; i++)
	{
		log_num = Log_log_save("Mup/Exit", "Exiting thread(" + std::to_string(i) + "/1)...", 1234567890, s_debug_slow);
	
		if(i == 0)
			result.code = threadJoin(mup_play_thread, time_out);
		else if(i == 1)
			result.code = threadJoin(mup_timer_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, s_success, result.code, s_debug_slow);
		else
		{
			failed = true;
			Log_log_add(log_num, s_error, result.code, s_debug_slow);
		}
	}

	threadFree(mup_play_thread);
	threadFree(mup_timer_thread);

	if (failed)
		Log_log_save("Cam/Exit", "[Warn] Some function returned error.", 1234567890, s_debug_slow);
}

void Mup_init(void)
{
	Log_log_save("Mup/Init", "Initializing...", 1234567890, s_debug_slow);
	bool failed = false;
	int log_num;
	Result_with_string result;

	Draw_progress("0/1 [Mup] Initializing service...");

	log_num= Log_log_save("Mup/Init", "csndInit...", 1234567890, s_debug_slow);
	result.code = csndInit();
	if (result.code == 0)
		Log_log_add(log_num, s_success, result.code, s_debug_slow);
	else
	{
		failed = true;
		Err_set_error_message("csndInit failed.", "", "Mup/Init", result.code);
		Err_set_error_show_flag(true);
		Log_log_add(log_num,s_error, result.code, s_debug_slow);
	}

	Draw_progress("1/1 [Mup] Starting threads...");
	if (!failed)
	{
		mup_play_thread_run = true;
		mup_timer_thread_run = true;
		mup_play_thread = threadCreate(Mup_play_thread, (void*)(""), STACKSIZE, 0x25, -1, false);
		mup_timer_thread = threadCreate(Mup_timer_thread, (void*)(""), STACKSIZE, 0x24, -1, false);
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
	Log_log_save("Mup/Init", "Initialized", 1234567890, s_debug_slow);
}

void Mup_main(void)
{
	int log_y = Log_query_y();
	double log_x = Log_query_x();
	float text_red, text_green, text_blue, text_alpha;
	float red[2], green[2], blue[2], alpha[2];

	if (Sem_query_settings(SEM_NIGHT_MODE))
	{
		text_red = 1.0f;
		text_green = 1.0f;
		text_blue = 1.0f;
		text_alpha = 0.75f;
		white_or_black_tint = white_tint;
	}
	else
	{
		text_red = 0.0f;
		text_green = 0.0f;
		text_blue = 0.0f;
		text_alpha = 1.0f;
		white_or_black_tint = black_tint;
	}

	for (int i = 0; i < 2; i++)
	{
		red[i] = text_red;
		green[i] = text_green;
		blue[i] = text_blue;
		alpha[i] = text_alpha;
	}

	Draw_set_draw_mode(Sem_query_settings(SEM_VSYNC_MODE));
	if (Sem_query_settings(SEM_NIGHT_MODE))
		Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
	else
		Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

	Draw_top_ui();
	Draw(mup_msg[0] + std::to_string(mup_music_bit_rate / 1000) + mup_msg[1], 0, 0.0, 20.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(mup_msg[2] + std::to_string((double)mup_music_sample_rate / 1000.0).substr(0, std::to_string((double)mup_music_sample_rate / 1000.0).length() - 4) + "Kbps", 0, 0.0, 35.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(mup_msg[3] + std::to_string(mup_num_of_music_ch) + mup_msg[4], 0, 0.0, 50.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(mup_msg[5] + std::to_string(mup_music_length).substr(0, std::to_string(mup_music_length).length() - 4) + mup_msg[6], 0, 0.0, 65.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw(mup_msg[7] + mup_file_type, 0, 0.0, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	
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

	for (int i = 0; i < 2; i++)
	{
		Draw_texture(Square_image, weak_aqua_tint, 0, 105.0 + (i * 60.0), 60.0, 50.0, 50.0);
		Draw(mup_msg[16 + i], 0, 107.5 + (i * 60.0), 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	}

	Draw(s_mup_ver, 0, 0.0, 0.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);	
	Draw(std::to_string(mup_bar_pos / 1000).substr(0, std::to_string(mup_bar_pos / 1000).length() - 4) + "/" + std::to_string(mup_music_length).substr(0, std::to_string(mup_music_length).length() - 4), 0, 12.5, 105.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
	Draw_texture(Square_image, aqua_tint, 0, 10.0, 120.0, 300.0, 5.0);
	if(mup_music_length != 0.0)
		Draw_texture(Square_image, red_tint, 0, 10.0, 120.0, 300.0 * ((mup_bar_pos / 1000) / mup_music_length), 5.0);

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
	if (mup_select_file_request)
	{
		Draw_texture(Square_image, aqua_tint, 10, 10.0, 20.0, 300.0, 190.0);
		Draw(mup_msg[8], 0, 12.5, 185.0, 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		Draw(Expl_query_current_patch(), 0, 12.5, 195.0, 0.45, 0.45, 0.0, 0.0, 0.0, 1.0);
		for (int i = 0; i < 16; i++)
		{
			if (i == (int)Expl_query_selected_num(EXPL_SELECTED_FILE_NUM))
				Draw(Expl_query_file_name(i + (int)Expl_query_view_offset_y()) + "(" + Expl_query_type(i + (int)Expl_query_view_offset_y()) + ")", 0, 12.5, 20.0 + (i * 10.0), 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
			else
				Draw(Expl_query_file_name(i + (int)Expl_query_view_offset_y()) + "(" + Expl_query_type(i + (int)Expl_query_view_offset_y()) + ")", 0, 12.5, 20.0 + (i * 10.0), 0.4, 0.4, 0.0, 0.0, 0.0, 1.0);
		}
	}

	if (Err_query_error_show_flag())
		Draw_error();

	Draw_bot_ui();

	if (Hid_query_key_held_state(KEY_H_TOUCH))
		Draw(s_circle_string, 0, Hid_query_touch_pos(true), Hid_query_touch_pos(false), 0.20f, 0.20f, 1.0f, 0.0f, 0.0f, 1.0f);
	s_fps += 1;

	Draw_apply_draw();
	osTickCounterUpdate(&s_tcount_frame_time);
	s_frame_time = osTickCounterRead(&s_tcount_frame_time);
}

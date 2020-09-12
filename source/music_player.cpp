#include <3ds.h>
#include <unistd.h>
#include <string>

#define BitVal(data,y) ( (data>>y) & 1)      //Return Data.Y value
#define SetBit(data,y)    data |= (1 << y)    //Set Data.Y   to 1
#define ClearBit(data,y)  data &= ~(1 << y)   //Clear Data.Y to 0
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
#include "setting_menu.hpp"
#include "music_player.hpp"
#include "explorer.hpp"

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
std::string mup_pre_file_type = "unknown";
/*---------------------------------------------*/

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
bool mup_shuffle_request = false;
bool mup_allow_sleep = false;
bool mup_seek_request = false;
bool mup_change_music_request = false;
double mup_music_length = 0.0;
float mup_bar_pos = 0.0;
int mup_music_sample_rate = 0;
int mup_num_of_music_ch = 0;
int mup_music_bit_rate = 0;
int mup_file_offset = 0;
int mup_file_size = 0;
int mup_sound_fs_out_buffer_size = 0x100000;
int mup_sound_fs_in_buffer_size = 0x100000;
std::string mup_load_file_name = "";
std::string mup_load_dir_name = "/";
std::string mup_file_type = "unknown";
std::string mup_msg[MUP_NUM_OF_MSG];
std::string mup_play_thread_string = "Mup/Play thread";
std::string mup_timer_thread_string = "Mup/Timer thread";
std::string mup_init_string = "Mup/Init";
std::string mup_exit_string = "Mup/Exit";
std::string mup_ver = "v1.0.3";
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
	mup_need_reflesh = true;
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
	else if(operation_num == MUP_SHUFFLE_REQUEST)
		return mup_shuffle_request;
	else if(operation_num == MUP_CHANGE_MUSIC_REQUEST)
		return mup_change_music_request;
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

void Mup_set_offset(int offset)
{
	if(offset >= 0 && offset <= 300)
	{
		mup_file_offset = offset * (mup_file_size / 300);
		if(mup_file_offset % 2 != 0)
			mup_file_offset++;

		mup_seek_request = true;
	}
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
	else if(operation_num == MUP_SHUFFLE_REQUEST)
		mup_shuffle_request = flag;
	else if(operation_num == MUP_CHANGE_MUSIC_REQUEST)
		mup_change_music_request = flag;
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

	result.code = csndPlaySound(31, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, (sample_rate * num_of_ch), 1.0, 0.0, (u32*)sound_buffer, NULL, buffer_size);
	if (result.code != 0)
		result.string = "csndPlaySound failed. ";

	return result;
}

bool Mup_detect_wave(u8* buffer, int size)
{
	bool wav = false;
	u8* wav_sample;
	wav_sample = (u8*)malloc(0x4);
	memset(wav_sample, 0x52, 0x1);
	memset((wav_sample + 1), 0x49, 0x1);
	memset((wav_sample + 2), 0x46, 0x1);
	memset((wav_sample + 3), 0x46, 0x1);

	if(size >= 4)
	{
		if (memcmp(wav_sample, buffer, 4) == 0)
			wav = true;
	}

	free(wav_sample);
	wav_sample = NULL;
	return wav;
}

bool Mup_detect_mp3(u8* buffer, int size)
{
	bool mp3 = false;
	u8* mp3_sample[2];
	mp3_sample[0] = (u8*)malloc(0x3);
	mp3_sample[1] = (u8*)malloc(0x1);
	memset(mp3_sample[0], 0x49, 0x1);
	memset((mp3_sample[0] + 1), 0x44, 0x1);
	memset((mp3_sample[0] + 2), 0x33, 0x1);
	memset(mp3_sample[1], 0xFF, 0x1);

	if(size >= 3)
	{
		if((memcmp(mp3_sample[0], buffer, 3) == 0) || (memcmp(mp3_sample[1], buffer, 1) == 0))
			mp3 = true;
	}

	free(mp3_sample[0]);
	free(mp3_sample[1]);
	mp3_sample[0] = NULL;
	mp3_sample[1] = NULL;
	return mp3;
}

void Mup_parse_wav(u8* header_buffer, int buffer_size, int* file_size, int* channels, int* samplerate, int* bitrate, int* raw_data_pos)
{
	u8* cache;
	cache = (u8*)malloc(0x4);

	if(buffer_size >= 32)
	{
		memset(cache, 0x0, 0x4);
		memcpy((void*)cache, (void*)(header_buffer + 4), 0x4);//file size
		*file_size = *(int*)cache;

		memset(cache, 0x0, 0x4);
		memcpy((void*)cache, (void*)(header_buffer + 22), 0x2);//ch
		*channels = *(int*)cache;

		memset(cache, 0x0, 0x4);
		memcpy((void*)cache, (void*)(header_buffer + 24), 0x4);//sample rate
		*samplerate = *(int*)cache;

		memset(cache, 0x0, 0x4);
		memcpy((void*)cache, (void*)(header_buffer + 28), 0x4);//byte rate
		*bitrate = *(int*)cache;
		*bitrate *= 8;

		memset(cache, 0x64, 0x1);
		memset((cache + 1), 0x61, 0x1);
		memset((cache + 2), 0x74, 0x1);
		memset((cache + 3), 0x61, 0x1);
		*raw_data_pos = 0;

		for(int i = 0; i + 4 <= buffer_size; i++)
		{
			if(memcmp(cache, (header_buffer + i), 4) == 0)
			{
				*raw_data_pos = i + 8;
				break;
			}
		}
	}

	free(cache);
	cache = NULL;
}

void Mup_parse_mp3(u8* header_buffer, int buffer_size, int* channels, int* samplerate, int* bitrate, int* block_size, int* first_block_pos)
{
	u8* cache[2];
	uint8_t layer = 0;
	uint8_t mpeg = 0;
	uint8_t bitrate_index = 0;
	uint8_t samplerate_index = 0;
	uint8_t ch = 0;
	int bitrate_list[70] = {
		32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,
		32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384,
		32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320,
		32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256,
		 8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,
	};
	int samplerate_list[9] = {
		44100, 48000, 32000,
		22050, 24000, 16000,
		11025, 12000,  8000,
	};

	cache[0] = (u8*)malloc(0x1);
	cache[1] = (u8*)malloc(0x1);
	memset(cache[0], 0xFF, 0x1);
	memset(cache[1], 0xEF, 0x1);

	for(int i = 0; i + 4 <= buffer_size; i++)
	{
		if(memcmp(cache[0], (header_buffer + i), 1) == 0)
		{
			if(memcmp(cache[1], (header_buffer + (i + 1)), 1) <= 0)
			{
				*first_block_pos = i;
				break;
			}
		}
	}

	mpeg = *(uint8_t*)((void*)(header_buffer + *first_block_pos + 1));
	mpeg = mpeg << 3;
	mpeg = mpeg >> 6;
	if(mpeg == 3)
		mpeg = 1;
	else if(mpeg == 2)
		mpeg = 2;
	else if(mpeg == 1)
		mpeg = 0;
	else if(mpeg == 0)
		mpeg = 3;//mpeg 2.5

	layer = *(uint8_t*)((void*)(header_buffer + *first_block_pos + 1));
	layer = layer << 5;
	layer = layer >> 6;
	if(layer == 3)
		layer = 1;
	else if(layer == 2)
		layer = 2;
	else if(layer == 1)
		layer = 3;
	else if(layer == 0)
		layer = 0;

	bitrate_index = *(uint8_t*)((void*)(header_buffer + *first_block_pos + 2));
	bitrate_index = bitrate_index >> 4;
	if(mpeg >= 1 && mpeg <= 3 && layer >= 1 && layer <= 3 && bitrate_index >= 1 && bitrate_index <= 14)
	{
		if(mpeg == 1)
			*bitrate = bitrate_list[(14 * (layer - 1)) + (bitrate_index - 1)];
		else if(mpeg == 2 || mpeg == 3)
		{
			if(layer == 1)
				*bitrate = bitrate_list[42 + (bitrate_index - 1)];
			else if(layer == 2 || layer == 3)
				*bitrate = bitrate_list[56 + (bitrate_index - 1)];
		}
		*bitrate *= 1000;
	}

	samplerate_index = *(uint8_t*)((void*)(header_buffer + *first_block_pos + 2));
	samplerate_index = samplerate_index << 4;
	samplerate_index = samplerate_index >> 6;
	if(mpeg >= 1 && mpeg <= 3 && samplerate_index >= 0 && samplerate_index <= 2)
		*samplerate = samplerate_list[(3 * (mpeg - 1)) + samplerate_index];

	*block_size = 144 * *bitrate / *samplerate;

	ch = *(uint8_t*)((void*)(header_buffer + *first_block_pos + 3));
	ch = ch >> 6;
	if(ch == 3)
		*channels = 1;
	else if(ch >= 0 && ch <= 2)
		*channels = 2;

	for(int i = 0; i < 2; i++)
	{
		free(cache[i]);
		cache[i] = NULL;
	}
}

double Mup_calc_mp3_length(long file_size, int bitrate)
{
	return (double)(file_size * 8.0) / bitrate;
}

double Mup_calc_wav_length(long file_size, int bitrate)
{
	return (double)file_size / (bitrate / 8.0);
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
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (mup_thread_suspend && !mup_count_request && !mup_count_reset_request)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}

	Log_log_save(mup_timer_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_play_thread(void* arg)
{
	Log_log_save(mup_play_thread_string, "Thread started.", 1234567890, false);
	int log_num = 0;
	int buffer_num = 0;
	int raw_buffer_size = 0;
	int mp3_result = 0;
	int mp3_estimated_output = 0;
	int mp3_input_size = 0;
	int random_num = 0;
	int previous_random_num = -1;
	int count = 0;
	int block_size = 0;
	u8* sound_buffer[2];
	u8* sound_header;
	u8* sound_in_buffer;
	u8 status;
	u32 read_size;
	u64 mp3_size = 0;
	std::string file_name = "";
	std::string dir_name = "/";
	Handle fs_handle = 0;
	FS_Archive fs_archive = 0;
	Result_with_string result;
	mp3dec_t mp3_decode;
	mp3dec_file_info_t mp3_info;
	mp3dec_init(&mp3_decode);

	mp3_info.buffer = NULL;
	sound_buffer[0] = (u8*)linearAlloc(mup_sound_fs_out_buffer_size / 2);
	sound_buffer[1] = (u8*)linearAlloc(mup_sound_fs_out_buffer_size / 2);
	sound_in_buffer = (u8*)malloc(mup_sound_fs_in_buffer_size);
	sound_header = (u8*)malloc(0x4000);
	if(sound_buffer[0] == NULL || sound_buffer[1] == NULL || sound_in_buffer == NULL)
	{
		Err_set_error_message(Err_query_template_summary(OUT_OF_MEMORY), Err_query_template_detail(OUT_OF_MEMORY), mup_play_thread_string, OUT_OF_MEMORY);
		Err_set_error_show_flag(true);
		Log_log_save(mup_play_thread_string, Err_query_template_summary(OUT_OF_MEMORY), OUT_OF_MEMORY, false);
		mup_play_thread_run = false;
	}

	while (mup_play_thread_run)
	{
		mup_stop_request = false;
		if (mup_play_request)
		{
			if(mup_change_music_request)
				mup_change_music_request = false;

			mup_count_reset_request = true;
			file_name = mup_load_file_name;
			dir_name = mup_load_dir_name;
			mup_bar_pos = 0.0;
			mup_file_size = 0;
			mup_file_offset = 0;
			mp3_size = 0;
			mp3_input_size = mup_sound_fs_in_buffer_size;
			count = 0;
			memset(sound_buffer[0], 0x0, mup_sound_fs_out_buffer_size / 2);
			memset(sound_buffer[1], 0x0, mup_sound_fs_out_buffer_size / 2);
			memset(sound_in_buffer, 0x0, mup_sound_fs_in_buffer_size);
			memset(sound_header, 0x0, 0x4000);

			if(mup_shuffle_request)
			{
				do {
					srand(time(NULL) + count);
					random_num = rand() % Expl_query_num_of_file();
					count++;
					if(count >= 50)
						break;
				}
				while(Expl_query_type(random_num) != "file" || previous_random_num == random_num);

				previous_random_num = random_num;
				file_name = Expl_query_file_name(random_num);
			}

			log_num = Log_log_save(mup_play_thread_string, "File_load_from_file_with_range()...", 1234567890, false);
			result = File_load_from_file_with_range(file_name, sound_header, 0x4000, 0, &read_size, dir_name, fs_handle, fs_archive);
			Log_log_add(log_num, result.string, result.code, false);

			if (result.code != 0)
			{
				Err_set_error_message(result.string, result.error_description, mup_play_thread_string, result.code);
				Err_set_error_show_flag(true);
				mup_play_request = false;
			}
			else
			{
				if (Mup_detect_mp3(sound_header, 3))
				{
					mup_file_type = "mp3";
					Mup_parse_mp3(sound_header, (int)read_size, &mup_num_of_music_ch, &mup_music_sample_rate, &mup_music_bit_rate, &block_size, &mup_file_offset);
					File_check_file_size(file_name, dir_name, &mp3_size, fs_handle, fs_archive);
					mup_file_size = (int)mp3_size;
					mup_music_length = Mup_calc_mp3_length(mup_file_size, mup_music_bit_rate);

					for(int i = 20000; i > 1; i--)
					{
						mp3_estimated_output = (i + 2) * 4608;
						if ((mp3_estimated_output <= (int)mup_sound_fs_out_buffer_size / 2 && block_size * i <= mup_sound_fs_in_buffer_size) || i == 1)
						{
							mp3_input_size = block_size * i;
							break;
						}
					}
				}
				else if (Mup_detect_wave(sound_header, 4))
				{
					mup_file_type = "wav";
					Mup_parse_wav(sound_header, (int)read_size, &mup_file_size, &mup_num_of_music_ch, &mup_music_sample_rate, &mup_music_bit_rate, &mup_file_offset);
					mup_music_length = Mup_calc_wav_length(mup_file_size, mup_music_bit_rate);
				}
				else
					mup_file_type = "unsupported";

				while(true)
				{
					if (mup_file_type == "wav")
					{
						log_num = Log_log_save(mup_play_thread_string, "File_load_from_file_with_range()...", 1234567890, false);
						result = File_load_from_file_with_range(file_name, sound_buffer[buffer_num], mup_sound_fs_out_buffer_size / 2, mup_file_offset, &read_size, dir_name, fs_handle, fs_archive);
						Log_log_add(log_num, result.string, result.code, false);

						mup_file_offset += (int)read_size;
						raw_buffer_size = read_size;
					}
					else if (mup_file_type == "mp3")
					{
						log_num = Log_log_save(mup_play_thread_string, "File_load_from_file_with_range()...", 1234567890, false);
						result = File_load_from_file_with_range(file_name, sound_in_buffer, mp3_input_size, mup_file_offset, &read_size, dir_name, fs_handle, fs_archive);
						Log_log_add(log_num, result.string, result.code, false);

						if((int)read_size - (block_size * 4) <= 0)
							mup_file_offset += (int)read_size;
						else
							mup_file_offset += (int)read_size - (block_size * 4);

						if (result.code == 0)
						{
							free(mp3_info.buffer);
							mp3_info.buffer = NULL;
							log_num = Log_log_save(mup_play_thread_string, "mp3dec_load_buf()...", 1234567890, false);
							mp3_result = mp3dec_load_buf(&mp3_decode, (const uint8_t*)sound_in_buffer, read_size, &mp3_info, NULL, NULL, mup_sound_fs_out_buffer_size / 2);
							Log_log_add(log_num, "decoded : " + std::to_string((mp3_info.samples * 2) / 1024) + "KB ", 1234567890, false);

							if(mp3_result != 0)
							{
								Log_log_add(log_num, Err_query_template_summary(MINIMP3_RETURNED_NOT_SUCCESS), mp3_result, false);
								Err_set_error_show_flag(true);
								Err_set_error_message(Err_query_template_summary(MINIMP3_RETURNED_NOT_SUCCESS), "minimp3 error code : " + std::to_string(mp3_result), mup_play_thread_string, MINIMP3_RETURNED_NOT_SUCCESS);
							}

							memset(sound_buffer[buffer_num], 0x0, mup_sound_fs_out_buffer_size / 2);
							memcpy((void*)(sound_buffer[buffer_num]), (void*)mp3_info.buffer, mp3_info.samples * 2);
							if((mp3_info.samples * 2) >= (0x1200 * (((size_t)mp3_input_size / block_size) - 3)))
							{
								for(int i = 0, k = 4; i < 4; i++, k--)
								{
									if((mp3_info.samples * 2) >= (0x1200 * (((size_t)mp3_input_size / block_size) - i)) && (mp3_info.samples * 2) - (0x1200 * k) > 0)
									{
										memmove((void*)sound_buffer[buffer_num], (void*)(sound_buffer[buffer_num] + (0x1200 * k)), ((mp3_info.samples * 2) - (0x1200 * k)));
										raw_buffer_size = (mp3_info.samples * 2) - (0x1200 * k);
										break;
									}
								}
							}
							else if(mp3_info.samples * 2 == 0)
								raw_buffer_size = 0;
							else
								raw_buffer_size = mp3_info.samples * 2;
						}
					}
					else
						break;

					if(result.code != 0)
						Log_log_save(mup_play_thread_string, "File_load_from_file()..." + result.string, result.code, false);

					while (true)
					{
						usleep(3000);
						csndIsPlaying(31, &status);
						if (status == 0 || !mup_play_thread_run || mup_stop_request || mup_seek_request || mup_change_music_request)
							break;
					}

					if (!mup_play_thread_run || mup_stop_request || read_size <= 0 || mup_seek_request || mup_change_music_request)
					{
						CSND_SetPlayState(31, 0);
						CSND_UpdateInfo(0);
						if(mup_seek_request)
						{
							mup_count_request = false;
							mup_seek_request = false;
							continue;
						}
						else
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
					if (mup_file_type == "wav")
						mup_bar_pos = 1000.0 * mup_music_length * ((mup_file_offset - (int)read_size) / (double)mup_file_size);
					else if (mup_file_type == "mp3")
						mup_bar_pos = 1000.0 * mup_music_length * ((mup_file_offset - ((int)read_size - block_size * 3)) / (double)mup_file_size);

					mup_count_request = true;
				}
		  }

			mup_count_request = false;
			if (!mup_loop_request && !mup_change_music_request)
				mup_play_request = false;
		}
		else
			usleep(ACTIW_THREAD_SLEEP_TIME);

		while (mup_thread_suspend && !mup_loop_request && !mup_change_music_request)
			usleep(INACTIW_THREAD_SLEEP_TIME);
	}

	free(mp3_info.buffer);
	free(sound_in_buffer);
	free(sound_header);
	mp3_info.buffer = NULL;
	sound_in_buffer = NULL;
	sound_header = NULL;
	for (int i = 0; i < 2; i++)
	{
		linearFree(sound_buffer[i]);
		sound_buffer[i] = NULL;
	}

	Log_log_save(mup_play_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mup_exit(void)
{
	Log_log_save(mup_exit_string, "Exiting...", 1234567890, DEBUG);
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
		log_num = Log_log_save(mup_exit_string, "threadJoin()" + std::to_string(i) + "/1...", 1234567890, DEBUG);

		if(i == 0)
			result.code = threadJoin(mup_play_thread, time_out);
		else if(i == 1)
			result.code = threadJoin(mup_timer_thread, time_out);

		if (result.code == 0)
			Log_log_add(log_num, Err_query_template_summary(0), result.code, DEBUG);
		else
		{
			failed = true;
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, DEBUG);
		}
	}

	threadFree(mup_play_thread);
	threadFree(mup_timer_thread);

	if (failed)
		Log_log_save(mup_exit_string, "[Warn] Some function returned error.", 1234567890, DEBUG);
}

void Mup_init(void)
{
	Log_log_save(mup_init_string, "Initializing...", 1234567890, DEBUG);
	bool failed = false;
	int log_num;
	Result_with_string result;

	Draw_progress("0/1 [Mup] Initializing service...");

	log_num= Log_log_save(mup_init_string, "csndInit()...", 1234567890, DEBUG);
	result.code = csndInit();
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, DEBUG);
	else
	{
		failed = true;
		Err_set_error_message("csndInit() failed.", "", mup_init_string, result.code);
		Err_set_error_show_flag(true);
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, DEBUG);
	}

	Draw_progress("1/1 [Mup] Starting threads...");
	if (!failed)
	{
		mup_play_thread_run = true;
		mup_timer_thread_run = true;
		mup_play_thread = threadCreate(Mup_play_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, -1, false);
		mup_timer_thread = threadCreate(Mup_timer_thread, (void*)(""), STACKSIZE, PRIORITY_REALTIME, -1, false);
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
	Log_log_save(mup_init_string, "Initialized", 1234567890, DEBUG);
}

void Mup_main(void)
{
	float text_red, text_green, text_blue, text_alpha;
	float red[2], green[2], blue[2], alpha[2];

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
		|| mup_pre_select_file_request != mup_select_file_request || mup_pre_shuffle_request != mup_shuffle_request || mup_pre_allow_sleep != mup_allow_sleep)
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
		mup_need_reflesh = true;
	}

	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE) || Expl_query_need_reflesh())
		mup_need_reflesh = true;

	if(mup_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		Draw(mup_msg[0] + std::to_string(mup_music_bit_rate / 1000) + mup_msg[1], 0, 0.0, 20.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[2] + std::to_string((double)mup_music_sample_rate / 1000.0).substr(0, std::to_string((double)mup_music_sample_rate / 1000.0).length() - 4) + "Kbps", 0, 0.0, 35.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[3] + std::to_string(mup_num_of_music_ch) + mup_msg[4], 0, 0.0, 50.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[5] + Sem_convert_seconds_to_time(mup_music_length), 0, 0.0, 65.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw(mup_msg[7] + mup_file_type, 0, 0.0, 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, 105.0 + (i * 60.0), 60.0, 50.0, 50.0);
			Draw(mup_msg[16 + i], 0, 107.5 + (i * 60.0), 80.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		}

		Draw(mup_ver, 0, 0.0, 0.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);
		Draw(Sem_convert_seconds_to_time(mup_bar_pos / 1000) + " / " + Sem_convert_seconds_to_time(mup_music_length), 0, 12.5, 105.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
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
			Draw_expl(mup_msg[8]);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		mup_need_reflesh = false;
	}
	else
		gspWaitForVBlank();
}

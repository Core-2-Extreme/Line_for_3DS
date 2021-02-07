#include <3ds.h>
#include <unistd.h>
#include <string>
#include <malloc.h>
#include "citro2d.h"

#include "hid.hpp"
#include "draw.hpp"
#include "file.hpp"
#include "log.hpp"
#include "error.hpp"
#include "menu.hpp"
#include "types.hpp"
#include "setting_menu.hpp"
#include "mic.hpp"
#include "music_player.hpp"
#include "util.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
}

/*For draw*/
bool mic_need_reflesh = false;
/*---------------------------------------------*/

bool mic_main_run = false;
bool mic_thread_run = false;
bool mic_already_init = false;
bool mic_thread_suspend = true;
bool mic_start_record_request = false;
bool mic_stop_record_request = false;
bool mic_encode_request = false;
bool mic_encoding = false;
u8* mic_buffer[3] = { NULL, NULL, NULL, };
u32 mic_buffer_size = 0x80000;
int mic_buffer_offset[2] = { 0, 0, };
int mic_buffer_num = 0;
double mic_record_time = 0.0;
std::string mic_msg[MIC_NUM_OF_MSG];
std::string mic_format = "mp4";
std::string mic_ver = "v1.1.0";
std::string mic_record_thread_string = "Mic/Record thread";
std::string mic_encode_thread_string = "Mic/Encode thread";
std::string mic_init_string = "Mic/Init";
std::string mic_exit_string = "Mic/Exit";
std::string mic_dir_path = "";
std::string mic_file_name = "";
Thread mic_record_thread, mic_encode_thread;


bool Mic_query_init_flag(void)
{
	return mic_already_init;
}

bool Mic_query_running_flag(void)
{
	return mic_main_run;
}

void Mic_set_msg(int msg_num, std::string msg)
{
	if (msg_num >= 0 && msg_num <= MIC_NUM_OF_MSG)
		mic_msg[msg_num] = msg;
}

void Mic_resume(void)
{
	mic_thread_suspend = false;
	mic_main_run = true;
	mic_need_reflesh = true;
	Menu_suspend();
}

void Mic_suspend(void)
{
	mic_thread_suspend = true;
	mic_main_run = false;
	Menu_resume();
}

void Mic_encode_thread(void* arg)
{
	Log_log_save(mic_encode_thread_string, "Thread started.", 1234567890, false);
	int log_num;
	Result_with_string result;

	File_save_to_file(".", NULL, 0, "/Line/sound/", true);

	while (mic_thread_run)
	{
		if (mic_encode_request)
		{
			mic_encode_request = false;
			mic_encoding = true;
			
			log_num = Log_log_save(mic_encode_thread_string, "Util_encode_audio()...", 1234567890, false);
			result = Util_encode_audio(mic_buffer_offset[mic_buffer_num], mic_buffer[mic_buffer_num], UTIL_ENCODER_0);
			Log_log_add(log_num, result.string, result.code, false);
			if(result.code != 0)
			{
				Err_set_error_show_flag(true);
				Err_set_error_message(result.string, result.error_description, mic_encode_thread_string, result.code);
			}

			mic_encoding = false;
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME / 2);
	}
	Log_log_save(mic_encode_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mic_record_thread(void* arg)
{
	Log_log_save(mic_record_thread_string, "Thread started.", 1234567890, false);
	int buffer_num = 0;
	int log_num;
	int count = 0;
	u32 buffer_pos = 0;
	u32 buffer_offset = 0;
	u32 sample_size = 0;
	Result_with_string result;

	File_save_to_file(".", NULL, 0, "/Line/sound/", true);

	while (mic_thread_run)
	{
		if (mic_start_record_request)
		{
			aptSetSleepAllowed(false);
			mic_buffer[0] = (u8*)malloc(mic_buffer_size);
			mic_buffer[1] = (u8*)malloc(mic_buffer_size);
			if (mic_buffer[0] == NULL || mic_buffer[1] == NULL)
			{
				Err_set_error_message("[Error] Out of memory.", "Couldn't allocate memory.", mic_record_thread_string, OUT_OF_MEMORY);
				Err_set_error_show_flag(true);
				Log_log_save(mic_record_thread_string, "[Error] Out of memory.", OUT_OF_MEMORY, false);
			}
			else
			{
				memset(mic_buffer[0], 0x0, mic_buffer_size);
				memset(mic_buffer[1], 0x0, mic_buffer_size);
				mic_dir_path = "/Line/sound/" + Menu_query_time(1) + "/";
				mic_file_name = Menu_query_time(2) + "." + mic_format;
				mic_record_time = 0;
				buffer_offset = 0;
				buffer_pos = 0;
				buffer_num = 0;
				count = 0;
				sample_size = micGetSampleDataSize();

				File_save_to_file(".", NULL, 0, mic_dir_path, true);//create directory

				log_num = Log_log_save(mic_record_thread_string, "Util_create_file()...", 1234567890, false);
				result = Util_create_output_file(mic_dir_path + mic_file_name, UTIL_ENCODER_0);
				Log_log_add(log_num, result.string, result.code, false);
				if(result.code != 0)
				{
					Err_set_error_show_flag(true);
					Err_set_error_message(result.string, result.error_description, mic_record_thread_string, result.code);
					mic_stop_record_request = true;
				}

				log_num = Log_log_save(mic_record_thread_string, "Util_init_audio_encoder()...", 1234567890, false);
				if(mic_format == "mp4")
				{
					result = Util_init_audio_encoder(AV_CODEC_ID_AAC, 32730, 32000, 128000, UTIL_ENCODER_0);
					if(result.code == 0)
						result = Util_write_header(UTIL_ENCODER_0);
				}
				else if(mic_format == "mp2")
					result = Util_init_audio_encoder(AV_CODEC_ID_MP2, 32730, 32000, 128000, UTIL_ENCODER_0);
				else
					result = Util_init_audio_encoder(AV_CODEC_ID_AC3, 32730, 32000, 128000, UTIL_ENCODER_0);

				Log_log_add(log_num, result.string, result.code, false);
				if(result.code != 0)
				{
					Err_set_error_show_flag(true);
					Err_set_error_message(result.string, result.error_description, mic_record_thread_string, result.code);
					mic_stop_record_request = true;
				}

				log_num = Log_log_save(mic_record_thread_string, "MICU_StartSampling()...", 1234567890, false);
				result.code = MICU_StartSampling(MICU_ENCODING_PCM16_SIGNED, MICU_SAMPLE_RATE_32730, 0, sample_size, true);
				Log_log_add(log_num, "", result.code, false);
				if(result.code != 0)
				{
					Err_set_error_show_flag(true);
					Err_set_error_message("MICU_StartSampling() failed", "", mic_record_thread_string, result.code);
					mic_stop_record_request = true;
				}

				while (true)
				{
					usleep(10000);
					if(count > 10)//update screen
					{
						count = 0;
						mic_need_reflesh = true;
					}
					else
						count++;

					if (buffer_pos != micGetLastSampleOffset())
					{
						if(buffer_pos > micGetLastSampleOffset())
						{
							while(mic_encoding)
								usleep(100000);
	
							mic_buffer_num = buffer_num;
							mic_buffer_offset[buffer_num] = buffer_offset;
							mic_encode_request = true;
	
							buffer_pos = 0;
							buffer_offset = 0;
							if(buffer_num == 0)
								buffer_num = 1;
							else
								buffer_num = 0;
						}
						else
						{
							buffer_pos = micGetLastSampleOffset();
							memcpy((void*)(mic_buffer[buffer_num] + buffer_offset), (void*)(mic_buffer[2] + buffer_offset), (buffer_pos - buffer_offset));
							mic_record_time += (buffer_pos - buffer_offset);
							buffer_offset += (buffer_pos - buffer_offset);
						}
					}

					if (mic_stop_record_request)
					{
						log_num = Log_log_save(mic_record_thread_string, "MICU_StopSampling()...", 1234567890, false);
						result.code = MICU_StopSampling();
						Log_log_add(log_num, "", result.code, false);

						while(mic_encoding)
							usleep(100000);

						mic_buffer_num = buffer_num;
						mic_buffer_offset[buffer_num] = buffer_offset;
						mic_encode_request = true;

						do
							usleep(100000);
						while(mic_encoding);
						/**chunk_size = (int)buffer_offset + 36;
						memcpy((void*)header, (void*)riff, 0x4);
						memcpy((void*)(header + 4), (void*)(chunk_size), 0x4);
						memcpy((void*)(header + 8), (void*)wave, 0x4);
						memcpy((void*)(header + 12), (void*)fmt, 0x4);
						memset((void*)(header + 16), 0x10, 0x1); //Subchunk1Size 16 for PCM
						memset((void*)(header + 20), 0x1, 0x1); //AudioFormat PCM = 1
						memset((void*)(header + 22), 0x1, 0x1); //NumChannels Mono = 1, Stereo = 2
						memset((void*)(header + 24), 0xE8, 0x1); //SampleRate
						memset((void*)(header + 25), 0x3F, 0x1); //SampleRate
						memset((void*)(header + 28), 0xD0, 0x1); //ByteRate == SampleRate * NumChannels * BitsPerSample/8
						memset((void*)(header + 29), 0x7F, 0x1); //ByteRate == SampleRate * NumChannels * BitsPerSample/8
						memset((void*)(header + 32), 0x2, 0x1); //BlockAlign == NumChannels * BitsPerSample/8
						memset((void*)(header + 34), 0x10, 0x1); //BitsPerSample 8 bits = 8, 16 bits = 16
						memcpy((void*)(header + 36), (void*)data, 0x4);
						*chunk_size = (int)buffer_offset;
						memcpy((void*)(header + 40), (void*)chunk_size, 0x4);

						log_num = Log_log_save(mic_record_thread_string, "File_save_to_file()...", 1234567890, false);
						result = File_save_to_file(file_name, (u8*)header, 44, dir_path, true);
						Log_log_add(log_num, "", result.code, false);

						log_num = Log_log_save(mic_record_thread_string, "File_save_to_file()...", 1234567890, false);
						result = File_save_to_file(file_name, (u8*)fs_buffer, buffer_offset, dir_path, false);
						Log_log_add(log_num, "", result.code, false);*/

						break;
					}
				}
			}

			Util_exit_audio_encoder(UTIL_ENCODER_0);
			Util_close_output_file(UTIL_ENCODER_0);
			free(mic_buffer[0]);
			free(mic_buffer[1]);
			mic_buffer[0] = NULL;
			mic_buffer[1] = NULL;
			mic_start_record_request = false;
			mic_stop_record_request = false;
			aptSetSleepAllowed(true);
		}
		else
			usleep(ACTIVE_THREAD_SLEEP_TIME);

		while (mic_thread_suspend)
			usleep(INACTIVE_THREAD_SLEEP_TIME);
	}
	Log_log_save(mic_record_thread_string, "Thread exit.", 1234567890, false);
	threadExit(0);
}

void Mic_exit(void)
{
	Log_log_save(mic_exit_string, "Exiting...", 1234567890, FORCE_DEBUG);
	u64 time_out = 10000000000;
	int log_num;
	Result_with_string result;

	mic_already_init = false;
	mic_thread_suspend = false;
	mic_thread_run = false;
	mic_stop_record_request = true;
	mic_start_record_request = false;

	Draw_progress("[Mic] Exiting...");
	log_num = Log_log_save(mic_exit_string, "threadJoin()0/1...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(mic_record_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);

	log_num = Log_log_save(mic_exit_string, "threadJoin()1/1...", 1234567890, FORCE_DEBUG);
	result.code = threadJoin(mic_encode_thread, time_out);
	if (result.code == 0)
		Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
	else
		Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);

	threadFree(mic_record_thread);
	threadFree(mic_encode_thread);

	MICU_SetPower(false);
	micExit();
	free(mic_buffer[2]);
}

Result_with_string Mic_load_msg(std::string lang)
{
	u8* fs_buffer = NULL;
	u32 read_size;
	std::string setting_data[128];
	Result_with_string result;
	fs_buffer = (u8*)malloc(0x2000);

	result = File_load_from_rom("mic_" + lang + ".txt", fs_buffer, 0x2000, &read_size, "romfs:/gfx/msg/");
	if (result.code != 0)
	{
		free(fs_buffer);
		return result;
	}

	result = Sem_parse_file((char*)fs_buffer, MIC_NUM_OF_MSG, setting_data);
	if (result.code != 0)
	{
		free(fs_buffer);
		return result;
	}

	for (int k = 0; k < MIC_NUM_OF_MSG; k++)
		Mic_set_msg(k, setting_data[k]);

	free(fs_buffer);
	return result;
}

void Mic_init(void)
{
	Log_log_save(mic_init_string, "Initializing...", 1234567890, FORCE_DEBUG);
	bool failed = false;
	int log_num;
	Result_with_string result;
	mic_buffer[2] = (u8*)memalign(0x1000, mic_buffer_size);
	if (mic_buffer[2] == NULL)
	{
		Err_set_error_message("Out of memory.", "Couldn't allocate memory.", mic_init_string, OUT_OF_MEMORY);
		Err_set_error_show_flag(true);
		Log_log_save(mic_init_string, "[Error] Out of memory. ", OUT_OF_MEMORY, false);
		failed = true;
	}

	Draw_progress("[Mic] Initializing mic...");
	if (!failed)
	{
		log_num = Log_log_save(mic_init_string, "micInit()...", 1234567890, FORCE_DEBUG);
		result.code = micInit(mic_buffer[2], mic_buffer_size);
		if (result.code == 0)
		{
			MICU_SetAllowShellClosed(true);
			Log_log_add(log_num, Err_query_template_summary(0), result.code, FORCE_DEBUG);
		}
		else
		{
			failed = true;
			Err_set_error_message("micInit() failed.", "", mic_init_string, result.code);
			Err_set_error_show_flag(true);
			Log_log_add(log_num, Err_query_template_summary(-1024), result.code, FORCE_DEBUG);
		}
	}

	Draw_progress("[Mic] Starting threads...");
	if (!failed)
	{
		mic_thread_run = true;
		mic_record_thread = threadCreate(Mic_record_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 1, false);
		mic_encode_thread = threadCreate(Mic_encode_thread, (void*)(""), STACKSIZE, PRIORITY_NORMAL, 0, false);
	}

	Mic_resume();
	mic_already_init = true;
	Log_log_save(mic_init_string, "Initialized", 1234567890, FORCE_DEBUG);
}

void Mic_main(void)
{
	float text_red;
	float text_green;
	float text_blue;
	float text_alpha;
	double draw_x;
	double draw_y;
	Hid_info key;

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

	Hid_query_key_state(&key);
	Log_main();
	if(Draw_query_need_reflesh() || !Sem_query_settings(SEM_ECO_MODE))
		mic_need_reflesh = true;

	Hid_key_flag_reset();

	if(mic_need_reflesh)
	{
		Draw_frame_ready();
		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(0, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(0, true, 2, 1.0, 1.0, 1.0);

		Draw_top_ui();

		if (Sem_query_settings(SEM_NIGHT_MODE))
			Draw_screen_ready_to_draw(1, true, 2, 0.0, 0.0, 0.0);
		else
			Draw_screen_ready_to_draw(1, true, 2, 1.0, 1.0, 1.0);

		Draw(mic_ver, 0, 0.0, 0.0, 0.4, 0.4, 0.0, 1.0, 0.0, 1.0);
		Draw(mic_msg[3] + Menu_query_time(1) + "/", 0, 75.0, 35.0, 0.4, 0.4, 1.0, 0.0, 0.0, 1.0);
		if(mic_start_record_request)
			Draw(mic_msg[2], 0, 15.0, 75.0, 0.6, 0.6, 1.0, 0.0, 0.0, 1.0);

		draw_x = 105.0;
		draw_y = 60.0;
		for (int i = 0; i < 2; i++)
		{
			Draw_texture(Square_image, weak_aqua_tint, 0, draw_x, draw_y, 50.0, 50.0);
			Draw(mic_msg[i], 0, (draw_x + 2.5), draw_y + 20.0, 0.425, 0.425, text_red, text_green, text_blue, text_alpha);
			draw_x += 60.0;
		}
		Draw(Sem_convert_seconds_to_time((double)mic_record_time / (32730 * 2.0)), 0, 102.5, 105.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);
		if(mic_start_record_request)
			Draw("format(Y) " + mic_format, 0, 102.5, 125.0, 0.5, 0.5, text_red, text_green, text_blue, 0.25);
		else
			Draw("format(Y) " + mic_format, 0, 102.5, 125.0, 0.5, 0.5, text_red, text_green, text_blue, text_alpha);

		Draw_bot_ui();
		Draw_touch_pos();

		Draw_apply_draw();
		mic_need_reflesh = false;
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
	else
	{
		if (key.p_start || (key.p_touch && key.touch_x >= 110	&& key.touch_x <= 230 && key.touch_y >= 220 && key.touch_y <= 240))
			Mic_suspend();
		else if (key.p_a || (key.p_touch && key.touch_x >= 105 && key.touch_x <= 154 && key.touch_y >= 60 && key.touch_y <= 109))
			mic_start_record_request = true;
		else if (key.p_b || (key.p_touch && key.touch_x >= 165 && key.touch_x <= 214 && key.touch_y >= 60 && key.touch_y <= 109))
			mic_stop_record_request = true;
		else if (key.p_y && !mic_start_record_request)
		{
			if(mic_format == "mp4")
				mic_format = "ac3";
			else if(mic_format == "ac3")
				mic_format = "mp2";
			else
				mic_format = "mp4";
			
			mic_need_reflesh = true;
		}
	}
}
